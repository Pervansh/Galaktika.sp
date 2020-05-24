#include "BlipState.h"
#include "Arduino.h"

BlipEmptyState* BlipState::emptyState = new BlipEmptyState(nullptr);

BlipState::BlipState(BlipSystem* pBlipSystem, String id) {
    this->pBlipSystem = pBlipSystem;
    this->id = new String(id);
}

String BlipState::getId() { return *id; }

BlipState::~BlipState() {
    delete id;
}

BlipEmptyState::BlipEmptyState(BlipSystem* pBlipSystem)
: BlipState(pBlipSystem, "BlipEmptyState") {}
