void HandlerBtnModule() {
  
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
    disp_mode = (++disp_mode > 2) ? 0 : disp_mode++;  //если в update_display добавили еще экран, то тут тоже надо поменять
    module.setLED(1, disp_mode);
  }
  
}
