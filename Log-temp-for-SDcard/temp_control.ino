
void print_temp_lcd(int col, int row, String temp, DallasTemperature sensors) {
  lcd.setCursor(col, row);
  lcd.print(temp + " ");
  sensors.requestTemperatures();
  lcd.print(sensors.getTempCByIndex(0));
  delay(100);
}

void whriteHourToSD(int seconds, String str) {
  int hour = seconds / 3600;
  int minute = seconds % 3600 / 60;
  int second = seconds % 3600 % 60;

  dataLog = SD.open("Log.txt", FILE_WRITE);
  dataLog.print(str);
  dataLog.print(hour);
  dataLog.print(":");
  dataLog.print(minute);
  dataLog.print(":");
  dataLog.println(second);
  dataLog.close();
}

void printHour(int seconds) {
  int hour = seconds / 3600;
  int minute = seconds % 3600 / 60;
  int second = seconds % 3600 % 60;
  lcd.print(String(hour) + ":" + String( minute ) + ":" + String( second ) );
}

void printLog(DallasTemperature temp1, DallasTemperature temp2, String switcher, DateTime t) {
  temp1.requestTemperatures();
  temp2.requestTemperatures();
  dataLog = SD.open("Log.txt", FILE_WRITE);
  dataLog.print('_');
  dataLog.print( t.day(), DEC );
  dataLog.print('/');
  dataLog.print( t.month(), DEC );
  dataLog.print('/');
  dataLog.print( t.year(), DEC );
  dataLog.print("__");
  dataLog.print('|');
  dataLog.print(t.hour(), DEC);
  dataLog.print(':');
  dataLog.print(t.minute(), DEC);
  dataLog.print(':');
  dataLog.print(t.second(), DEC);
  dataLog.print('|');
  dataLog.print("T1 - ");
  dataLog.print(sensors1.getTempCByIndex(0));
  dataLog.print(" T2 - ");
  dataLog.print(sensors2.getTempCByIndex(0));
  dataLog.print("|");
  dataLog.println(switcher);
  dataLog.close();   // close the file
}
