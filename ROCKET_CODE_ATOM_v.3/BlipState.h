#ifndef _BLIP_STATE_BLIP_H_
#define _BLIP_STATE_BLIP_H_

//#include "BlipSystem.h"

class BlipSystem;

class BlipState {
  protected:
    BlipSystem* pBlipSystem;
  public:
    BlipState(BlipSystem* pBlipSystem){
      this->pBlipSystem = pBlipSystem;
    }
    
    virtual void execute() = 0;
};

#endif
