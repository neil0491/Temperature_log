//************libraries**************//
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>

#define TEMP1 A2 //Подключение к пину датчика DS18B20
#define TEMP2 A1 //Подключение к пину датчика DS18B20

//************************************//
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);  //Подключение дисплея
RTC_DS1307 RTC;
OneWire oneWire1(TEMP1);
OneWire oneWire2(TEMP2);
DallasTemperature sensors1(&oneWire1);
DallasTemperature sensors2(&oneWire2);
File dataLog;
//************Variables**************//
int P1 = A0; // Button SET'
int newTime = 0;
int prevTime = 0;
bool key = 1;
boolean sd_ok = 0;
const int input_charge = 9;  // контакты, за которыми будем наблюдать
int time_on, time_off; // Период времени высокого и низкого пина контакте

// переменная хранения времени (unsigned long)
uint32_t myTimer1;

//************VariablesTime**************//
int hourupg;
int minupg;
int yearupg;
int monthupg;
int dayupg;
int menu = 0;

void setup()
{
  lcd.begin(20, 4);
  lcd.clear();

  sensors1.begin();
  sensors2.begin();

  pinMode(P1, INPUT);

  Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // Set the date and time at compile time
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  RTC.adjust(DateTime(__DATE__, __TIME__)); //removing "//" to adjust the time
  if ( !SD.begin() )
    Serial.println("initialization failed!");  // initialization error
  else {   // initialization OK
    sd_ok = 1;
    Serial.println("initialization done.");
    if ( SD.exists("Log.txt") == 0 )  // test if file with name 'Log.txt' already exists
    { // create a text file named 'Log.txt'
      Serial.print("\r\nCreate 'Log.txt' file ... ");
      dataLog = SD.open("Log.txt", FILE_WRITE);   // create (&open) file Log.txt
      if (dataLog) {                              // if the file opened okay, write to it:
        Serial.println("OK");
        // write some texts to 'Log.txt' file
        dataLog.println("    DATE    |  TIME  |    TEMPERATURE     |  STATUS");
        dataLog.close();   // close the file
      }
      else
        Serial.println("error creating file.");
    }
  }
  // The default display shows the date and time
  int menu = 0;
}

void loop()
{
  DateTime t = RTC.now();
  // check if you press the SET button and increase the menu index
  if (analogRead(P1) > 500 && analogRead(P1) < 1000)
  {
    menu = menu + 1;
  }
  // in which subroutine should we go?
  if (menu == 0)
  {
    DisplayDateTime(t); // void DisplayDateTime
    if (millis() - myTimer1 >= 500) {   // ищем разницу (500 мс)
      myTimer1 = millis();              // сброс таймера
      if ( digitalRead(input_charge) && key == 1) {
        prevTime = t.unixtime();
        key = 0;
        time_off = prevTime - newTime;
        //      Serial.println("PIN HIGHT");
        //      Serial.print("Prevent time - ");
        //      Serial.println(prevTime);
        printLog(sensors1, sensors2, "ON", t);
        lcd.clear();
      }

      if (  !digitalRead(input_charge) && key == 0) {
        //Serial.println("PIN LOW");
        //Serial.println(status);
        newTime = t.unixtime();
        //      Serial.print("New time - ");
        //      Serial.println(newTime);
        time_on = newTime - prevTime;
        //Serial.print("Разница - ");
        //Serial.println(period);
        key = 1;
        printLog(sensors1, sensors2, "OFF", t);
        whriteHourToSD(time_on, "Время работы - ");
        whriteHourToSD(time_off, "Время отдыха - ");
        lcd.clear();
      }
      lcd.setCursor(0, 1);
      lcd.print("Rabota: ");
      printHour(time_on);
      lcd.setCursor(0, 2);
      lcd.print("Otdih: ");
      // Serial.println(time_off);
      if (time_off <= 0 ) {
        lcd.print("No data");
      } else {
        printHour(time_off);
      }
    }
    print_temp_lcd(0, 0, "T1", sensors1);
    print_temp_lcd(10, 0, "T2", sensors2);

  }
  if (menu == 1)
  {
    DisplaySetHour();
  }
  if (menu == 2)
  {
    DisplaySetMinute();
  }
  if (menu == 3)
  {
    DisplaySetYear();
  }
  if (menu == 4)
  {
    DisplaySetMonth();
  }
  if (menu == 5)
  {
    DisplaySetDay();
  }
  if (menu == 6)
  {
    StoreAgg();
    delay(500);
    menu = 0;
  }
  delay(100);
}

void DisplayDateTime (DateTime now)
{
  lcd.setCursor(0, 3);
  if (now.hour() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.hour(), DEC);
  hourupg = now.hour();
  lcd.print(":");
  if (now.minute() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC);
  minupg = now.minute();
  lcd.print(":");
  if (now.second() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.second(), DEC);

  lcd.setCursor(10, 3);
  if (now.day() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.day(), DEC);
  dayupg = now.day();
  lcd.print("/");
  if (now.month() <= 9)
  {
    lcd.print("0");
  }
  lcd.print(now.month(), DEC);
  monthupg = now.month();
  lcd.print("/");
  lcd.print(now.year(), DEC);
  yearupg = now.year();
}

void DisplaySetHour()
{
  // time setting
  lcd.clear();
  DateTime now = RTC.now();
  if (analogRead(P1) > 1000)
  {
    if (hourupg == 23)
    {
      hourupg = 0;
    }
    else
    {
      hourupg = hourupg + 1;
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("Set time:");
  lcd.setCursor(0, 1);
  lcd.print(hourupg, DEC);
  delay(100);
}

void DisplaySetMinute()
{
  // Setting the minutes
  lcd.clear();
  if (analogRead(P1) > 1000)
  {
    if (minupg == 59)
    {
      minupg = 0;
    }
    else
    {
      minupg = minupg + 1;
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("Set Minutes:");
  lcd.setCursor(0, 1);
  lcd.print(minupg, DEC);
  delay(200);
}

void DisplaySetYear()
{
  // setting the year
  lcd.clear();
  if (analogRead(P1) > 1000)
  {
    yearupg = yearupg + 1;
  }

  lcd.setCursor(0, 0);
  lcd.print("Set Year:");
  lcd.setCursor(0, 1);
  lcd.print(yearupg, DEC);
  delay(200);
}

void DisplaySetMonth()
{
  // Setting the month
  lcd.clear();
  if (analogRead(P1) > 1000)
  {
    if (monthupg == 12)
    {
      monthupg = 1;
    }
    else
    {
      monthupg = monthupg + 1;
    }
  }
  lcd.setCursor(0, 0);
  lcd.print("Set Month:");
  lcd.setCursor(0, 1);
  lcd.print(monthupg, DEC);
  delay(200);
}

void DisplaySetDay()
{
  // Setting the day
  lcd.clear();
  if (analogRead(P1) > 1000)
  {
    if (dayupg == 31)
    {
      dayupg = 1;
    }
    else
    {
      dayupg = dayupg + 1;
    }
  }
  lcd.setCursor(0, 0);
  lcd.print("Set Day:");
  lcd.setCursor(0, 1);
  lcd.print(dayupg, DEC);
  delay(200);
}

void StoreAgg()
{
  // Variable saving
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SAVING IN");
  lcd.setCursor(0, 1);
  lcd.print("PROGRESS");
  RTC.adjust(DateTime(yearupg, monthupg, dayupg, hourupg, minupg, 0));
  delay(200);
}
