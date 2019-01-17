#include "stm32f4xx_hal.h"

void LowPower_init();
void LowPower_EnableWakeUpPin(uint32_t pin, uint32_t mode);
void LowPower_sleep(uint32_t regulator);
void LowPower_stop();
void LowPower_standby();
void LowPower_shutdown();
/* Weaked function */
void SystemClock_ConfigFromStop(void);
