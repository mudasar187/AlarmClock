#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "pitches.h" // Sound library

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

#define TFT_CS 10 // Chip select line for TFT display
#define TFT_DC 9  // Data/command line for TFT
#define TFT_RST 8 // Reset line for TFT (or connect to +5V)


DateTime now; // Get the actual time/date from TFT
char* dayName; // Day names
int dayNumber; // Day number
int monthNumber; // Month number
int hourNumber; // Hour number
int minuteNumber; // Minute number
int secondNumber; // Second number
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // Days

const int buttonNearDisplay = 5; // the button from right
const int buttonMid = 6; // the button between left and right
const int buttonNearRTC = 7; // the button from left

int buttonStateNearRTC = 0; // state for button near rtc
int buttonStateMid = 0; // state for button in middle
int buttonStateNearDisplay = 0; // state for button near display

bool alarmMode = false;
bool alarmOn = false;
bool alarmOff = false;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
RTC_DS1307 rtc;

void setup()
{

  initPinModes();

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // Initialize TFT and fixed text's
  initTFT();
  initFixedText();

}

void loop()
{

  // Get updates from TFT
  getUpdatesFromTFT();

  // Update date
  updateDate();

  // Update time
  updateTime();

  // Set alarm
  setAlarm();

  Serial.begin(9600);


  buttonStateNearRTC = digitalRead(buttonNearRTC);
  buttonStateMid = digitalRead(buttonMid);
  buttonStateNearDisplay = digitalRead(buttonNearDisplay);

  if (buttonStateNearRTC == HIGH) {
    Serial.println(buttonStateNearRTC);
  }
  if (buttonStateNearDisplay == HIGH) {
    Serial.println(buttonStateNearDisplay);

  }
  if (buttonStateMid == HIGH) {
    Serial.println(buttonStateMid);
  }


}

void setAlarm() {
  if (buttonStateNearRTC == HIGH) {
    alarmMode = true;
  } else {
    draw_text(45, 135, "OFF", 2, ST7735_RED);
  }

  int minuteAlarm = 0;

  if(alarmMode == true) {
    draw_text(45, 135, "   ", 2, ST7735_RED);
    if(buttonStateMid = HIGH) {
        draw_number(55, 135, minuteAlarm , 2, ST7735_CYAN);
      }
  }

}

void setAlarmTime() {
  // Actual time
  if (hourNumber < 10) {
    updateToDecimal(15, 135, hourNumber , 2, ST7735_CYAN);
  } else {
    draw_number(15, 135, hourNumber , 2, ST7735_CYAN);
  }

  // Actual minute
  if (minuteNumber < 10) {
    updateToDecimal(55, 135, minuteNumber , 2, ST7735_CYAN);
  } else {
    draw_number(55, 135, minuteNumber , 2, ST7735_CYAN);
  }

  // Actual second
  if (secondNumber < 10) {
    updateToDecimal(90, 135, secondNumber , 2, ST7735_CYAN);
  } else {
    draw_number(90, 135, secondNumber , 2, ST7735_CYAN);
  }
}

void initPinModes() {
  pinMode(buttonNearRTC, INPUT);
  pinMode(buttonMid, INPUT);
  pinMode(buttonNearDisplay, INPUT);
}

void initTFT() {
  tft.initR(INITR_BLACKTAB); // initialize a ST7735R chip, red tab
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(2);
  tft.drawFastHLine(0, 53, tft.width(), ST7735_BLUE);
  tft.drawFastHLine(0, 106, tft.width(), ST7735_BLUE);
}

void initFixedText() {
  // Space with ":" between the time -> 20:13:56
  draw_text(40, 80, ":", 2, ST7735_CYAN);
  draw_text(80, 80, ":", 2, ST7735_CYAN);

  // Time text
  draw_text(52, 64, "TIME", 1, ST7735_MAGENTA);

  // Alarm text
  draw_text(45, 117, "ALARM", 1, ST7735_MAGENTA);
}

void getUpdatesFromTFT() {
  now = rtc.now(); // Get updates from TFT

  // Get each update like day name, day number, monthnumber, hour, minute, second
  dayName = daysOfTheWeek[now.dayOfTheWeek()];
  dayNumber = now.day();
  monthNumber = now.month();
  hourNumber = now.hour();
  minuteNumber = now.minute();
  secondNumber = now.second();
}

// Update the actual date
void updateDate() {
  // Day text
  draw_text(40, 5, dayName, 1, ST7735_MAGENTA);

  // Date number
  draw_number(30, 25, dayNumber , 2, ST7735_CYAN);
  draw_text(57, 25, "/", 2, ST7735_CYAN);
  draw_number(70, 25, monthNumber , 2, ST7735_CYAN);
}

// Updates the actual time
void updateTime() {
  // Actual time
  if (hourNumber < 10) {
    updateToDecimal(15, 80, hourNumber , 2, ST7735_CYAN);
  } else {
    draw_number(15, 80, hourNumber , 2, ST7735_CYAN);
  }

  // Actual minute
  if (minuteNumber < 10) {
    updateToDecimal(55, 80, minuteNumber , 2, ST7735_CYAN);
  } else {
    draw_number(55, 80, minuteNumber , 2, ST7735_CYAN);
  }

  // Actual second
  if (secondNumber < 10) {
    updateToDecimal(90, 80, secondNumber , 2, ST7735_CYAN);
  } else {
    draw_number(90, 80, secondNumber , 2, ST7735_CYAN);
  }
}

// Draw a text with X and Y position and the actual text with size and colour
void draw_text(byte x_pos, byte y_pos, const char *text, byte text_size, uint16_t text_color) {
  tft.setCursor(x_pos, y_pos);
  tft.setTextSize(text_size);
  tft.setTextColor(text_color, ST7735_BLACK);
  tft.print(text);
}

// Draw a number with X and Y position and the actual number with size and colour
void draw_number(byte x_pos, byte y_pos, int number, byte text_size, uint16_t text_color) {
  tft.setCursor(x_pos, y_pos);
  tft.setTextSize(text_size);
  tft.setTextColor(text_color, ST7735_BLACK);
  tft.print(number);
}

// Draw a text with X and Y position and the actual text with size and colour with update to a decimal, otherwise you will get "1" instead of "01"
void updateToDecimal(byte x_pos, byte y_pos, int number, byte text_size, uint16_t text_color) {
  tft.setCursor(x_pos, y_pos);
  tft.setTextSize(text_size);
  tft.setTextColor(text_color, ST7735_BLACK);
  tft.print("0"); tft.print(number);
}
