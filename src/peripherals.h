#ifndef _VVC_PERIPHS_H
#define _VVC_PERIPHS_H

#include "global.h"

/* Timer Peripherals */
void stop_timer(TIM_TypeDef *TIMx);
void start_ms_timer(TIM_TypeDef *TIMx,
                    uint16_t ms,
                    uint8_t with_interrupt);
void start_timer(TIM_TypeDef *TIMx,
                 uint16_t prescaler,
                 uint16_t period,
                 uint8_t  with_interrupt);

#endif
