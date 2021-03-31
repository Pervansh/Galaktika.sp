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

BlipSystem *pBlipSystem = new BlipSystem();

/*          ЗАМЕТКИ
   ( (-) - не исправлено, (+) - исправлено)
   -Отключена проверка состояния
   -Отключена проверка BMP280
   -Перед запуском проверь код
   -Парашют отключен!!!!!!!!!!!!!!!
   -Время калибровки уменьшено
   +Время между логами уменьшено
   +Отключена проверка работы SD Card Holder'а
   +Задержка в лупе!!!
   +Постоянная коректировка по акселерометру
*/

SetupState* setupState = new SetupState(pBlipSystem, SAFE_SWITCH);
WaitingStartState* waitingStartState = new WaitingStartState(pBlipSystem, false);
ThrusterState* thrusterState = new ThrusterState(pBlipSystem);
LandingState* landingState = new LandingState(pBlipSystem);
SaveState* saveState = new SaveState(pBlipSystem);

void attachEvents() {
  BlipFunctorSubscriber* logSub = blipMakeFuncSub(BLIP_SUB_FUNC(system, event) {
    if (system->getState()->getId().equals("ThrusterState")) {
      system->getSystemLogger()->start();
    }
  } );
}

void setup() {
  pBlipSystem->init();

  setupState->setNextState(waitingStartState);
  waitingStartState->setNextState(thrusterState);
  thrusterState->setNextState(landingState);
  landingState->setNextState(saveState);

  pBlipSystem->changeState(setupState);
}

void loop() {
  pBlipSystem->execute();
}

void restartSystem() {
  pBlipSystem->init();
}
