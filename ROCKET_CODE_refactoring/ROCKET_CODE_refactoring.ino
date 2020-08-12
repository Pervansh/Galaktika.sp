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
#include "TrieEventManager.h"
#include "BlipRangeSigner.h"

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

class TestSub : public BlipSubscriber {
  private:
    bool b = true;
  public:
    void update(BlipSystem* system, const String& event) {
      if (event == BlipEventTypes::onExecution && b) {
        Serial.println("TESTING");
      }/* else if (event == BlipEventTypes::onBlipStateChange && system->getState()->getId() == "LandingState") {
      system->unsubscribe(this, BlipEventTypes::onExecution);
      Serial.println("-Prokladka-");
      system->unsubscribe(this, BlipEventTypes::onBlipStateChange);
    }*/
    }

    void spam(BlipSystem* system, const String& event) {
      Serial.println("!TEST SPAM!");
    }
};

MpuOrintationUnit* pImuUnit = new MpuOrintationUnit(AXIS_ROTATION, true);
TrieEventManager* eventManager = new TrieEventManager();

BlipSystem *pBlipSystem = new BlipSystem(pImuUnit, eventManager);

SetupState* setupState = new SetupState(pBlipSystem, SAFE_SWITCH);
WaitingStartState* waitingStartState = new WaitingStartState(pBlipSystem, false);
ThrusterState* thrusterState = new ThrusterState(pBlipSystem);
LandingState* landingState = new LandingState(pBlipSystem);
SaveState* saveState = new SaveState(pBlipSystem);

void attachTestEvents();

void setup() {
  // pImuUnit = new MpuOrintationUnit(AXIS_ROTATION, true);
  // pBlipSystem = new BlipSystem(pImuUnit);
  // pBlipSystem = new BlipSystem();
  // Serial.begin(115200);
  // while (!Serial) {}
  Serial.println(">> preInit");
  pBlipSystem->init();
  Serial.println(">> postInit (v0.4)!-(\\+(--)+/)-!");

  setupState->setNextState(waitingStartState);
  waitingStartState->setNextState(thrusterState);
  thrusterState->setNextState(landingState);
  landingState->setNextState(saveState);

  // pImuUnit->useRawGyroCorrection(pBlipSystem, "SetupState", "SetupState");

  attachTestEvents();

  pBlipSystem->changeState(setupState);
}

void loop() {
  pBlipSystem->execute();
}

void restartSystem() {
  pBlipSystem->init();
}

void attachTestEvents() {
  /*
    auto helper = BLIP_SUB_FUNC(system, event) {
    if (system->getState()->getId() == "WaitingStartState") {
      system->getImuUnitPointer()->useAccelCorrection(false);
    }
    };
    auto helperSub = blipMakeFuncSub<decltype(helper)>(&helper);
    pBlipSystem->subscribe(helperSub, BlipEventTypes::onBlipStateChange);
  */
  /*
    auto spamer = blipMakeFuncSub(BLIP_SUB_FUNC(system, event) {
    Serial.println("!SPAM!");
    });
  */
  auto test = new TestSub();
  auto spamer = blipMakeFuncSub(&TestSub::spam, test);
  auto dop = BLIP_SUB_FUNC(system, event) {
    Serial.println("DOP");
  };
  auto dopSub = blipMakeFuncSub<decltype(dop)>(&dop);
  // auto spamSigner = new BlipRangeSigner(spamer, BlipEventTypes::onExecution, "WaitingStartState", "ThrusterState");
  auto testSigner = new BlipRangeSigner(test, BlipEventTypes::onExecution, "ThrusterState", "ThrusterState");
  auto dopSigner = new BlipRangeSigner(dopSub, BlipEventTypes::onExecution, "WaitingStartState", "WaitingStartState");
  // spamSigner->init(pBlipSystem, true);
  testSigner->init(pBlipSystem, true);
  dopSigner->init(pBlipSystem, true);
  // pBlipSystem->subscribe(dopSub, BlipEventTypes::onExecution);
  // pBlipSystem->subscribe(spamer, BlipEventTypes::onExecution);
  /*
    auto* trigger = blipMakeFuncSub(BLIP_SUB_FUNC(system, event) {
    if (event == BlipEventTypes::onBlipStateChange && system->getState()->getId() == "WaitingStartState") {
      TestSub* sub = new TestSub();
      auto dop = blipMakeFuncSub(BLIP_SUB_FUNC(system, event) {
        Serial.println("DOP");
      });
      system->subscribe(sub, BlipEventTypes::onExecution);
      system->subscribe(sub, BlipEventTypes::onBlipStateChange);
      system->subscribe(dop, BlipEventTypes::onExecution);
    }
    });

    pBlipSystem->subscribe(trigger, BlipEventTypes::onBlipStateChange);
  */
}
