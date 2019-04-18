#include "Arduino.h"
#include <stm32f4xx_hal.h>

void HAL_Delay2(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(HAL_TICK_FREQ_DEFAULT);
  }

  while((HAL_GetTick() - tickstart) < wait)
  {
  	__WFI();
  }
}

void delay(unsigned long ms)
{
	HAL_Delay2(ms);
}

void delayMicroseconds(int us)
{
	unsigned long end = micros() + us;
	long diff;
	do {
		diff = micros() - end;
	} while (diff < 0);
}

unsigned long micros()
{
	unsigned long ms;
	uint32_t sysTickCurrent;
	do {
		ms = millis();
		sysTickCurrent = SysTick->VAL;
	} while (millis() != ms || SysTick->VAL > sysTickCurrent);

	uint32_t sysTickReload = SysTick->LOAD;

	return ms * 1000 + (sysTickReload - sysTickCurrent) * 1000 / (sysTickReload + 1);
}

unsigned long millis()
{
	return HAL_GetTick();
}
