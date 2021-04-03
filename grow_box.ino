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
} mdl;

struct Pump {
  byte pin;
  byte sens_pin;
  bool is_active = false;
  long start_pumping = 0;
  byte hmdt_trigger = 60;
};
Pump pumps[PUMP_QUAN];


unsigned int time_pumping = 2 * 1000;
unsigned int to_wait_flow_water = 10 * 1000;


void setup() {
  
}

void loop() {
  

  delay(500);
}
