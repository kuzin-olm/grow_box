void UpdateDisplay() {
  
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
  
}
