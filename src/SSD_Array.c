// ****************************************************************
// * TEAM50: T. LUM and R. MARTIN
// * Helper file for SSD_Array.c
// * CPEG222,9/03/26 modified for JLCPCB SSD array
// ****************************************************************

#include "SSD_Array.h"
#include "stm32f4xx.h"
#include <stdbool.h>

#define SSDCA_PIN (9) // Assuming SSD segment CA is connected to GPIOG pin 9
#define SSDCA_PORT (GPIOG)
#define SSDCB_PIN (12) // Assuming SSD segment CB is connected to GPIOF pin 12
#define SSDCB_PORT (GPIOF)
#define SSDCC_PIN (13) // Assuming SSD segment CC is connected to GPIOF pin 13
#define SSDCC_PORT (GPIOF)
#define SSDCD_PIN (14) // Assuming SSD segment CD is connected to GPIOG pin 14
#define SSDCD_PORT (GPIOG)
#define SSDCE_PIN (8) // Assuming SSD segment CE is connected to GPIOE pin 8
#define SSDCE_PORT (GPIOE)
#define SSDCF_PIN (15) // Assuming SSD segment CF is connected to GPIOF pin 15
#define SSDCF_PORT (GPIOF)
#define SSDCG_PIN (4) // Assuming SSD segment CG is connected to GPIOB pin 4
#define SSDCG_PORT (GPIOB)
#define SSDCDP_PIN (14) // Assuming SSD segment DP is connected to GPIOF pin 14
#define SSDCDP_PORT (GPIOF)
#define DIGIT1_PIN  (10) // Assuming digit1 common pin is connected to GPIOE pin 12
#define DIGIT1_PORT (GPIOE)
#define DIGIT2_PIN (7) // Assuming digit2 common pin is connected to GPIOE pin 10
#define DIGIT2_PORT (GPIOE)
#define DIGIT3_PIN (5) // Assuming digit3 common pin is connected to GPIOF pin 12
#define DIGIT3_PORT (GPIOB)
#define DIGIT4_PIN (3) // Assuming digit4 common pin is connected to GPIOB pin 3
#define DIGIT4_PORT (GPIOB)



const unsigned char digitSegments[] = { //common anmode SSD segment patterns for digits 0-9 and blank

    ~0b0111111, // 0
    ~0b0000110, // 1
    ~0b1011011, // 2
    ~0b1001111, // 3
    ~0b1100110, // 4
    ~0b1101101, // 5
    ~0b1111101, // 6
    ~0b0000111, // 7
    ~0b1111111, // 8
    ~0b1101111, // 9
    ~0b0000000  // Blank
};


void SSD_init(void) { // Initialize GPIO pins for SSD

  // Enable GPIOA, GPIOB, and GPIOC clocks
  RCC->AHB1ENR |=
      RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOFEN | RCC_AHB1ENR_GPIOEEN| RCC_AHB1ENR_GPIOGEN;
  // Configure segment pins as outputs
  SSDCA_PORT->MODER &= ~(0x3 << (SSDCA_PIN * 2));   // Clear mode bits
  SSDCB_PORT->MODER &= ~(0x3 << (SSDCB_PIN * 2));   // Clear mode bits
  SSDCC_PORT->MODER &= ~(0x3 << (SSDCC_PIN * 2));   // Clear mode bits
  SSDCD_PORT->MODER &= ~(0x3 << (SSDCD_PIN * 2));   // Clear mode bits
  SSDCE_PORT->MODER &= ~(0x3 << (SSDCE_PIN * 2));   // Clear mode bits
  SSDCF_PORT->MODER &= ~(0x3 << (SSDCF_PIN * 2));   // Clear mode bits
  SSDCG_PORT->MODER &= ~(0x3 << (SSDCG_PIN * 2));   // Clear mode bits
  SSDCDP_PORT->MODER &= ~(0x3 << (SSDCDP_PIN * 2));   // Clear mode bits
  DIGIT1_PORT->MODER &= ~(0x3 << (DIGIT1_PIN * 2)); // Clear mode bits
  DIGIT2_PORT->MODER &= ~(0x3 << (DIGIT2_PIN * 2)); // Clear mode bits
  DIGIT3_PORT->MODER &= ~(0x3 << (DIGIT3_PIN * 2)); // Clear mode bits
  DIGIT4_PORT->MODER &= ~(0x3 << (DIGIT4_PIN * 2)); // Clear mode bits
  SSDCA_PORT->MODER |= (0x1 << (SSDCA_PIN * 2)); // Set as General purpose output mode
  SSDCB_PORT->MODER |= (0x1 << (SSDCB_PIN * 2)); // Set as General purpose output mode
  SSDCC_PORT->MODER |= (0x1 << (SSDCC_PIN * 2)); // Set as General purpose output mode
  SSDCD_PORT->MODER |= (0x1 << (SSDCD_PIN * 2)); // Set as General purpose output mode
  SSDCE_PORT->MODER |= (0x1 << (SSDCE_PIN * 2)); // Set as General purpose output mode
  SSDCF_PORT->MODER |= (0x1 << (SSDCF_PIN * 2)); // Set as General purpose output mode
  SSDCG_PORT->MODER |= (0x1 << (SSDCG_PIN * 2)); // Set as General purpose output mode
  SSDCDP_PORT->MODER |= (0x1 << (SSDCDP_PIN * 2)); // Set as General purpose output mode
  // Configure digit common pins as outputs
  DIGIT1_PORT->MODER |= (0x1 << (DIGIT1_PIN * 2)); // Set as General purpose output mode
  DIGIT2_PORT->MODER |= (0x1 << (DIGIT2_PIN * 2)); // Set as General purpose output mode
  DIGIT3_PORT->MODER |= (0x1 << (DIGIT3_PIN * 2)); // Set as General purpose output mode
  DIGIT4_PORT->MODER |= (0x1 << (DIGIT4_PIN * 2)); // Set as General purpose output mode
   // Set all segment and digit pins to low (turn off) 
  SSDCA_PORT->ODR &= ~(1 << SSDCA_PIN);
  SSDCB_PORT->ODR &= ~(1 << SSDCB_PIN);
  SSDCC_PORT->ODR &= ~(1 << SSDCC_PIN);
  SSDCD_PORT->ODR &= ~(1 << SSDCD_PIN);
  SSDCE_PORT->ODR &= ~(1 << SSDCE_PIN);
  SSDCF_PORT->ODR &= ~(1 << SSDCF_PIN);
  SSDCG_PORT->ODR &= ~(1 << SSDCG_PIN);
  SSDCDP_PORT->ODR &= ~(1 << SSDCDP_PIN);
  DIGIT1_PORT->ODR &= ~(1 << DIGIT1_PIN);
  DIGIT2_PORT->ODR &= ~(1 << DIGIT2_PIN);
  DIGIT3_PORT->ODR &= ~(1 << DIGIT3_PIN);
  DIGIT4_PORT->ODR &= ~(1 << DIGIT4_PIN);
}


void SSD_update(int digitSelect, int value, int decimalPoint) {

  int firstDigit = (value / 1000) % 10;
  int secondDigit = (value / 100) % 10;
  int thirdDigit = (value / 10) % 10;
  int fourthDigit = value % 10;

  DIGIT1_PORT->ODR |= (1 << DIGIT1_PIN); // Turn off common pin for first digit
  DIGIT2_PORT->ODR |= (1 << DIGIT2_PIN); // Turn off common pin for second digit
  DIGIT3_PORT->ODR |= (1 << DIGIT3_PIN); // Turn off common pin for third digit
  DIGIT4_PORT->ODR |= (1 << DIGIT4_PIN); // Turn off common pin for fourth digit
  switch (digitSelect) {
  case 0: { // Update first digit
    unsigned char segments = digitSegments[firstDigit];
    if (firstDigit == 0 && decimalPoint != 1) {
      segments = digitSegments[10]; // Blank leading zero
    }
    SSDCA_PORT->ODR = (SSDCA_PORT->ODR & ~(1 << SSDCA_PIN)) |
                      ((segments & 0x01) ? (1 << SSDCA_PIN) : 0);
    SSDCB_PORT->ODR = (SSDCB_PORT->ODR & ~(1 << SSDCB_PIN)) |
                      ((segments & 0x02) ? (1 << SSDCB_PIN) : 0);
    SSDCC_PORT->ODR = (SSDCC_PORT->ODR & ~(1 << SSDCC_PIN)) |
                      ((segments & 0x04) ? (1 << SSDCC_PIN) : 0);
    SSDCD_PORT->ODR = (SSDCD_PORT->ODR & ~(1 << SSDCD_PIN)) |
                      ((segments & 0x08) ? (1 << SSDCD_PIN) : 0);
    SSDCE_PORT->ODR = (SSDCE_PORT->ODR & ~(1 << SSDCE_PIN)) |
                      ((segments & 0x10) ? (1 << SSDCE_PIN) : 0);
    SSDCF_PORT->ODR = (SSDCF_PORT->ODR & ~(1 << SSDCF_PIN)) |
                      ((segments & 0x20) ? (1 << SSDCF_PIN) : 0);
    SSDCG_PORT->ODR = (SSDCG_PORT->ODR & ~(1 << SSDCG_PIN)) |
                      ((segments & 0x40) ? (1 << SSDCG_PIN) : 0);
    if (decimalPoint == 1) {
      SSDCDP_PORT->ODR &= ~(1 << SSDCDP_PIN); // Turn on decimal point
    } else {
      SSDCDP_PORT->ODR |= (1 << SSDCDP_PIN); // Turn off decimal point
    }
    DIGIT1_PORT->ODR &=
        ~(1 << DIGIT1_PIN); // Turn on common pin for first digit
    break;
  }
  case 1: { // Update second digit
    unsigned char segments1 = digitSegments[secondDigit];
    if (firstDigit == 0 && secondDigit == 0 && (decimalPoint != 2 && decimalPoint != 1)) {
      segments1 = digitSegments[10]; // Blank leading zero
    }
    SSDCA_PORT->ODR = (SSDCA_PORT->ODR & ~(1 << SSDCA_PIN)) |
                      ((segments1 & 0x01) ? (1 << SSDCA_PIN) : 0);
    SSDCB_PORT->ODR = (SSDCB_PORT->ODR & ~(1 << SSDCB_PIN)) |
                      ((segments1 & 0x02) ? (1 << SSDCB_PIN) : 0);
    SSDCC_PORT->ODR = (SSDCC_PORT->ODR & ~(1 << SSDCC_PIN)) |
                      ((segments1 & 0x04) ? (1 << SSDCC_PIN) : 0);
    SSDCD_PORT->ODR = (SSDCD_PORT->ODR & ~(1 << SSDCD_PIN)) |
                      ((segments1 & 0x08) ? (1 << SSDCD_PIN) : 0);
    SSDCE_PORT->ODR = (SSDCE_PORT->ODR & ~(1 << SSDCE_PIN)) |
                      ((segments1 & 0x10) ? (1 << SSDCE_PIN) : 0);
    SSDCF_PORT->ODR = (SSDCF_PORT->ODR & ~(1 << SSDCF_PIN)) |
                      ((segments1 & 0x20) ? (1 << SSDCF_PIN) : 0);
    SSDCG_PORT->ODR = (SSDCG_PORT->ODR & ~(1 << SSDCG_PIN)) |
                      ((segments1 & 0x40) ? (1 << SSDCG_PIN) : 0);
    if (decimalPoint == 2) {
      SSDCDP_PORT->ODR &= ~(1 << SSDCDP_PIN); // Turn on decimal point
    } else {
      SSDCDP_PORT->ODR |= (1 << SSDCDP_PIN); // Turn off decimal point
    }
    DIGIT2_PORT->ODR &=
        ~(1 << DIGIT2_PIN); // Turn on common pin for second digit
    break;
  }
  case 2: { // Update third digit
    unsigned char segments2 = digitSegments[thirdDigit];
    if (firstDigit == 0 && secondDigit == 0 && (thirdDigit == 0 && (decimalPoint == 0 || decimalPoint == 4))) {
      segments2 = digitSegments[10]; // Blank leading zero
    }
    SSDCA_PORT->ODR = (SSDCA_PORT->ODR & ~(1 << SSDCA_PIN)) |
                      ((segments2 & 0x01) ? (1 << SSDCA_PIN) : 0);
    SSDCB_PORT->ODR = (SSDCB_PORT->ODR & ~(1 << SSDCB_PIN)) |
                      ((segments2 & 0x02) ? (1 << SSDCB_PIN) : 0);
    SSDCC_PORT->ODR = (SSDCC_PORT->ODR & ~(1 << SSDCC_PIN)) |
                      ((segments2 & 0x04) ? (1 << SSDCC_PIN) : 0);
    SSDCD_PORT->ODR = (SSDCD_PORT->ODR & ~(1 << SSDCD_PIN)) |
                      ((segments2 & 0x08) ? (1 << SSDCD_PIN) : 0);
    SSDCE_PORT->ODR = (SSDCE_PORT->ODR & ~(1 << SSDCE_PIN)) |
                      ((segments2 & 0x10) ? (1 << SSDCE_PIN) : 0);
    SSDCF_PORT->ODR = (SSDCF_PORT->ODR & ~(1 << SSDCF_PIN)) |
                      ((segments2 & 0x20) ? (1 << SSDCF_PIN) : 0);
    SSDCG_PORT->ODR = (SSDCG_PORT->ODR & ~(1 << SSDCG_PIN)) |
                      ((segments2 & 0x40) ? (1 << SSDCG_PIN) : 0);
    if (decimalPoint == 3) {
      SSDCDP_PORT->ODR &= ~(1 << SSDCDP_PIN); // Turn on decimal point
    } else {
      SSDCDP_PORT->ODR |= (1 << SSDCDP_PIN); // Turn off decimal point
    }
    DIGIT3_PORT->ODR &=
        ~(1 << DIGIT3_PIN); // Turn on common pin for third digit
    break;
  }
  case 3: { // Update fourth digit
    unsigned char segments3 = digitSegments[fourthDigit];
    SSDCA_PORT->ODR = (SSDCA_PORT->ODR & ~(1 << SSDCA_PIN)) |
                      ((segments3 & 0x01) ? (1 << SSDCA_PIN) : 0);
    SSDCB_PORT->ODR = (SSDCB_PORT->ODR & ~(1 << SSDCB_PIN)) |
                      ((segments3 & 0x02) ? (1 << SSDCB_PIN) : 0);
    SSDCC_PORT->ODR = (SSDCC_PORT->ODR & ~(1 << SSDCC_PIN)) |
                      ((segments3 & 0x04) ? (1 << SSDCC_PIN) : 0);
    SSDCD_PORT->ODR = (SSDCD_PORT->ODR & ~(1 << SSDCD_PIN)) |
                      ((segments3 & 0x08) ? (1 << SSDCD_PIN) : 0);
    SSDCE_PORT->ODR = (SSDCE_PORT->ODR & ~(1 << SSDCE_PIN)) |
                      ((segments3 & 0x10) ? (1 << SSDCE_PIN) : 0);
    SSDCF_PORT->ODR = (SSDCF_PORT->ODR & ~(1 << SSDCF_PIN)) |
                      ((segments3 & 0x20) ? (1 << SSDCF_PIN) : 0);
    SSDCG_PORT->ODR = (SSDCG_PORT->ODR & ~(1 << SSDCG_PIN)) |
                      ((segments3 & 0x40) ? (1 << SSDCG_PIN) : 0);
    if (decimalPoint == 4) {
      SSDCDP_PORT->ODR &= ~(1 << SSDCDP_PIN); // Turn on decimal point
    } else {
      SSDCDP_PORT->ODR |= (1 << SSDCDP_PIN); // Turn off decimal point
    }
    DIGIT4_PORT->ODR &=
        ~(1 << DIGIT4_PIN); // Turn  on common pin for fourth digit
    break;
  }
  default:
    break;
  }
}
