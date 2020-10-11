#include "config.h"
#include "BlipState.h"
#include "BlipSystem.h"
#include "BlipSubscriber.h"
#include "BlipFunctorSubscriber.h"
#include "SetupState.h"
#include "WaitingStartState.h"
#include "ThrusterState.h"
#include "LandingState.h"
#include "SaveState.h"
#include "WaitingState.h"
#include "ApogeeDetectionState.h"
#include "TrieEventManager.h"
#include "BlipRangeSigner.h"
#include <IFunctional.h>

/*          ЗАМЕТКИ
   ( (-) - не исправлено, (+) - исправлено)
   -Отключена проверка состояния
   -Отключена проверка BMP280
   -Перед запуском проверь код
   -Парашют отключен!!!!!!!!!!!!!!!
   -Время калибровки уменьшено
   +Время между логами уменьшено
   +Отключена проверка работы SD Card Holder'а
   -Задержка в лупе!!!
   +Постоянная коректировка по акселерометру
*/

#define LAUNCH_SIGNAL_PIN 14

MpuOrintationUnit* pImuUnit = new MpuOrintationUnit(AXIS_ROTATION, true);
TrieEventManager* eventManager = new TrieEventManager();

BlipSystem *pBlipSystem = new BlipSystem(pImuUnit, eventManager);

SetupState* setupState = new SetupState(pBlipSystem, SAFE_SWITCH);
WaitingStartState* waitingStartState = new WaitingStartState(pBlipSystem, false);
ThrusterState* thrusterState = new ThrusterState(pBlipSystem);
LandingState* landingState = new LandingState(pBlipSystem);
SaveState* saveState = new SaveState(pBlipSystem);
ApogeeDetectionState* apogeeDetectionState = new ApogeeDetectionState(pBlipSystem);

uint64_t calculateActivationTime() {
  return calculateActivationTime(DROP_HEIGHT);
}

uint64_t calculateActivationTime(double h) {
  double m = ROCKET_MASS;
  double t = LANDING_ENGINE_BURNING_TIME;
  double F = AVERAGE_LANDING_ENGINE_FORCE;
  double g = G_CONSTANT;
  double V = CRITICAL_LANDING_VELOCITY;

  double timeInSeconds = (m * h) / (F * t) - (m * sq(V)) / (2 * F * g * t) + (F * t) / (2 * m * g) - t / 2 - ENGINE_WARMING_TIME;

  return (uint64_t)(timeInSeconds * 1000.0);
}

void waitFallInit(BlipSystem* system) {
  system->getImuUnitPointer()->useAccelCorrection(false);
  bool cls[3] = {true, true, false};
  system->setIndication(175, 1500, cls, 500);
}

bool isFreeFall(BlipSystem* system) {
    uint32_t a = sqrt(sq(system->getAccX()) + sq(system->getAccY()) + sq(system->getAccZ()));
    return a <= FREE_FALL_MAX_ACC;
}

uint64_t activationTimer = 0;

void freeFallInit(BlipSystem* system) {
  auto h = system->getHeight();
  uint64_t calculation = calculateActivationTime(h);
  activationTimer = calculation + millis();
  system->getSystemLogger()->logEvent("Landing Engine Time: " + (String)long(calculation) + ", height: " + (String)h);
  bool cls[3] = {true, false, false};
  system->setIndication(350, 100, cls, 100);
}

bool activationTimeInit(BlipSystem* system) {
  freeFallInit(system);
  return true;
}

bool isActivationTime(BlipSystem* system) {
  return millis() >= activationTimer;
}

void waitLaunchInit(BlipSystem* system) {
  pinMode(LAUNCH_SIGNAL_PIN, INPUT);
}

bool isLaunch(BlipSystem* system) {
  Serial.println("WAITING LAUNCH");
  return digitalRead(LAUNCH_SIGNAL_PIN);
}

bool resetOffsets(BlipSystem* system) {
  system->getImuUnitPointer()->resetAngles();
  Serial.println("--------------------------------------");
  return true;
}

bool startDataLog(BlipSystem* system) {
  system->getSystemLogger()->start();
  return true;
}

bool stopDataLog(BlipSystem* system) {
  system->getSystemLogger()->finish();
  return true;
}

bool nextStateFlag = false;
void setup() {
  Serial.begin(9600);
  
  Serial.println(">> preInit");
  pBlipSystem->init();
  Serial.println(">> postInit (ENGINE_LANDING_TEST_W/_MPU)");
  Serial.println((int)calculateActivationTime());
  pinMode(LAUNCH_SIGNAL_PIN, INPUT);

  WaitingState* waitingFallState = new WaitingState(pBlipSystem, blip::toFunctional(&isFreeFall), blip::toFunctional(&waitFallInit));
  //WaitingState* waitingActivationState = new WaitingState(pBlipSystem, blip::toFunctional(&isActivationTime), blip::toFunctional(&freeFallInit));
  WaitingState* isActivationTimeState = new WaitingState(pBlipSystem, blip::toFunctional(&isActivationTime));
  WaitingState* activationTimeInitState = new WaitingState(pBlipSystem, blip::toFunctional(&activationTimeInit));
  WaitingState* resetOffsetState = new WaitingState(pBlipSystem, blip::toFunctional(&resetOffsets));
  WaitingState* startDataLogState = new WaitingState(pBlipSystem, blip::toFunctional(&startDataLog));
  WaitingState* startApogeeLogState = new WaitingState(pBlipSystem, blip::toFunctional(&startDataLog));
  WaitingState* stopDataLogState = new WaitingState(pBlipSystem, blip::toFunctional(&stopDataLog));
  WaitingState* waitingLaunchState = new WaitingState(pBlipSystem, blip::toFunctional(&isLaunch), blip::toFunctional(&waitLaunchInit));

  setupState->setNextState(waitingLaunchState);
  waitingLaunchState->setNextState(startDataLogState);
  startDataLogState->setNextState(waitingFallState);
  waitingFallState->setNextState(apogeeDetectionState);
  apogeeDetectionState->setNextState(activationTimeInitState);
  activationTimeInitState->setNextState(stopDataLogState);
  stopDataLogState->setNextState(startApogeeLogState);
  startApogeeLogState->setNextState(isActivationTimeState);
  isActivationTimeState->setNextState(resetOffsetState);
  resetOffsetState->setNextState(thrusterState);
  thrusterState->setNextState(landingState);
  landingState->setNextState(saveState);

  pImuUnit->useRawGyroCorrection(pBlipSystem, "SetupState", "SetupState");
  
  pBlipSystem->changeState(setupState);
}

void loop() {
  pBlipSystem->execute();
  if (nextStateFlag) {
    pBlipSystem->getState()->next();
    nextStateFlag = false;
  }
}

void restartSystem() {
  pBlipSystem->init();
}

void serialEvent() {
  String cmd = Serial.readStringUntil('<');
  if (cmd == "resetAngles") {
    pBlipSystem->getImuUnitPointer()->resetAngles();
  } else if (cmd == "next") {
    nextStateFlag = true;
  }
  Serial.println(cmd);
}
