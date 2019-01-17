#include "low_power.h"

/* Save callback pointer */
static void (*WakeUpUartCb)( void ) = NULL;

/**
  * @brief  Initialize low power mode
  * @param  None
  * @retval None
  */
void LowPower_init(){
  /* Allow access to Backup domain */
  HAL_PWR_EnableBkUpAccess();

#ifdef __HAL_RCC_WAKEUPSTOP_CLK_CONFIG
  /* Ensure that HSI is wake-up system clock */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);
#endif

  /* Check if the system was resumed from StandBy mode */
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }

  /* Clear all related wakeup flags */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
}

/**
  * @brief  Configure a pin as wakeup source if compatible.
  * @param  pin: pin to configure
  * @param  mode: pin mode (edge or state). The configuration have to be compatible.
  * @retval None
  */
void LowPower_EnableWakeUpPin(uint32_t pin, uint32_t mode) {
  UNUSED(pin);
  UNUSED(mode);
}

/**
  * @brief  Enable the sleep mode.
  * @param  None
  * @retval None
  */
void LowPower_sleep(uint32_t regulator){
  /*
   * Suspend Tick increment to prevent wakeup by Systick interrupt.
   * Otherwise the Systick interrupt will wake up the device within
   * 1ms (HAL time base)
   */
  HAL_SuspendTick();

  /* Enter Sleep Mode , wake up is done once User push-button is pressed */
  HAL_PWR_EnterSLEEPMode(regulator, PWR_SLEEPENTRY_WFI);

  /* Resume Tick interrupt if disabled prior to SLEEP mode entry */
  HAL_ResumeTick();

  if (WakeUpUartCb != NULL) {
    WakeUpUartCb();
  }
}

/**
  * @brief  Enable the stop mode.
  * @param  obj : pointer to serial_t structure
  * @retval None
  */
void LowPower_stop(){
  __disable_irq();

#ifdef __HAL_RCC_WAKEUPSTOP_CLK_CONFIG
  /* Select HSI as system clock source after Wake Up from Stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);
#endif

  /* Enter Stop mode */
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* Exit Stop mode reset clocks */
  SystemClock_ConfigFromStop();
  __enable_irq();

  HAL_Delay(10);

  if (WakeUpUartCb != NULL) {
    WakeUpUartCb();
  }
}

/**
  * @brief  Enable the standby mode. The board reset when leaves this mode.
  * @param  None
  * @retval None
  */
void LowPower_standby(){
  __disable_irq();
  HAL_PWR_EnterSTANDBYMode();
}

/**
  * @brief  Enable the shutdown mode.The board reset when leaves this mode.
  *         If shutdown mode not available, use standby mode instead.
  * @param  None
  * @retval None
  */
void LowPower_shutdown(){
  __disable_irq();
  {
    LowPower_standby();
  }
}

/**
  * @brief  Configures system clock after wake-up from STOP
  * @note   Weaked function which can be redefined by user at the sketch level.
  *         By default, call 'SystemClock_Config()'.
  * @param  None
  * @retval None
  */
void SystemClock_ConfigFromStop(void) {
  SystemClock_Config();
}
