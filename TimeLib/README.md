Библиотека для работы со временем в Arduino ESP8266

Как использовать:

#include <TimeLib.h>
 
TimeLib tt;
 
Serial.print ( tt.tmDate() );
Serial.print ( tt.tmTime() );

!!При этом в скетче должно быть установлено соединение с Интернет.
