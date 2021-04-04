void HandlerSensDHT() {
  
  //обработка данных с DHT (температура воздуха и влажность в боксе)
  float h = dht.readHumidity();
  bool high_hmdt = h > 70; // #TODO в константы уровни
  bool low_hmdt = h < 50;
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
  bool high_temp = t > (29.44 * 1.05);  // #TODO в константы уровни
  bool low_temp = t < (29.44 * 0.95);
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
  
}
