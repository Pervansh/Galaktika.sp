#include <QuadDisplay.h>
#include <Servo.h>
#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля

#define BUTTON 6
#define BUZZER A3
#define LED 5
#define DISPLAY_PIN 8
#define START_CODE B10010101
#define LAUNCH_CODE B11011011
#define CHECK_CODE B10110001

template <typename T>
void transmite(T* ptr);

byte startCode = B10010101;
byte launchCode = B11011011;
byte checkCode = B10110001;

unsigned long transmiteTimer = 0, transmiteDelta = 50;

const String START_COMMAND = "START", LAUNCH_COMMAND = "LAUNCH";

RF24 radio(10, 9); // "создать" модуль на пинах 10 и 9 Для Уно

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("START>>>\n");
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, INPUT);
  pinMode(LED, OUTPUT);
  displayDigits(DISPLAY_PIN, QD_S, QD_Y, QD_S, QD_DOT);

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x6a);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик
  Serial.println("Transmiter is ready...");
}

bool start_state = false;
void loop() {
  String msg = checkMsg();
  //if(!msg.equals(""))
  //Serial.println("CHECK (" + msg + ") w len(" + (String)msg.length() + ")\t"+ (String)msg.equals("START") + "\t:\t" + (String)msg.equals(START_COMMAND));
  if (msg.equals(START_COMMAND)) {
    start_state == true;
    displayDigits(DISPLAY_PIN, QD_S, QD_Y, QD_S, QD_DOT);
    Serial.println("!SYSTEM STARTS TRASMITING!");
  }
  else if (msg.equals(LAUNCH_COMMAND)) {
    msg = "";
    Serial.println("Enter time in seconds");
    while (msg.equals("")) {
      msg = checkMsg();
    }
    unsigned long launchTime = (unsigned long)msg.toInt() * 1000;
    launchTrasmiting(launchTime);
    start_state = false;
  }

  if (!digitalRead(BUTTON)) {
    unsigned long timer = millis(), d, launchTime;
    d = 1000;
    Serial.println("BUTTON ON");
    digitalWrite(LED, HIGH);
    while (!digitalRead(BUTTON)) {
      launchTime = (millis() - timer) * 5;// - (millis() - timer) % d * 10;
      Serial.println(launchTime);
      displayInt(DISPLAY_PIN, launchTime / 100);
    }
    Serial.println("BUTTON OFF");
    digitalWrite(LED, LOW);
    launchTrasmiting(launchTime);
  }
}

void launchTrasmiting(unsigned long launchTime) {
  //Отправка LAUNCH_CODE?
  Serial.println("!SYSTEM STARTS LAUNCHING!\nTIME BEFORE LAUNCHING: " + (String)(launchTime / 1000));
  delay(100);
  transmite(&startCode);
  unsigned long timer = millis(), secLeft = 0, bzTimer = millis(), bzDelta = 0;
  bool bzS = true;
  while (millis() - timer < launchTime) {
    if (!digitalRead(BUTTON)) {
      noTone(BUZZER);
      digitalWrite(LED, LOW);
      displayDigits(DISPLAY_PIN, QD_O, QD_u, QD_t, QD_NONE);
      unsigned long t = millis();
      while (!digitalRead(BUTTON) || millis() - t < 3000) {}
      displayDigits(DISPLAY_PIN, QD_S, QD_Y, QD_S, QD_DOT);
      return;
    }
    byte sec = (byte)((launchTime - (millis() - timer)) / 1000);
    transmite(&sec);
    if (sec > 0) {
      displayInt(DISPLAY_PIN, sec * 10 + 6);
      if (millis() - bzTimer >= bzDelta) {
        if (bzS) {
          tone(BUZZER, 1000 / (launchTime / (millis() - timer)));
          //Serial.println("D: " + (String)(1000 / ((double)launchTime / (double)(millis() - timer))));
          digitalWrite(LED, HIGH);
        } else {
          noTone(BUZZER);
          digitalWrite(LED, LOW);
        }
        bzTimer = millis();
        bzDelta = (launchTime - (millis() - timer)) / 50;
        bzS = !bzS;
      }
    }
    else {
      displayDigits(DISPLAY_PIN, QD_NONE, QD_NONE, QD_0, QD_0);
      tone(BUZZER, (launchTime - (millis() - timer)) + 200);
    }
  }
  transmite(&launchCode);
  Serial.println("!!!_LAUNCH_!!!");
  noTone(BUZZER);
  displayDigits(DISPLAY_PIN, QD_A, QD_I, QD_r, QD_NONE);
}

template <typename T>
void transmite(T* ptr) {
  if (millis() - transmiteTimer >= transmiteDelta) {
    transmiteTimer = millis();
    radio.write(ptr, sizeof(*ptr));
  }
}

String checkMsg() {
  String msg = "";
  if (Serial.available() != 0) {
    //Serial.flush();
    delay(50);
  }
  while (Serial.available() > 0 && Serial.peek() != ' ') {
    msg += (char)Serial.read();
  }
  Serial.read();
  if (!msg.equals(""))Serial.println("enter: " + msg);
  return msg.substring(0, msg.length() - 1);
}
