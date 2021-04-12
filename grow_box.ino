#include <math.h>

//init led&key module
#include <TM1638.h>
TM1638 module(2, 3, 4);     //(dio, clk, stb) pins

//init DHT sensor
#include <DHT.h>        //требует либ <Adafruit_Sensor.h>
#define DHTPIN 22
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321    DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

//подразумевается, что датчики стоят параллельно помпам
//чтобы каждая помпа имела свою обратную связь(ОС)
#define PUMP_PIN 45         //подключение помп с пина
#define PUMP_QUAN 1         //кол-во помп
#define SWITCH_LEVEL 1      //уровень управления реле 0/1

#define SENSOR_HMDT_PIN 0   //Dry: [520 430] Wet: [430 350] Water: [350 260] (sens v1.0)
#define SENSOR_HMDT_QUAN PUMP_QUAN

//свет и вентилятор
#define FUN_PIN 42          //управление реле вентилятора
#define GROW_LIGHT_PIN 44   //управление реле света
                            //т.к. led AC 220V и утановленны на радиатор, то можно использовать для нагрева
                            //также реле должно быть опторазвязано (от греха :) )
#define HMDT_PUMP_PIN 43    //помпа для поддержания влаги в боксе
#define RELAY_ON 1          //уровень управления реле (если один тип реле, то можно поставить равным SWITCH_LEVEL)
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

//дефолтные настройки для помп, чтобы потом можно было отдельно для разных горшков настраивать
struct Pump {
  public:
  byte pin;                               //пин, на котором помпа
  byte sens_pin;                          //пин, на котором датчик (ОС от помпы)
  bool is_active = false;                 //текущее состояние
  long start_pumping = 0;                 //ms, последнее включение
  byte hmdt_trigger = 60;                 //%, уровень влажность для вкл/выкл
  byte water_per_sec = 50;                //мл/с, = 3 л/мин прокачивает 5В помпа
  unsigned int time_pumping = 2 * 1000;   //ms, время работы помпы

  Pump(){}
  Pump(byte pin){ this->pin = pin; }
  Pump(byte pin, byte sens_pin){ this->pin = pin; this->sens_pin = sens_pin; }
};
Pump pumps[PUMP_QUAN];


const unsigned int to_wait_flow_water = 10 * 1000;     //ms, опрос после пролива (ожидание текучести воды)
const byte time_refresh_disp = 2;                      //sec, промежуток времени, через который обновляеся дисплей
const byte dht_hmdt_h = 70;                            //%, влжст в теплице, верх порог
const byte dht_hmdt_l = 50;                            //%, влжст в теплице, низ порог
const float dht_temp = 29.44;                          //t°C, шикарная тмпртра для теплицы    (t°F-32)*5/9=t°C (85°F)

Pump pump_box(HMDT_PUMP_PIN);                     //помпа в боксе
long to_wait_flow_water_box = 0;   //ms, ожидание повышения влаги в боксе

byte last_refresh_disp = 0;        //sec, последнее обновление дисплея
byte disp_sens = 0;                //счетчик датчиков для обновления дисплея
bool disp_on = true;               //вкл/выкл дисплея
byte disp_mode = 0;                //выбор экранчика отображения


//преобразование аналоговой величины с емкостного датчика влажности (sens v1.2) в % влажности
byte read_hmdt(int x) {
  int res = x * (-0.257) + 163.452; //kx+b, где k=-0.257 b=163.452  (эксперементально 246=100% 636=0%)
  return (res > 100) ? 100 : (res < 0) ? 0 : res;
}

float getMassWater(float curr_temp, float curr_hmdt, float volume) {
  //кол-во воды, необходимое для повышения влажности до требуемого уровня dht_hmdt_h
  //
  //парциальное давление паров воды (p насыщенного пара):
  //  p(н.п.) = 6.1078 * exp^( 17.08*t/(234.175+t) ) * 100  [Па]
  float p_np = 6.1078 * exp( 17.08 * curr_temp/(234.175 + curr_temp) ) * 100;
  //
  //подставим  p(н.п.)  в уравнение К-М и получим  ro(н.п.)
  //по формуле Клапейрона-Менделеева
  //абс.влжность воздуха   ro(н.п.) = m/V = ( M * p(н.п.) )/( R * T )   [г/м^3]
  //где:
  //  m - масса воды [г]
  //  V - объем помещения [м^3]
  //  p - парциальное давление паров воды [Па] (необходимо расчитать - зависит от t)
  const float M = 18.02;          //  M - молярная масса воды [г/моль] (18.02)
  const float R = 8.314;          //  R - универсальная газовая постоянная [Дж/(моль*К)] (8.314)
  float T = curr_temp + 273;      //  T - температура [К] (t + 273)
  
  float ro_np = (M * p_np) / (R * T);
  //
  //относительная влажность:
  //  фи = ( ro(п)/ro(н.п.) ) * 100% 
  //
  //плотность пара при текущей влажности
  //  ro(п) = (фи * ro(н.п.))/100%  [г/м^3]
  //
  //исходя из  ro(п)=m/V  => дельта ro(п) = (дельта m)/V :
  //  дельта ro(п) = (фи2 - фи1) * ro(н.п.) / 100%  [г/м^3]
  //  дельта m = (фи2 - фи1) * ro(н.п.) * V / 100%  [г]
  //
  float m = (dht_hmdt_h - curr_hmdt) * ro_np * volume / 100;
  //
  //  дельта m [г] — та масса воды, которую надо пролить
  return m;
}

float getEvaporationRateOfWater(float curr_temp, float curr_hmdt, float S) {
  //скорость испарения воды с поверхности [г/ч]
  //
  const float p_rt = 0.0075;    // 1Па = 0,00750062 мм.рт.ст
  //  парциальное давление паров воды (p насыщенного пара):
  //    p(н.п.) = 6.1078 * exp^( 17.08*t/(234.175+t) ) * 100  [Па]
  float p_np = 6.1078 * exp( 17.08 * curr_temp/(234.175 + curr_temp) ) * 100;
  //
  //  W = S * (a + 0.0174 * v) * (P1 - P2) * 760/Pбар   [кг/ч]
  //  где:
  //    S - площадь поверхности испарения
  const float a = 0.022;                    // a - фактор скорости подвижности окрж.воздуха (0,022 при температуре от 0-30; 0,028 - при температуре от 30-40)
  const float v = 0.00;                     // v - относительная скорость движения воздуха над поверхностью испарения (0,2-0,4 м/с - естественное движение, 0,5 м/с - принудительное)
  float P1 = p_np * p_rt;                   // P1 - упругость водяного пара, соответствующая полному насыщению воздуха при его температуре (по сути в боксе) [мм.рт.ст]
  float P2 = p_np * curr_hmdt / 100 * p_rt; // P2 - упругость водяного пара воздуха в помещении при температуре и влажности окрж.среды (по сути в боксе) [мм.рт.ст] => P2 = p(н.п.)*фи/100
  const float Pbar = 750.00;                // Pбар - давление для данного географического пункта [мм.рт.ст]
  //
  float W = S * (a + 0.0174 * v) * (P1 - P2) * 760 / Pbar * 1000; // [г/ч]
  return W;
}


void setup() {
  Serial.begin(9600);

  module.setDisplayToString("RUN  SET");
  delay(1000);

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

  //set свет,кулер,помпа - OFF
  pinMode(FUN_PIN, OUTPUT);
  digitalWrite(FUN_PIN, !RELAY_ON);
  
  pinMode(GROW_LIGHT_PIN, OUTPUT);
  digitalWrite(GROW_LIGHT_PIN, !RELAY_ON);
  
//  pump_box.pin = HMDT_PUMP_PIN;
  pinMode(pump_box.pin, OUTPUT);
  digitalWrite(pump_box.pin, !RELAY_ON);


  module.clearDisplay();
  module.setLED(1, disp_mode);
}

void loop() {

  HandlerSensPumps();

  HandlerSensDHT();

  HandlerBtnModule();

  UpdateDisplay();

  Serial.println(" ");
  delay(500);
}
