#include "pcf8591.h"

HAL_StatusTypeDef PCF8591_Init(PCF8591_HandleTypeDef *hpcf, I2C_HandleTypeDef *hi2c) {
    if (hpcf == NULL || hi2c == NULL) {
        return HAL_ERROR;
    }
    hpcf->hi2c = hi2c;
    hpcf->state = PCF8591_STATE_READY;
    hpcf->control_reg = 0x00;
    

    return HAL_OK;
}

HAL_StatusTypeDef PCF8591_ReadChannel_IT(PCF8591_HandleTypeDef *hpcf, uint8_t channel) {
    if (hpcf == NULL || hpcf->hi2c == NULL) {
        return HAL_ERROR;
    }
    if (hpcf->state != PCF8591_STATE_READY) {
        return HAL_BUSY;
    }
    
    hpcf->state = PCF8591_STATE_BUSY_RX;
    
    // Prepare the control byte. Preserve the DAC enable bit if active
    hpcf->control_reg = (hpcf->control_reg & PCF8591_CTRL_DAC_ENABLE) | (channel & 0x03);
    
    // First we write the control register to select the channel
    // Then we launch the transmit of the control byte.
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_IT(hpcf->hi2c, PCF8591_I2C_ADDR, &(hpcf->control_reg), 1);
    if (status != HAL_OK) {
        hpcf->state = PCF8591_STATE_READY;
    }
    
    return status;
}

HAL_StatusTypeDef PCF8591_WriteDAC_IT(PCF8591_HandleTypeDef *hpcf, uint8_t value) {
    if (hpcf == NULL || hpcf->hi2c == NULL) {
        return HAL_ERROR;
    }
    if (hpcf->state != PCF8591_STATE_READY) {
        return HAL_BUSY;
    }
    
    hpcf->state = PCF8591_STATE_BUSY_TX;
    
    // Enable DAC bit in control register
    hpcf->control_reg |= PCF8591_CTRL_DAC_ENABLE;
    
    hpcf->tx_buffer[0] = hpcf->control_reg;
    hpcf->tx_buffer[1] = value;
    
    // Send 2 bytes: Control byte (with DAC enable) + DAC Value
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_IT(hpcf->hi2c, PCF8591_I2C_ADDR, hpcf->tx_buffer, 2);
    if (status != HAL_OK) {
        hpcf->state = PCF8591_STATE_READY;
    }
    
    return status;
}

void PCF8591_RxCpltCallback(PCF8591_HandleTypeDef *hpcf, uint8_t *read_val) {
    if (hpcf == NULL || read_val == NULL) {
        return;
    }
    
    // PCF8591 ADC conversion timing: the first byte returned is the conversion result
    // of the PREVIOUS channel/cycle. The second byte is the CURRENT conversion result.
    // Therefore, we parse the buffer and extract the correct value.
    *read_val = hpcf->rx_buffer[1]; 
    
    hpcf->state = PCF8591_STATE_READY;
}

void PCF8591_TxCpltCallback(PCF8591_HandleTypeDef *hpcf) {
    if (hpcf == NULL) {
        return;
    }
    
    // If the Tx completed was the channel selection for a read,
    // we now must trigger the non-blocking read (receive) of the 2 bytes.
    if (hpcf->state == PCF8591_STATE_BUSY_RX) {
        HAL_I2C_Master_Receive_IT(hpcf->hi2c, PCF8591_I2C_ADDR, hpcf->rx_buffer, 2);
    } else {
        hpcf->state = PCF8591_STATE_READY;
    }
}
