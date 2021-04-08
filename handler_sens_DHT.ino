void HandlerSensDHT() {
  
  // проверка помпы для увлажнения бокса
  if (pump_box.is_active) {
    if (abs(millis() - pump_box.start_pumping) > pump_box.time_pumping) {
      pump_box.is_active = false;
      pump_box.start_pumping = millis();
      digitalWrite(pump_box.pin, !RELAY_ON);      
    }
  }
  
  //обработка данных с DHT (температура воздуха и влажность в боксе)
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  bool high_hmdt = h > dht_hmdt_h;
  bool low_hmdt = h < dht_hmdt_l;
  if (!high_hmdt & !low_hmdt){
    module.setLED(0, mdl.led6);
    //все норм
    if (pump_box.is_active) {
      pump_box.is_active = false;
      pump_box.start_pumping = millis();
      digitalWrite(pump_box.pin, !RELAY_ON);
    }
    
    digitalWrite(FUN_PIN, !RELAY_ON);
    
  } else {
    module.setLED(1, mdl.led6);
    if (high_hmdt){
      //включить винты на проветривание
      digitalWrite(FUN_PIN, RELAY_ON);
    }
    if ((low_hmdt) & (abs(millis() - pump_box.start_pumping) > to_wait_flow_water_box)) {
      //залить в бокс немного воды (через Клапейрона-Менделеева)
      float m = getMassWater(t, h, 1);    // 1 - объем бокса #TODO переделать в константы по x*y*z
      Serial.print(F("-- масса m [г]: "));
      Serial.print(m);
      //скорость испарения в боксе
      float w = getEvaporationRateOfWater(t, h, 0.4); // 0,4 - площадь испарения #TODO переделать в константы по x*y
      Serial.print(F("-- испарение [г/ч]: "));
      Serial.print(w);

      to_wait_flow_water_box = m/w * 3600 * 1000; //ms, ожидание повышения влаги
      Serial.print(F("-- испарится через [мс]: "));
      Serial.print(to_wait_flow_water_box);

      pump_box.is_active = true;
      pump_box.start_pumping = millis();
      float time_pumping = m / pump_box.water_per_sec;     //т.к. плотность воды ~=1, то 1г=1мл
      pump_box.time_pumping = (time_pumping < 1) ? 1000 : time_pumping * 1000;
      digitalWrite(pump_box.pin, RELAY_ON);
    }
  }
  
  
  bool high_temp = t > (dht_temp * 1.05);
  bool low_temp = t < (dht_temp * 0.95);
  if (!high_temp & !low_temp){
    module.setLED(0, mdl.led7);
    //температура норм
    digitalWrite(FUN_PIN, !RELAY_ON);
    digitalWrite(GROW_LIGHT_PIN, !RELAY_ON);
    
  } else {
    module.setLED(1, mdl.led7);
    if (high_temp){
      //включить винты на проветривание
      digitalWrite(FUN_PIN, RELAY_ON);
      digitalWrite(GROW_LIGHT_PIN, !RELAY_ON);
    }
    if (low_temp){
      //максимально уменьшить приток воздуха
      digitalWrite(FUN_PIN, !RELAY_ON);
      //включить grow_led - нагреемся от радиаторов
      digitalWrite(GROW_LIGHT_PIN, RELAY_ON);
    }
  }
  Serial.print(F("-- Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  
}
