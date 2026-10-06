/****************************************************************
* Author: T. LUM and R. MARTIN
* CPEG222 DemoProgram10, 10/6/26
* NucleoF446ZE CMSIS STM32F4xx example F26Demo10_Servo_Pot_SSD
* The shield potentiometer (V_POT, PC2, ADC3_IN12) sets the PWM pulse
* width for a standard servo on PC6 (TIM3_CH1, shield Servo0 "Radar").
* Turning the pot moves the servo from 1000 us to 2000 us, and the pulse
* width in microseconds is shown on the 4-digit SSD array.
*   - TIM3 makes the 50 Hz servo PWM signal
*   - TIM5 interrupt refreshes one SSD digit every 0.5 ms
*   - ADC3 reads the pot (16-sample average) once per PWM period (20 ms)
* System clock is 180 MHz: APB1 timers run at 90 MHz, APB2 at 90 MHz.
*
* Revision History:
* V1  10/6/26  Initial version
****************************************************************/
#include "stm32f4xx.h"       // Include the STM32F4xx CMSIS header file
#include "SysClock180MHz.h"  // 180 MHz system clock configuration
#include "SSD_Array.h"       // 4-digit SSD array functions
#include <stdint.h>          // Include stdint.h for uint32_t type

#define SERVO0_PORT    GPIOC
#define SERVO0_PIN     6               // PC6 = TIM3_CH1 (AF2), shield Servo0 (Radar)
#define ADC_PORT       GPIOC
#define ADC_PIN        2               // V_POT on PC2
#define ADC_CHANNEL    12              // PC2 = ADC3_IN12
#define ADC_SAMPLES    16              // Number of samples to average
#define TIMER_CLOCK    90000000UL      // TIM3 and TIM5 clock at 180 MHz SYSCLK (APB1 45 MHz x 2)
#define PWM_PERIOD_US  20000           // 20,000 us = 20 ms period = 50 Hz
#define SERVO_MIN_US   600            // .6 ms pulse = one end of travel
#define SERVO_MAX_US   2400            // 2.4 ms pulse = other end of travel

volatile int digitSelect = 0;   // Which SSD digit the TIM5 ISR shows next (0-3)
volatile int decimalPoint = 0;  // 0 = no decimal point
volatile int SSD_value = 1500;  // Value shown on the SSD array (pulse width in us)
uint32_t pulse_width = 1500;    // Current servo pulse width in us

void PWM_Output_PC6_Init(void);
void ADC_Init(void);
void SSD_Timer_Init(void);
uint32_t ADC_ReadAverage(void);
void delay_ms(uint32_t ms);

void TIM5_IRQHandler(void) {
    if (TIM5->SR & TIM_SR_UIF) {   // Check if the update interrupt flag is set
        TIM5->SR = ~TIM_SR_UIF;    // Clear flag first (rc_w0 register)
        SSD_update(digitSelect, SSD_value, decimalPoint); // Display current SSD_value
        digitSelect = (digitSelect + 1) % 4; // Cycle through digitSelect values 0 to 3
    }
}

int main(void) {
    SystemClock_Config();    // Configure the system clock to 180 MHz
    SystemCoreClockUpdate(); // Update SystemCoreClock variable after clock change
    PWM_Output_PC6_Init();   // Start 50 Hz PWM on PC6, servo starts at 1500 us
    ADC_Init();              // Pot on PC2
    SSD_init();              // SSD array GPIO pins
    SSD_Timer_Init();        // Start TIM5 interrupt to refresh the SSD array

    while (1) {
        uint32_t analog_value = ADC_ReadAverage(); // 0 to 4095
        // Map 0-4095 to = 600-2400 us
        pulse_width = SERVO_MIN_US + (analog_value * (SERVO_MAX_US - SERVO_MIN_US)) / 4095;
        TIM3->CCR1 = pulse_width; // 1 tick = 1 us, so CCR1 is the pulse width in us
        SSD_value = pulse_width;  // Show the pulse width on the SSD array
        delay_ms(20);             // Update once per PWM period
    }
    return 0; // Program should never reach here due to infinite loop
}

void PWM_Output_PC6_Init(void) {
    // 1. Enable GPIOC and TIM3 clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    // 2. Set PC6 to alternate function mode (MODER = 10b)
    SERVO0_PORT->MODER &= ~(0x3 << (SERVO0_PIN * 2));
    SERVO0_PORT->MODER |=  (0x2 << (SERVO0_PIN * 2));
    // 3. Select AF2 (TIM3_CH1) for PC6. Pins 0-7 use AFR[0], pins 8-15 use AFR[1]
    SERVO0_PORT->AFR[0] &= ~(0xF << (SERVO0_PIN * 4));
    SERVO0_PORT->AFR[0] |=  (0x2 << (SERVO0_PIN * 4));
    // 4. Configure TIM3 for PWM output on CH1
    TIM3->CR1 &= ~TIM_CR1_CEN;                   // Disable TIM3 before configuring it
    TIM3->PSC = (TIMER_CLOCK / 1000000UL) - 1;   // 90 MHz / 90 = 1 MHz timer clock (1 us per tick)
    TIM3->ARR = PWM_PERIOD_US - 1;               // 20,000 ticks = 20 ms period (50 Hz)
    TIM3->CCR1 = pulse_width;                    // 1,500 ticks = 1.5 ms pulse (neutral)
    TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M); // CH1 as output, clear output mode
    TIM3->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);    // PWM mode 1: high while CNT < CCR1
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;              // Preload enable: new CCR1 takes effect at next period
    TIM3->CCER |= TIM_CCER_CC1E;                 // Enable CH1 output on PC6
    TIM3->CR1 |= TIM_CR1_ARPE;                   // Auto-reload preload enable
    TIM3->EGR = TIM_EGR_UG;                      // Generate update event to load PSC, ARR, CCR1
    TIM3->CR1 |= TIM_CR1_CEN;                    // Enable timer
}

void ADC_Init(void) {
    // 1. Enable GPIOC clock and set PC2 to analog mode (MODER = 11b)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    ADC_PORT->MODER &= ~(0x3 << (ADC_PIN * 2));
    ADC_PORT->MODER |=  (0x3 << (ADC_PIN * 2));
    // 2. ADC clock: APB2 is 90 MHz at 180 MHz SYSCLK. The ADC clock must be 36 MHz or less,
    //    so divide by 4: 90 MHz / 4 = 22.5 MHz (the default /2 would give 45 MHz, too fast)
    RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;          // Enable ADC3 clock
    ADC->CCR = (ADC->CCR & ~ADC_CCR_ADCPRE) | ADC_CCR_ADCPRE_0; // ADCPRE = 01b, PCLK2 / 4
    // 3. Initialize ADC, default resolution is 12 bits
    ADC3->SQR3 = ADC_CHANNEL;                    // Select channel 12
    ADC3->SMPR1 = ADC_SMPR1_SMP12_0 | ADC_SMPR1_SMP12_1; // Sample time 56 cycles (0b011)
    ADC3->CR2 = ADC_CR2_ADON;                    // Enable ADC
}

uint32_t ADC_ReadAverage(void) {
    uint32_t total = 0;
    for (int i = 0; i < ADC_SAMPLES; i++) {
        ADC3->CR2 |= ADC_CR2_SWSTART;            // Start conversion
        while (!(ADC3->SR & ADC_SR_EOC));        // Wait for conversion to complete
        total += ADC3->DR;                       // Read data (also clears EOC)
    }
    return total / ADC_SAMPLES;                  // Average removes pot jitter on the SSD
}

void SSD_Timer_Init(void) {
    // TIM5 interrupt every 0.5 ms, one digit per interrupt (500 Hz full refresh)
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    TIM5->PSC = (TIMER_CLOCK / 1000000UL) - 1;   // 90 MHz / 90 = 1 MHz -> 1 us tick
    TIM5->ARR = 500 - 1;                         // 500 ticks x 1 us = 0.5 ms per digit
    TIM5->EGR = TIM_EGR_UG;                      // Load PSC now so the first period is correct
    TIM5->SR = ~TIM_SR_UIF;                      // Clear UIF set by UG
    TIM5->DIER |= TIM_DIER_UIE;                  // Enable update interrupt
    NVIC_SetPriority(TIM5_IRQn, 5);              // Set priority for TIM5
    NVIC_EnableIRQ(TIM5_IRQn);                   // Enable TIM5 interrupt in NVIC
    TIM5->CR1 |= TIM_CR1_CEN;                    // Enable TIM5
}

void delay_ms(uint32_t ms) {
    if (SystemCoreClock == 16000000U) { // If the system clock is 16 MHz
        for (uint32_t i = 0; i < ms * 2000; i++) {
            __NOP();
        }
    } else if (SystemCoreClock == 180000000U) { // If the system clock is 180 MHz
        for (uint32_t i = 0; i < ms * 22500; i++) {
            __NOP();
        }
    }
}
