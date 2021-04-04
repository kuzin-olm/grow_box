//init led&key module
#include <TM1638.h>
TM1638 module(2, 3, 4);     //(dio, clk, stb) pins

//init DHT sensor
#include <DHT.h>
#define DHTPIN 20
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321    DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

//подразумевается, что датчики стоят параллельно помпам
//чтобы каждая помпа имела свою обратную связь(ОС)
#define PUMP_PIN 45         //подключение помп с пина
#define PUMP_QUAN 1         //кол-во помп
#define SWITCH_LEVEL 1      //уровень управления реле 0/1

#define SENSOR_HMDT_PIN 0   //Dry: [520 430] Wet: [430 350] Water: [350 260] (sens v1.0)
#define SENSOR_HMDT_QUAN PUMP_QUAN
//----------------------------------------------------------------------------------------------

struct {
  byte btn1 = 1;
  byte btn2 = 2;
  byte btn3 = 4;
  byte btn4 = 8;
  byte btn5 = 16;
  byte btn6 = 32;
  byte btn7 = 64;
  byte btn8 = 128;

  byte led1 = 0;
  byte led2 = 1;
  byte led3 = 2;
  byte led4 = 3;
  byte led5 = 4;
  byte led6 = 5;
  byte led7 = 6;
  byte led8 = 7;
} mdl;  //адреса кнопок и светодиодов

struct Pump {
  byte pin;                   //пин, на котором помпа
  byte sens_pin;              //пин, на котором датчик (ОС от помпы)
  bool is_active = false;     //текущее состояние
  long start_pumping = 0;     //ms, последнее включение
  byte hmdt_trigger = 60;     //%, уровень влажность для вкл/выкл
};
Pump pumps[PUMP_QUAN];


unsigned int time_pumping = 2 * 1000;           //ms, время работы помпы
unsigned int to_wait_flow_water = 10 * 1000;    //ms, ожидание текучести воды в горшках

byte time_refresh_disp = 2;   //sec, обновление дисплея через...
long last_refresh_disp = 0;   //sec, последнее обновление
bool disp_on = true;
byte disp_mode = 0;
byte disp_sens = 0;

//преобразование аналоговой величины с емкостного датчика влажности (sens v1.2) в % влажности
byte read_hmdt(int x) {
  int res = x * (-0.257) + 163.452; //kx+b, где k=-0.257 b=163.452  (эксперементально 246=100% 636=0%)
  return (res > 100) ? 100 : (res < 0) ? 0 : res;
}



void setup() {
  Serial.begin(9600);

  //set помп
  for (byte i = 0; i < PUMP_QUAN; i++) {
    pumps[i].pin = PUMP_PIN + i;
    pinMode(pumps[i].pin, OUTPUT);
    digitalWrite(pumps[i].pin, !SWITCH_LEVEL);    //от греха выключим
  }

  //set датчиков
  for (byte i = 0; i < SENSOR_HMDT_QUAN; i++) {
    pumps[i].sens_pin = SENSOR_HMDT_PIN + i;
    pinMode(pumps[i].sens_pin, INPUT);
  }

  //set датчика влажности и температуры
  dht.begin();
  
  module.clearDisplay();
  module.setLED(1, disp_mode);
}

void loop() {

  //обойдем помпы (точнее показания с датчиков)
  for (byte i = 0; i < PUMP_QUAN; i++) {
    byte hmdt = read_hmdt(analogRead(pumps[i].sens_pin));

    if (pumps[i].is_active) {
      if (abs(millis() - pumps[i].start_pumping) > time_pumping) {
        pumps[i].start_pumping = millis();
        pumps[i].is_active = false;
        digitalWrite(pumps[i].pin, !SWITCH_LEVEL);
        Serial.print("выкл -- ");
      }
    }

    if ((hmdt < pumps[i].hmdt_trigger) & (!pumps[i].is_active)) {
      if (abs(millis() - pumps[i].start_pumping) > to_wait_flow_water) {
        pumps[i].start_pumping = millis();
        pumps[i].is_active = true;
        digitalWrite(pumps[i].pin, SWITCH_LEVEL);
        Serial.print("вкл -- ");
      }
    }

    Serial.print(analogRead(pumps[i].sens_pin));
    Serial.print(" -- ");
    Serial.print(hmdt);
    Serial.print("% -- pupm [" + String(i) + "] is active?: ");
    Serial.print(pumps[i].is_active);
    Serial.print(" -- ");
    Serial.print(pumps[i].start_pumping);
    Serial.print(" -- " + String(millis()) + "ms");
  }


  //обработка данных с DHT (температура воздуха и влажность в боксе)
  float h = dht.readHumidity();
  bool high_hmdt = h > 70; // #TODO в константы уровни
  bool low_hmdt = h < 50;
  if (!high_hmdt & !low_hmdt){
    //все норм
  } else {
    if (high_hmdt){
      //включить винты на проветривание
    }
    if (low_hmdt){
      //залить в бокс немного воды (прикинуть коэф. требуемого объема воды от дельты по влажности)
      //ну и время на испарение
    }
  }
  
  float t = dht.readTemperature();
  bool high_temp = t > (29.44 * 1.05);  // #TODO в константы уровни
  bool low_temp = t < (29.44 * 0.95);
  if (!high_temp & !low_temp){
    //температура норм
  } else {
    if (high_temp){
      //включить винты на проветривание
    }
    if (low_temp){
      //максимально уменьшить приток воздуха
      //включить grow_led - нагреемся от радиаторов
    }
  }

  
  //обработка кнопок
  byte curr_btn = module.getButtons();
  //вкл/выкл дисплея
  if (curr_btn == mdl.btn8) {
    disp_on = !disp_on;
    module.setLED(!disp_on, mdl.led8);    //если дисплей выкл - то светодиод вкл
  }
  //изменение/выбор отображения на дисплее
  if (curr_btn == mdl.btn1) {
    module.setLED(0, disp_mode);
    disp_mode = (++disp_mode > 1) ? 0 : disp_mode++;
    module.setLED(1, disp_mode);
  }

  //обновление дисплея
  if (abs(last_refresh_disp -  millis()) > time_refresh_disp * 1000) {
    last_refresh_disp = millis() / 1000;
    if (disp_on) {
      if (disp_mode == 0) {
        byte hmdt = read_hmdt(analogRead(pumps[disp_sens].sens_pin));
        module.clearDisplay();
        module.setDisplayToString("d" + String(disp_sens + 1) + "  " + String(hmdt));
        disp_sens = (++disp_sens > sizeof(pumps) / sizeof(*pumps) - 1) ? 0 : disp_sens++;
      }
      if (disp_mode == 1) {
        // DHT датчик сюда припилить
      }
    } else {
      module.clearDisplay();
    }
  }

  Serial.println(" ");

  delay(500);
}
