#ifndef __MAX7219_H
#define __MAX7219_H

#include "stm32l4xx_hal.h"

/* MAX7219 Register Addresses */
#define MAX7219_REG_NOP          0x00
#define MAX7219_REG_DIGIT0       0x01
#define MAX7219_REG_DIGIT1       0x02
#define MAX7219_REG_DIGIT2       0x03
#define MAX7219_REG_DIGIT3       0x04
#define MAX7219_REG_DIGIT4       0x05
#define MAX7219_REG_DIGIT5       0x06
#define MAX7219_REG_DIGIT6       0x07
#define MAX7219_REG_DIGIT7       0x08
#define MAX7219_REG_DECODE_MODE  0x09
#define MAX7219_REG_INTENSITY    0x0A
#define MAX7219_REG_SCAN_LIMIT   0x0B
#define MAX7219_REG_SHUTDOWN     0x0C
#define MAX7219_REG_DISPLAY_TEST 0x0F

/* Driver State */
typedef enum {
    MAX7219_STATE_READY,
    MAX7219_STATE_BUSY_TX,
    MAX7219_STATE_ERROR
} MAX7219_State_t;

/* Driver Handle Structure */
typedef struct {
    SPI_HandleTypeDef *hspi;       // HAL SPI Handle
    GPIO_TypeDef      *cs_port;    // GPIO Port for CS pin (SS)
    uint16_t           cs_pin;     // GPIO Pin for CS pin
    MAX7219_State_t    state;      // Driver state
    uint8_t            tx_buffer[2]; // Temp buffer for single register transmission
    uint8_t            frame_buffer[16]; // Buffer to send all 8 lines (8 reg-value pairs)
    uint8_t            current_line; // Tracker for multi-step transmissions
} MAX7219_HandleTypeDef;

/* Public Function Prototypes */

/**
  * @brief Initializes the MAX7219 LED matrix driver.
  * @param hmax: Pointer to a MAX7219_HandleTypeDef structure.
  * @param hspi: Pointer to a HAL SPI_HandleTypeDef structure.
  * @param cs_port: GPIO Port for Chip Select (SS).
  * @param cs_pin: GPIO Pin for Chip Select (SS).
  * @retval HAL Status
  */
HAL_StatusTypeDef MAX7219_Init(MAX7219_HandleTypeDef *hmax, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin);

/**
  * @brief Sends a register write command non-blocking (interrupt-based).
  * @param hmax: Pointer to a MAX7219_HandleTypeDef structure.
  * @param reg: MAX7219 Register address (MAX7219_REG_...).
  * @param data: Data byte to write.
  * @retval HAL Status
  */
HAL_StatusTypeDef MAX7219_WriteReg_IT(MAX7219_HandleTypeDef *hmax, uint8_t reg, uint8_t data);

/**
  * @brief Displays an 8x8 pattern on the LED matrix non-blocking.
  * @param hmax: Pointer to a MAX7219_HandleTypeDef structure.
  * @param pattern: Pointer to an 8-byte array representing rows 0-7.
  * @retval HAL Status
  */
HAL_StatusTypeDef MAX7219_DisplayPattern_IT(MAX7219_HandleTypeDef *hmax, const uint8_t *pattern);

/**
  * @brief Callback function that must be called from HAL_SPI_TxCpltCallback.
  * @param hmax: Pointer to a MAX7219_HandleTypeDef structure.
  * @retval None
  */
void MAX7219_TxCpltCallback(MAX7219_HandleTypeDef *hmax);

#endif /* __MAX7219_H */
