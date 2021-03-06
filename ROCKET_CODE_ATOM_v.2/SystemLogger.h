#ifndef _SYSTEM_LOGGER_BLIP_H_
#define _SYSTEM_LOGGER_BLIP_H_

#include <SD.h>
//#include "BlipSystem.h"

class BlipSystem;

class SystemLogger {
  private:
    BlipSystem* pBlipSystem;
    File file;
    String fileName;
    unsigned long startTimer = 0, logDelta, logTimer = 0;
  public:
    SystemLogger(BlipSystem* pBS, unsigned long _logDelta = 50);

    bool start();

    void logInfo();

    void logEvent(String eventName);

    void finish();
};

#endif
