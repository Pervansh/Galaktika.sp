#include "BlipSystem.h"
#include "Arduino.h"

void BlipSystem::init() {
  bool wasError = false;
  Serial.begin(9600);
  delay(50);
  Serial.println("START...");

  pinMode(BUZZER, OUTPUT);
  pinMode(PARACHUTE_PIN_1, OUTPUT);
  digitalWrite(PARACHUTE_PIN_1, LOW);
  pinMode(PARACHUTE_PIN_2, OUTPUT);
  digitalWrite(PARACHUTE_PIN_2, LOW);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(SAFE_SWITCH, INPUT_PULLUP);

  bool cls_start[3] = {true, true, true};
  setIndication(0, 0, cls_start, 1000, 0);
  tone(BUZZER, P_TONE);
  delay(250);
  noTone(BUZZER);
  delay(250);
  tone(BUZZER, P_TONE);
  delay(250);
  noTone(BUZZER);
  delay(250);
  tone(BUZZER, P_TONE);
  delay(250);
  noTone(BUZZER);
  delay(250);
  
  pServoX->attach(14);
  pServoZ->attach(8);

  for (int pos = 70; pos <= 110; pos += 1) { // goes from 0 degrees to 180 degrees in steps of 1 degree
    pServoX->write(pos);
    pServoZ->write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (int pos = 110; pos >= 70; pos -= 1) { // goes from 180 degrees to 0 degrees
    pServoX->write(pos);
    pServoZ->write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }

  pServoX->write(86);
  pServoZ->write(96);

  //  Serial.println("Flash init: " + flash.initialize() ? "OK!" : "FAIL!");
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    //while (1);
    wasError = true;
  }
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  
  Serial.println("\nUpdating position...");
  updatePosition();
  //setIndication(b_im, b_tn);
  
  if (!pLogger->start()){
    wasError = true;
  }
  
  Serial.println("Setup is over");
  if (wasError) {
    Serial.println("was ERROR!");
    bool cls_err[3] = {true, false, false};
    setIndication(N_TONE, 2000, cls_err, 2000);
    state = FINAL_STATE;
    return;
  }
  loopTimer = millis();
  bool cls[3] = {true, true, true};
  setIndication(P_TONE, 1000, cls, 1000, 3);
}

void BlipSystem::execute() {
  updatePosition();
  switch (state) {
    case SETUP:
      pState->execute();
      break;
    case WAITING_START:
      waitingStart();
      break;
    case MIDDLE_AIR:
      //myservoX.write((axis_x_max - axis_x_min) / 2 - roll);
      //myservoY.write((axis_y_max - axis_y_min) / 2 - pitch);
      thrusterStabilization((double)(millis() - loopTimer) / 1000);
      break;
    case LANDING:
      parachuteLanding();
      pState->execute();
      break;
    case SAVING_STATE:
      state = FINAL_STATE;
      bool cls[3] = {true, true, true};
      setIndication(P_TONE, 2000, cls, 2000, 1);
      break;
    case FINAL_STATE:
      break;
  }
  Serial.println("ACCEL:");
  Serial.println(" aX : " + (String)accX);
  Serial.println(" aY : " + (String)accY);
  Serial.println(" aZ : " + (String)accZ);
  Serial.println("GYRO:");
  Serial.println(" gX : " + (String)gyroX);
  Serial.println(" gY : " + (String)gyroY);
  Serial.println(" gZ : " + (String)gyroZ);
  Serial.println("M: " + (String)height);
  Serial.println("STATE: " + (String)state);
  //checkState();
  indicate();
  pLogger->logInfo();
  loopTimer = millis();
  delay(20);
}

void BlipSystem::checkState(){
  double a = sqrt(accY * accY + accZ * accZ + accX * accX);
  if ((gyroX >= 60 || gyroX <= -60 || gyroZ >= 60 || gyroZ <= -60) && state == MIDDLE_AIR) {
    event_code = EVENT_MISSION_ABORT;
    event_meter += 2;
    Serial.println("ENTER: " + (String)event_meter);
  } else if (height >= goal_height) {
    event_code = EVENT_GOAL_HAD_REACHED;
    event_meter += 100;
  } else if (a <= 5000) {
    event_code = EVENT_GOAL_HAD_REACHED;
    event_meter += 10;
  }
  else {
    event_meter = 0;
    event_code = 0;
  }
  if (event_meter >= 100) {
    Serial.println("CODE: " + (String)event_code);
    if (EVENT_GOAL_HAD_REACHED == event_code) {
      state = LANDING;
      delete pState;
      pState = new LandingState(this);
      bool cls[3] = {true, false, false};
      setIndication(N_TONE, 2000, cls, 2000, 1);
      pLogger->logEvent("GOAL HAD REACHED(empty fueltank)");
      pImuUnit->useAccelCorrection(true);
    } else if (EVENT_MISSION_ABORT == event_code) {
      state = LANDING;
      delete pState;
      pState = new LandingState(this);
      Serial.println("ENTRED!");
      bool cls_2[3] = {true, false, false};
      setIndication(N_TONE, 0, cls_2, 0);
      pLogger->logEvent("MISSION ABORT");
      pImuUnit->useAccelCorrection(true); 
    }
    event_meter = 0;
  }
}

void BlipSystem::setIndication(int buzTone, unsigned long buzDelta = 0, int buzCount = -1) {
  buzzerTone = buzTone;
  buzzerDelta = buzDelta;
  buzzerCount = buzCount * 2 - 1;
  buzzerTimer = millis();
  buzzerState = true;
  tone(BUZZER, buzzerTone);
}

void BlipSystem::setIndication(bool rgbCls[3], unsigned long rgbDelta = 0, int rgbCount = -1) {
  ledColors[0] = rgbCls[0];
  ledColors[1] = rgbCls[1];
  ledColors[2] = rgbCls[2];
  ledDelta = rgbDelta;
  ledCount = rgbCount * 2 - 1;
  ledTimer = millis();
  ledState = true;
  digitalWrite(LED_R, !ledColors[0]);
  digitalWrite(LED_G, !ledColors[1]);
  digitalWrite(LED_B, !ledColors[2]);
}

void BlipSystem::setIndication(int buzTone, unsigned long buzDelta, bool rgbCls[3], unsigned long rgbDelta, int buzCount = -1, int rgbCount = -1) {
  setIndication(buzTone, buzDelta, buzCount);
  setIndication(rgbCls, rgbDelta, rgbCount);
}

void BlipSystem::indicate() {
  if (millis() - buzzerTimer >= buzzerDelta && buzzerCount != 0 && buzzerDelta != 0) {
    if (!buzzerState)
      tone(BUZZER, buzzerTone);
    else
      noTone(BUZZER);
    buzzerTimer = millis();
    buzzerState = !buzzerState;
    if (buzzerCount > 0)
      buzzerCount--;
  }
  if (millis() - ledTimer >= ledDelta && ledCount != 0 && ledDelta != 0) {
    digitalWrite(LED_R, ledColors[0] && ledState);
    digitalWrite(LED_G, ledColors[1] && ledState);
    digitalWrite(LED_B, ledColors[2] && ledState);
    ledTimer = millis();
    ledState = !ledState;
    if (ledCount > 0)
      ledCount--;
  }
}

void BlipSystem::parachuteLanding() {
  landingTimer = landingTimer == 0 ? millis() : landingTimer;
  /*
    if (millis() - landingTimer >= 2000) {
    digitalWrite(PARACHUTE_PIN_1, LOW);
    digitalWrite(PARACHUTE_PIN_2, LOW);
    }
    else {
    digitalWrite(PARACHUTE_PIN_1, HIGH);
    digitalWrite(PARACHUTE_PIN_2, HIGH);
    }
    double a = sqrt(accY * accY + accZ * accZ + accX * accX);
  */
  /*
    if (a >= 18000) {
    state = SAVING_STATE;
    bool cls[3] = {false, true, false};
    setIndication(P_TONE, 500, cls, 500);
    }*/
}

void BlipSystem::thrusterStabilization(double dt) {
  double x_u, z_u, x_e, z_e, x_d, z_d, x_i, z_i, x_p, z_p;
  x_e = gyroX;
  z_e = -gyroZ;
  x_p = kP * x_e;
  z_p = kP * z_e;
  x_i = last_x_i + kI * x_e * dt / 1000;
  z_i = last_z_i + kI * z_e * dt / 1000;
  x_d = kD * (x_e - last_x_e) / dt * 1000;
  z_d = kD * (z_e - last_z_e) / dt * 1000;
  x_u = x_p + x_i + x_d;
  z_u = z_p + z_i + z_d;
  double angleX = constrain(axis_x_center + x_e, axis_x_min, axis_x_max);
  double angleZ = constrain(axis_z_center + z_e, axis_z_min, axis_z_max);
  pServoX->write(angleX);
  pServoZ->write(angleZ);
  Serial.println("servoX: " + (String)angleX + ", uX: " + (String)x_u);
  Serial.println("servoZ: " + (String)angleZ + ", uZ: " + (String)z_u);
  last_x_e = x_e;
  last_z_e = z_e;
  last_x_i = x_i;
  last_z_i = z_i;
}

void BlipSystem::waitingStart() {
  double a = sqrt(accY * accY + accZ * accZ + accX * accX);
  if (accY >= 25000) {
    state = MIDDLE_AIR;
    bool cls[3] = {false, false, true};
    setIndication(P_TONE, 1000, cls, 0);
    pLogger->logEvent("ROCKET IN AIR");
    pImuUnit->useAccelCorrection(false);
    //setIndication(b_im, b_tn);
  }
}

void BlipSystem::updatePosition() {
  pImuUnit->getMetrics6(&gyroX, &gyroY, &gyroZ, &accX, &accY, &accZ);
  height = bmp.readAltitude(seaAirPressure) - zeroHeight;
}
