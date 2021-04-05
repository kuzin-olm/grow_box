void HandlerSensDHT() {
  
  //обработка данных с DHT (температура воздуха и влажность в боксе)
  float h = dht.readHumidity();
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
      //залить в бокс немного воды (прикинуть коэф. требуемого объема воды от дельты по влажности)
      //ну и время на испарение
    }
  }
  
  float t = dht.readTemperature();
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
