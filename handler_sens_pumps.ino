void HandlerSensPumps() {
  
  //обойдем помпы (точнее показания с датчиков)
  for (byte i = 0; i < PUMP_QUAN; i++) {
    byte hmdt = read_hmdt(analogRead(pumps[i].sens_pin));

    if (pumps[i].is_active) {
      if (abs(millis() - pumps[i].start_pumping) > time_pumping) {
        pumps[i].start_pumping = millis();
        pumps[i].is_active = false;
        digitalWrite(pumps[i].pin, !SWITCH_LEVEL);
        Serial.print("выкл -- ");
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
  }
  
}
