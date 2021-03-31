#include "BlipState.h"
#include "BlipSystem.h"
#include "Arduino.h"

BlipEmptyState* BlipState::emptyState = new BlipEmptyState(nullptr);

BlipState::BlipState(BlipSystem* pBlipSystem, String id)
 : BlipState(pBlipSystem, id, getBlipEmptyState()) {}

BlipState::BlipState(BlipSystem* pBlipSystem, String id, BlipState* nextState) {
    this->pBlipSystem = pBlipSystem;
    this->id = new String(id);
    this->nextState = nextState;
}

void BlipState::next() {
    pBlipSystem->changeState(nextState);
}

String BlipState::getId() { return *id; }

BlipState::~BlipState() {
    delete id;
}

BlipEmptyState::BlipEmptyState(BlipSystem* pBlipSystem)
: BlipState(pBlipSystem, "BlipEmptyState") {}
