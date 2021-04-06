void HandlerSensDHT() {
  
  //обработка данных с DHT (температура воздуха и влажность в боксе)
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  bool high_hmdt = h > dht_hmdt_h;
  bool low_hmdt = h < dht_hmdt_l;
  if (!high_hmdt & !low_hmdt){
    module.setLED(0, mdl.led6);
    //все норм
  } else {
    module.setLED(1, mdl.led6);
    if (high_hmdt){
      //включить винты на проветривание
    }
    if (low_hmdt){
      //залить в бокс немного воды (через Клапейрона-Менделеева)
      float m = getMassWater(t, h, 1);    // 1 - объем бокса #TODO переделать в константы по x*y*z
      Serial.print(F("-- масса m [г]: "));
      Serial.print(m);
      //скорость испарения в боксе
      float w = getEvaporationRateOfWater(t, h, 0.4); // 0,4 - площадь испарения #TODO переделать в константы по x*y
      Serial.print(F("-- испарение [г/ч]: "));
      Serial.print(w);

      long to_wait_flow_water_box = m/w * 3600 * 1000; //ms, ожидание повышения влаги
      Serial.print(F("-- испарится через [мс]: "));
      Serial.print(to_wait_flow_water_box);
    }
  }
  
  
  bool high_temp = t > (dht_temp * 1.05);
  bool low_temp = t < (dht_temp * 0.95);
  if (!high_temp & !low_temp){
    module.setLED(0, mdl.led7);
    //температура норм
  } else {
    module.setLED(1, mdl.led7);
    if (high_temp){
      //включить винты на проветривание
    }
    if (low_temp){
      //максимально уменьшить приток воздуха
      //включить grow_led - нагреемся от радиаторов
    }
  }
  Serial.print(F("-- Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  
}
