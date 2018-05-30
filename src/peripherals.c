#include "peripherals.h"

/* Timer Peripherals */

/*
 * Stop a running timer on a given Timer peripheral.
 */
void stop_timer(TIM_TypeDef *TIMx) {
  // Turn off the timer's 'counter'.
  TIMx->CR1 &= ~(TIM_CR1_CEN);
  // Clear the 'pending update interrupt' flag.
  TIMx->SR  &= ~(TIM_SR_UIF);
}

/*
 * 'Start Milliseconds Timer'
 * This method starts a timer to tick over every N
 * milliseconds, and optionally trigger a hardware interrupt.
 */
void start_ms_timer(TIM_TypeDef *TIMx,
                    uint16_t ms,
                    uint8_t with_interrupt) {
  start_timer(TIMx, core_clock_hz / 1000, ms, with_interrupt);
}

/*
 * 'Start Timer'
 * This method starts a new 'counter' timer, which triggers
 * an 'UPDATE' interrupt every time that it triggers.
 * It should trigger every N seconds, where:
 * N = (period) / (core clock frequency / prescaler)
 */
void start_timer(TIM_TypeDef *TIMx,
                 uint16_t prescaler,
                 uint16_t period,
                 uint8_t  with_interrupt) {
  // Initialize the 'TIMx' timer peripheral. (TODO: F303 support)
  // Start by making sure the timer's 'counter' is off.
  TIMx->CR1 &= ~(TIM_CR1_CEN);
  // Next, reset the peripheral. (This is where a HAL can help)
  if (TIMx == TIM2) {
    RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);
  }
  else if (TIMx == TIM3) {
    RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM3RST);
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM3RST);
  }
  // Set clock division to 1; the timer triggers every N events.
  // Also set the counter to count up.
  TIMx->CR1  &= ~(TIM_CR1_DIR |
                  TIM_CR1_CKD);
  // Set the 'prescaler'; the timer 'ticks' every N clock cycles.
  TIMx->PSC   =  prescaler;
  // Set 'auto-reload'; this is the timer's period in 'ticks'.
  TIMx->ARR   =  period;
  // Before enabling the counter, trigger an 'update' event to
  // make sure that the chip registers the 'prescaler' settings.
  TIMx->EGR  |=  (TIM_EGR_UG);
  if (with_interrupt) {
    // Enable the 'update' timer event/interrupt.
    TIMx->DIER |=  (TIM_DIER_UIE);
  }
  // Finally, enable the timer.
  TIMx->CR1  |=  (TIM_CR1_CEN);
}
