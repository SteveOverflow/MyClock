#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_I2C_ADDRESS 0x3C
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define DS3231_I2C_ADDRESS 0x68

unsigned long lastToggle;
boolean showColon = true;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
  lastToggle = millis();
}

void loop() {
  display.display();
  display.clearDisplay();
  //delay(2000);
  boolean showColon = true;
  lastToggle = millis();
  
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(formatDayOfWeek(dayOfWeek));
  display.print(" ");
  display.print(formatMonth(month));
  display.print(" ");
  display.print(formatNumber(dayOfMonth));
  display.setTextSize(1);
  display.println();
  display.setTextSize(2);
  display.print(" ");
  display.print(formatNumber(hour));
  display.print(":");
  display.print(formatNumber(minute));
  display.print(".");
  display.print(formatNumber(second));
 
  display.display();
}

byte bcdToDec(byte val){
  return( (val/16*10) + (val%16) ); 
}

void readTime(byte *second,
              byte *minute,
              byte *hour,
              byte *dayOfWeek,
              byte *dayOfMonth,
              byte *month,
              byte *year){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);

  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());  
}

String formatDayOfWeek(byte dayOfWeek){
  String day;
  switch(dayOfWeek){
  case 1:
    day="Sun";
    break;
  case 2:
    day="Mon";
    break;
  case 3:
    day="Tue";
    break;
  case 4:
    day="Wed";
    break;
  case 5:
    day="Thu";
    break;
  case 6:
    day="Fri";
    break;
  case 7:
    day="Sat";
    break;
  }
  return(day);
}

String formatMonth(byte m){
  String month;
  
  switch(m){
    case 1:
      month = "Jan";
      break;
    case 2:
      month = "Feb";
      break;
    case 3:
      month = "Mar";
      break;
    case 4:
      month = "Apr";
      break;
    case 5:
      month = "May";
      break;
    case 6:
      month = "Jun";
      break;
    case 7:
      month = "Jul";
      break;
    case 8:
      month = "Aug";
      break;
    case 9:
      month = "Sep";
      break;
    case 10:
      month = "Oct";
      break;
    case 11:
      month = "Nov";
      break;
    case 12:
      month = "Dec";
      break; 
  }
  
  return(month);
}

String formatNumber(byte n){
  if(n<10){
    return("0" +String(n));
  }else{
    return(String(n));  
  }
}
