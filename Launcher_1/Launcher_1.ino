#include <QuadDisplay.h>
#include <Servo.h>

#include <RCSwitch.h>

#define BUTTON 8
#define BUZZER A0
#define LED 9
#define DISPLAY_PIN 7
#define START_CODE B10010101
#define LAUNCH_CODE B11011011
#define CHECK_CODE B10110001

const String START_COMMAND = "START", LAUNCH_COMMAND = "LAUNCH";

RCSwitch transmiter = RCSwitch();

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("START>>>\n");
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, INPUT);
  pinMode(LED, OUTPUT);
  transmiter.enableTransmit(2);
  Serial.println("Transmiter is ready...");
  displayDigits(DISPLAY_PIN, QD_S, QD_Y, QD_S, QD_DOT);
}

bool start_state = false;
void loop() {
  String msg = checkMsg();
  //if(!msg.equals(""))
    //Serial.println("CHECK (" + msg + ") w len(" + (String)msg.length() + ")\t"+ (String)msg.equals("START") + "\t:\t" + (String)msg.equals(START_COMMAND));
  if(msg.equals(START_COMMAND)){
    start_state == true;
    displayDigits(DISPLAY_PIN, QD_S, QD_Y, QD_S, QD_DOT);
    Serial.println("!SYSTEM STARTS TRASMITING!");
  }
  else if(msg.equals(LAUNCH_COMMAND)){
    msg = "";
    Serial.println("Enter time in seconds");
    while(msg.equals("")){
      msg = checkMsg();
    }
    unsigned long launchTime = (unsigned long)msg.toInt() * 1000;
    launchTrasmiting(launchTime);
    start_state = false;
  }
  
  if(!digitalRead(BUTTON)){
    unsigned long timer = millis(), d, launchTime;
    d = 1000;
    Serial.println("BUTTON ON");
    digitalWrite(LED, HIGH);
    while(!digitalRead(BUTTON)){
      launchTime = (millis() - timer) * 5;// - (millis() - timer) % d * 10;
      Serial.println(launchTime);
      displayInt(DISPLAY_PIN, launchTime / 100);
    }
    Serial.println("BUTTON OFF");
    digitalWrite(LED, LOW);
    launchTrasmiting(launchTime);
  }
}

void launchTrasmiting(unsigned long launchTime){
  //Отправка LAUNCH_CODE?
  Serial.println("!SYSTEM STARTS LAUNCHING!\nTIME BEFORE LAUNCHING: " + (String)(launchTime / 1000));
  delay(100);
  transmiter.send(START_CODE, 8);
  unsigned long timer = millis(), secLeft = 0, bzTimer = millis(), bzDelta = 0;
  bool bzS = true;
  while(millis() - timer < launchTime){
    if(!digitalRead(BUTTON)){
      noTone(BUZZER);
      digitalWrite(LED, LOW);
      displayDigits(DISPLAY_PIN, QD_O, QD_u, QD_t, QD_NONE);
      unsigned long t = millis();
      while(!digitalRead(BUTTON) || millis() - t < 3000){}
      displayDigits(DISPLAY_PIN, QD_S, QD_Y, QD_S, QD_DOT);
      return;
    }
    int sec = (launchTime - (millis() - timer)) / 1000;
    transmiter.send(sec, 8);
    if(sec > 0){
      displayInt(DISPLAY_PIN, sec * 10 + 6);
      if(millis() - bzTimer >= bzDelta){
        if(bzS){
          tone(BUZZER, 1000 / (launchTime / (millis() - timer)));
          //Serial.println("D: " + (String)(1000 / ((double)launchTime / (double)(millis() - timer))));
          digitalWrite(LED, HIGH);
        }else{
          noTone(BUZZER);
          digitalWrite(LED, LOW);
        }
        bzTimer = millis();
        bzDelta = (launchTime - (millis() - timer)) / 50;
        bzS = !bzS;
      }
    }
    else{
      displayDigits(DISPLAY_PIN, QD_NONE, QD_NONE, QD_0, QD_0);
      tone(BUZZER, (launchTime - (millis() - timer)) + 200);
    }
  }
  transmiter.send(LAUNCH_CODE, 8);
  Serial.println("!!!_LAUNCH_!!!");
  noTone(BUZZER);
  displayDigits(DISPLAY_PIN, QD_A, QD_I, QD_r, QD_NONE);
}

String checkMsg(){
  String msg = "";
  if(Serial.available() != 0){ 
    //Serial.flush();
    delay(50);
  }
  while(Serial.available() > 0 && Serial.peek() != ' '){
    msg += (char)Serial.read();
  }
  Serial.read();
  if(!msg.equals(""))Serial.println("enter: " + msg);
  return msg.substring(0, msg.length() - 1);
}
