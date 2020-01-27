#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "Matrix.h"

//CODES
#define LAUNCH_CODE B11011011
#define START_CODE B10010101

//PINS
#define R 21
#define MATRIX 8
#define BUZZER A3
#define SWITCH 6

// Max is 255, 32 is a conservative value to not overload
// a USB power supply (500mA) for 12x12 pixels.
#define BRIGHTNESS 10
#define LED_BLACK    0

// Define matrix width and height.
#define mw 8
#define mh 8

bool launchStart = false;
unsigned long buzzerDelta = 0, buzzerTimer = 0;
int lastValue = 11;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoMatrix *matrix = new Adafruit_NeoMatrix(mw, mh, MATRIX,
    NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
    NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
    NEO_GRB            + NEO_KHZ800);


RF24 radio(10, 9); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

void setup() {
  bool wasError = false;

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.setChannel(0x6a);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль

  Serial.begin(9600);
  pinMode(R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);
  // Test full bright of all LEDs. If brightness is too high
  // for your current limit (i.e. USB), decrease it.
  //matrix->fillScreen(LED_WHITE_HIGH);
  //matrix->show();
  //delay(1000);
  matrix->clear();

  if (wasError) {
    Serial.println("SYSTEM IS NOT READY!");
    display_rgbBitmap(MATRIX_CROSS);
    while (true) {}
  }
  Serial.println("SYSTEM IS READY!");
  display_rgbBitmap(MATRIX_CHECK);
}

void loop() {
  byte pipeNo;
  if (radio.available(&pipeNo)) {    // слушаем эфир со всех труб
    int value;
    radio.read(&value, sizeof(value));         // чиатем входящий сигнал
    Serial.print("Recieved: "); Serial.println(value);

    Serial.println(value);

    if (!digitalRead(SWITCH)) {
      if (launchStart) {
        launchStart = false;
        tone(BUZZER, 720);
        delay(500);
        noTone(BUZZER);
      }
    }
    else {
      if (value == START_CODE && !launchStart) {
        Serial.println("START");
        //display_rgbBitmap(MATRIX_SIGNAL);
        matrix->clear();
        launchStart = true;
      }
      else if (value == LAUNCH_CODE && launchStart) {
        Serial.println("LAUNCH");
        digitalWrite(BUZZER, HIGH);
        digitalWrite(R, HIGH);
        for (int i = MATRIX_ROCKET; i <= MATRIX_ROCKET_END; i++) {
          display_rgbBitmap(i);
          delay(200);
        }
        digitalWrite(BUZZER, LOW);
        digitalWrite(R, LOW);
        launchStart = false;
        display_rgbBitmap(MATRIX_CHECK);
      }
      else if (launchStart) {
        indicate(value);
        if (value < 10 && lastValue != value) {
          digitalWrite(BUZZER, HIGH);
          delay(300);
          digitalWrite(BUZZER, LOW);
          lastValue = value;
        }
      }
      else {
        display_rgbBitmap(MATRIX_CHECK);
        launchStart = true;
      }
    }
  }
}

void indicate(int value) {
  if (value == 0) {}

  else if (value >= 10) {}

  else
    display_rgbBitmap(MATRIX_SIGNAL);
}

void display_rgbBitmap(uint8_t bmp_num) {
  static uint16_t bmx, bmy;

  fixdrawRGBBitmap(bmx, bmy, RGB_bmp[bmp_num], 8, 8);
  bmx += 8;
  if (bmx >= mw) bmx = 0;
  if (!bmx) bmy += 8;
  if (bmy >= mh) bmy = 0;
  matrix->show();
}

// Convert a BGR 4/4/4 bitmap to RGB 5/6/5 used by Adafruit_GFX
void fixdrawRGBBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
  uint16_t RGB_bmp_fixed[w * h];
  for (uint16_t pixel = 0; pixel < w * h; pixel++) {
    uint8_t r, g, b;
    uint16_t color = pgm_read_word(bitmap + pixel);

    //Serial.print(color, HEX);
    b = (color & 0xF00) >> 8;
    g = (color & 0x0F0) >> 4;
    r = color & 0x00F;
    //Serial.print(" ");
    //Serial.print(b);
    //Serial.print("/");
    //Serial.print(g);
    //Serial.print("/");
    //Serial.print(r);
    //Serial.print(" -> ");
    // expand from 4/4/4 bits per color to 5/6/5
    b = map(b, 0, 15, 0, 31);
    g = map(g, 0, 15, 0, 63);
    r = map(r, 0, 15, 0, 31);
    //Serial.print(r);
    //Serial.print("/");
    //Serial.print(g);
    //Serial.print("/");
    //Serial.print(b);
    RGB_bmp_fixed[pixel] = (r << 11) + (g << 5) + b;
    // Serial.print(" -> ");
    //Serial.print(pixel);
    //Serial.print(" -> ");
    //Serial.println(RGB_bmp_fixed[pixel], HEX);
  }
  matrix->drawRGBBitmap(x, y, RGB_bmp_fixed, w, h);
}
