#include "interrupts_c.h"

// C-language hardware interrupt method definitions.
/*
 * Each EXTI line between 0-15 can map to a GPIO pin.
 * The EXTI line number is the same as the pin number, and
 * each pin number can only have its interrupt active on
 * ONE (1) of its GPIO banks. So for example, you can have
 * an interrupt on pin A0 (EXTI line 0) and pin B1 (EXTI line 1)
 * at the same time, but not on pins A1 (EXTI line 1)
 * and B1 (EXTI line 1), since they share the same interrupt line.
 */
// Common definitions for each line, independent of
// available NVIC interrupts on any specific chip.
inline void EXTI0_line_interrupt(void) {
  // 'Down' button.
}

inline void EXTI1_line_interrupt(void) {
  // 'Right' button.
}

inline void EXTI2_line_interrupt(void) {
  // (Unused)
}

inline void EXTI3_line_interrupt(void) {
  // (Unused)
}

inline void EXTI4_line_interrupt(void) {
  // (Unused)
}

inline void EXTI5_line_interrupt(void) {
  // (Unused)
}

inline void EXTI6_line_interrupt(void) {
  // 'Left' button.
}

inline void EXTI7_line_interrupt(void) {
  // 'Up' button.
}

inline void EXTI8_line_interrupt(void) {
  // 'B' button.
}

inline void EXTI9_line_interrupt(void) {
  // 'A' button.
}

inline void EXTI10_line_interrupt(void) {
  // (Unused)
}

inline void EXTI11_line_interrupt(void) {
  // (Unused)
}

inline void EXTI12_line_interrupt(void) {
  // (Unused)
}

inline void EXTI13_line_interrupt(void) {
  // (Unused)
}

inline void EXTI14_line_interrupt(void) {
  // (Unused)
}

inline void EXTI15_line_interrupt(void) {
  // (Unused)
}

#ifdef VVC_F0
// STM32F0xx EXTI lines.
/*
 * EXTI0_1: Handle interrupt lines 0 and 1.
 */
void EXTI0_1_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR0) {
  EXTI->PR |= EXTI_PR_PR0;
  EXTI0_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR1) {
  EXTI->PR |= EXTI_PR_PR1;
  EXTI1_line_interrupt();
}
return;
}

/*
 * EXTI2_3: Handle interrupt lines 2 and 3.
 */
void EXTI2_3_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR2) {
  EXTI->PR |= EXTI_PR_PR2;
  EXTI2_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR3) {
  EXTI->PR |= EXTI_PR_PR3;
  EXTI3_line_interrupt();
}
return;
}

/*
 * EXTI4_15: Handle interrupt lines between [4:15], inclusive.
 */
void EXTI4_15_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR4) {
  EXTI->PR |= EXTI_PR_PR4;
  EXTI4_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR5) {
  EXTI->PR |= EXTI_PR_PR5;
  EXTI5_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR6) {
  EXTI->PR |= EXTI_PR_PR6;
  EXTI6_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR7) {
  EXTI->PR |= EXTI_PR_PR7;
  EXTI7_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR8) {
  EXTI->PR |= EXTI_PR_PR8;
  EXTI8_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR9) {
  EXTI->PR |= EXTI_PR_PR9;
  EXTI9_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR10) {
  EXTI->PR |= EXTI_PR_PR10;
  EXTI10_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR11) {
  EXTI->PR |= EXTI_PR_PR11;
  EXTI11_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR12) {
  EXTI->PR |= EXTI_PR_PR12;
  EXTI12_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR13) {
  EXTI->PR |= EXTI_PR_PR13;
  EXTI13_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR14) {
  EXTI->PR |= EXTI_PR_PR14;
  EXTI14_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR15) {
  EXTI->PR |= EXTI_PR_PR15;
  EXTI15_line_interrupt();
}
return;
}


#elif VVC_F3
// STM32F3xx(?) EXTI lines.
void EXTI2_touchsense_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR2) {
  EXTI->PR |= EXTI_PR_PR2;
  EXTI2_line_interrupt();
}
return;
}

void EXTI3_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR3) {
  EXTI->PR |= EXTI_PR_PR3;
  EXTI3_line_interrupt();
}
return;
}

void EXTI4_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR4) {
  EXTI->PR |= EXTI_PR_PR4;
  EXTI4_line_interrupt();
}
return;
}

void EXTI5_9_IRQ_handler(void) {
if (EXTI->PR & EXTI_PR_PR5) {
  EXTI->PR |= EXTI_PR_PR5;
  EXTI5_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR6) {
  EXTI->PR |= EXTI_PR_PR6;
  EXTI6_line_interrupt();
}
if (EXTI->PR & EXTI_PR_PR7) {
  EXTI->PR |= EXTI_PR_PR7;
  EXTI7_line_interrupt();
}
return;
}

#endif

// Interrupts common to all supported chips.
void TIM2_IRQ_handler(void) {
  // Handle a timer 'update' interrupt event
  if (TIM2->SR & TIM_SR_UIF) {
    TIM2->SR &= ~(TIM_SR_UIF);
    // (TIM2 timer interrupt code)
  }
}
