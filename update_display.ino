void UpdateDisplay() {
  
  //обновление дисплея
  if (abs(millis() / 1000 - last_refresh_disp) > time_refresh_disp) {
    last_refresh_disp = millis() / 1000;
    if (disp_on) {
      if (disp_mode == 0) {
        byte hmdt = read_hmdt(analogRead(pumps[disp_sens].sens_pin));
        module.clearDisplay();
        module.setDisplayToString("d" + String(disp_sens + 1) + "  " + String(hmdt));
        disp_sens = (++disp_sens > sizeof(pumps) / sizeof(*pumps) - 1) ? 0 : disp_sens++;
      }
      //влажность с датчика DHT
      if (disp_mode == 1) {
        int h = dht.readHumidity() * 100;
        module.clearDisplay();
        module.setDisplayToString("hdt " + String(h), 1 << 2);
      }
      //температура с датчика DHT
      if (disp_mode == 2) {
        int t = dht.readTemperature() * 100;
        module.clearDisplay();
        module.setDisplayToString("tpt " + String(t), 1 << 2);
      }
    } else {
      module.clearDisplay();
    }
  }
  
}
