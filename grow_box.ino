#include <TM1638.h>
TM1638 module(2, 3, 4);     //(dio, clk, stb) pins

#define PUMP_PIN 45         //подключение помп с пина
#define PUMP_QUAN 1         //кол-во помп
#define SWITCH_LEVEL 1      //уровень управления реле 0/1

#define SENSOR_HMDT_PIN 0   //Dry: [520 430] Wet: [430 350] Water: [350 260] (sens v1.0)
#define SENSOR_HMDT_QUAN PUMP_QUAN


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


//преобразование аналоговой величины с емкостного датчика влажности (sens v1.2)
byte read_hmdt(int x) {
  int res = x * (-0.257) + 163.452; //kx+b, где k=-0.257 b=163.452  (эксперементально 246=100% 636=0%)
  return (res > 100) ? 100 : (res < 0) ? 0 : res;
}



void setup() {
  Serial.begin(9600);

  //init помп
  for (byte i = 0; i < PUMP_QUAN; i++) {
    pumps[i].pin = PUMP_PIN + i;
    pinMode(pumps[i].pin, OUTPUT);
    digitalWrite(pumps[i].pin, !SWITCH_LEVEL);    //от греха выключим
  }

  //init датчиков
  for (byte i = 0; i < SENSOR_HMDT_QUAN; i++) {
    pumps[i].sens_pin = SENSOR_HMDT_PIN + i;
    pinMode(pumps[i].sens_pin, INPUT);
  }

  module.clearDisplay();
}

void loop() {
  
  //обойдем помпы (точнее показания с датчиков)
  for (byte i = 0; i < PUMP_QUAN; i++) {
    byte hmdt = read_hmdt(analogRead(pumps[i].sens_pin));

    if (pumps[i].is_active) {
      if (abs(millis() - pumps[i].start_pumping) > time_pumping) {
        Serial.print("выкл -- ");
        pumps[i].start_pumping = millis();
        pumps[i].is_active = false;
        digitalWrite(pumps[i].pin, !SWITCH_LEVEL);
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

    if (abs(last_refresh_disp -  millis() / 1000) > time_refresh_disp) {
      last_refresh_disp = millis() / 1000;

      module.clearDisplay();
      module.setDisplayToString("d" + String(i + 1) + "  " + String(hmdt));
    }

  }
  Serial.println(" ");

  delay(500);
}
