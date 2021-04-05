# grow box 

для макета теплицы:
- полив горшков в зависимости от влажности почвы
- поддержание требуемых влажности воздуха и температуры
- TODO контроль освещенности (либо по датчику, либо от модуля реального времени)

## arduino

- <a href="https://a.aliexpress.com/_AanRRW">ATmega2560 + ESP8266</a>
- <a href="https://a.aliexpress.com/_AkAfRm">TM1638 led&key</a>
- <a href="https://a.aliexpress.com/_9iWCY8">емкостный датчик влажности почвы</a> - v1.2
- <a href="https://a.aliexpress.com/_9ha2b6">цифровой датчик температуры и влажности</a> - DHT22 (AM2302)
- <a href="https://a.aliexpress.com/_A3H75u">помпа</a> - 5В (2-3 л/мин)
- <a href="https://a.aliexpress.com/_A0fLBM">плата реле</a> - in 5V out DC30V/AC220V, механические реле 

#### библиотеки

- <a href="https://github.com/adafruit/Adafruit_Sensor">Adafruit Unified Sensor Lib</a>
- <a href="https://github.com/rjbatista/tm1638-library">TM1638</a>
- <a href="https://github.com/adafruit/DHT-sensor-library">DHT-sensor</a>