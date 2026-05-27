#ifndef __PCF8591_H
#define __PCF8591_H

#include "stm32l4xx_hal.h"

/* PCF8591 I2C Address (7-bit address 0x48, shifted left for HAL) */
#define PCF8591_I2C_ADDR         (0x48 << 1)

/* PCF8591 Control Register Definitions */
#define PCF8591_CTRL_DAC_ENABLE  0x40  // Bit 6: Analog output enable
#define PCF8591_CTRL_AUTO_INC    0x04  // Bit 2: Auto-increment mode

/* Analog Input Channels */
#define PCF8591_CHANNEL_AIN0     0x00  // AIN0: LDR (Luminosidade)
#define PCF8591_CHANNEL_AIN1     0x01  // AIN1: Thermistor (Temperatura)
#define PCF8591_CHANNEL_AIN2     0x02  // AIN2: Unused
#define PCF8591_CHANNEL_AIN3     0x03  // AIN3: Potentiometer (Volt)

/* Driver state */
typedef enum {
    PCF8591_STATE_READY,
    PCF8591_STATE_BUSY_RX,
    PCF8591_STATE_BUSY_TX,
    PCF8591_STATE_ERROR
} PCF8591_State_t;

/* Driver Handle Structure */
typedef struct {
    I2C_HandleTypeDef *hi2c;      // HAL I2C Handle
    PCF8591_State_t    state;     // Driver state
    uint8_t            control_reg; // Last written control register
    uint8_t            rx_buffer[2]; // PCF8591 returns previous conversion first, so we read 2 bytes
    uint8_t            tx_buffer[2]; // Buffer for control byte and DAC value
} PCF8591_HandleTypeDef;

/* Public Function Prototypes */

/**
  * @brief Initializes the PCF8591 driver context.
  * @param hpcf: Pointer to a PCF8591_HandleTypeDef structure.
  * @param hi2c: Pointer to a HAL I2C_HandleTypeDef structure.
  * @retval HAL Status
  */
HAL_StatusTypeDef PCF8591_Init(PCF8591_HandleTypeDef *hpcf, I2C_HandleTypeDef *hi2c);

/**
  * @brief Requests a non-blocking (interrupt-based) read of an analog channel.
  * @param hpcf: Pointer to a PCF8591_HandleTypeDef structure.
  * @param channel: The channel to read (PCF8591_CHANNEL_AIN0 to AIN3).
  * @retval HAL Status
  */
HAL_StatusTypeDef PCF8591_ReadChannel_IT(PCF8591_HandleTypeDef *hpcf, uint8_t channel);

/**
  * @brief Sets the DAC output value (0 - 255) using non-blocking (interrupt-based) I2C write.
  * @param hpcf: Pointer to a PCF8591_HandleTypeDef structure.
  * @param value: DAC value to write (0 = 0V, 255 = Vcc).
  * @retval HAL Status
  */
HAL_StatusTypeDef PCF8591_WriteDAC_IT(PCF8591_HandleTypeDef *hpcf, uint8_t value);

/**
  * @brief Callback function that must be called from HAL_I2C_MasterRxCpltCallback.
  * @param hpcf: Pointer to a PCF8591_HandleTypeDef structure.
  * @param read_val: Pointer to variable where the fresh ADC value will be stored.
  * @retval None
  */
void PCF8591_RxCpltCallback(PCF8591_HandleTypeDef *hpcf, uint8_t *read_val);

/**
  * @brief Callback function that must be called from HAL_I2C_MasterTxCpltCallback.
  * @param hpcf: Pointer to a PCF8591_HandleTypeDef structure.
  * @retval None
  */
void PCF8591_TxCpltCallback(PCF8591_HandleTypeDef *hpcf);

#endif /* __PCF8591_H */
