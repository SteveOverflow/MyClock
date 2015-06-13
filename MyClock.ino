#include <SoftwareSerial.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_I2C_ADDRESS 0x3C
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define DS3231_I2C_ADDRESS 0x68

const int btConnectPin = 12;

SoftwareSerial bluetooth(10, 11);

// bitmaps for display of notifications
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

//facebook notification icon
static const unsigned char PROGMEM fb_bmp[] ={ 
  B00001111, B11110000,
  B00111111, B11111100,
  B01111111, B11111110,
  B01111111, B11111110,
  B11111111, B11000011,
  B11111111, B10000011,
  B11111111, B10001111,
  B11111111, B10011111,
  B11111110, B00000011,
  B11111110, B00000111,
  B11111111, B10011111,
  B11111111, B10011111,
  B01111111, B10011110,
  B01111111, B10011110,
  B00111111, B10011100,
  B00001111, B10010000 };
  
//exclamation mark icon  
static const unsigned char PROGMEM exclamation_bmp[] = {
  B00001111, B11110000,
  B00111111, B11111100,
  B01111110, B01111110,
  B01111100, B00111110,
  B11111000, B00011111,
  B11111000, B00011111,
  B11111000, B00011111,
  B11111000, B00011111,
  B11111100, B00111111,
  B11111100, B00111111,
  B11111110, B01111111,
  B11111111, B11111111,
  B01111110, B01111110,
  B01111110, B01111110,
  B00111111, B11111100,
  B00001111, B11110000  
};

//bluetooth connection icon
static const unsigned char PROGMEM bluetooth_bmp[] ={
   B00000000, B00000000,
   B00000000, B00000000,
   B00000001, B11000000,
   B00000001, B11100000,
   B00000001, B10110000,
   B00011001, B10011000,
   B00001101, B10110000,
   B00000111, B11100000,
   B00000001, B10000000,
   B00000111, B11100000,
   B00001101, B10110000,
   B00011001, B10011000,
   B00000001, B10110000,
   B00000001, B11100000,
   B00000001, B11000000,
   B00000000, B00000000
};

//hangouts notificaiton icon
static const unsigned char PROGMEM hangouts_bmp[] = {
  B00000000, B00000000,//1
  B00000111, B11100000,//2
  B00001111, B11110000,//3
  B00011111, B11111000,//4
  B00111111, B11111100,//5
  B01111111, B11111110,//6
  B01110001, B10001110,//7
  B01110001, B10001110,//8
  B01110001, B10001110,//9
  B01111101, B11101110,//10
  B01111011, B11011110,//11
  B00111111, B11111100,//12
  B00011111, B11111000,//13
  B00001111, B11110000,//14
  B00000111, B11100000,//15
  B00000000, B00000000//16
};

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
  
  pinMode(btConnectPin, INPUT);
}

boolean showHangouts = false;
boolean showFacebook = false;
boolean showExclamation = false;

void loop() {
  display.display();
  display.clearDisplay();
  
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  displayDateTime(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
 
  showBluetooth();
  setIcons();
  showIcons();
 
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

void displayDateTime(byte second,
              byte minute,
              byte hour,
              byte dayOfWeek,
              byte dayOfMonth,
              byte month,
              byte year){
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
  display.print("  ");
  display.print(formatNumber(hour));
  display.print(":");
  display.print(formatNumber(minute));
  
  display.setTextSize(1);
  display.print(" ");
  display.print(formatNumber(second));                  
}

void showBluetooth(){
  if(digitalRead(btConnectPin)==HIGH){
    display.drawBitmap(0, 48, bluetooth_bmp, 16, 16, WHITE);
  }
}

void setIcons(){
  while(bluetooth.available()){
    char op = bluetooth.read();
    if(op=='+'||op=='-'){
       char app = bluetooth.read();
       setIconFlags(op, app);
    }
  }
}

void setIconFlags(char op, char app){
  if(app=='h'){
    if(op=='+'){
      showHangouts = true;
    }
    if(op=='-'){
      showHangouts = false; 
    }
  }
  
  if(app=='f'){
    if(op=='+'){
      showFacebook = true;
    }
    if(op=='-'){
      showFacebook = false;
    }
  }
  
  if(app=='o'){
    if(op=='+'){

      showExclamation = true;
    }
    if(op=='-'){
      showExclamation = false;
    }
  }
}

void showIcons(){
  if(showHangouts){
    display.drawBitmap(18, 48, hangouts_bmp, 16, 16, WHITE); 
  }
  
  if(showFacebook){
    display.drawBitmap(36, 48, fb_bmp, 16, 16, WHITE);
  }
  
  if(showExclamation){
    display.drawBitmap(54, 48, exclamation_bmp, 16, 16, WHITE); 
  }
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
