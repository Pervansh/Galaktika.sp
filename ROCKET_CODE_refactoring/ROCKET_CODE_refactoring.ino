#include "config.h"
#include "BlipSystem.h"

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

void setup() {
  pBlipSystem->init();
}

void loop() {
  pBlipSystem->execute();
}

void restartSystem() {
  pBlipSystem->init();
}
