#include "max7219.h"

static void CS_Low(MAX7219_HandleTypeDef *hmax) {
    HAL_GPIO_WritePin(hmax->cs_port, hmax->cs_pin, GPIO_PIN_RESET);
}

static void CS_High(MAX7219_HandleTypeDef *hmax) {
    HAL_GPIO_WritePin(hmax->cs_port, hmax->cs_pin, GPIO_PIN_SET);
}

HAL_StatusTypeDef MAX7219_Init(MAX7219_HandleTypeDef *hmax, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin) {
    if (hmax == NULL || hspi == NULL || cs_port == NULL) {
        return HAL_ERROR;
    }
    
    hmax->hspi = hspi;
    hmax->cs_port = cs_port;
    hmax->cs_pin = cs_pin;
    hmax->state = MAX7219_STATE_READY;
    hmax->current_line = 0;
    
    // Ensure Chip Select starts high
    CS_High(hmax);
    
    // In a full implementation, we initialize the registers of the MAX7219.
    // For this stub, we return HAL_OK. In actual runtime, we would run:
    // - Display Test: OFF (0x0F -> 0x00)
    // - Scan Limit: All Digits 0-7 (0x0B -> 0x07)
    // - Decode Mode: No decode (0x09 -> 0x00)
    // - Intensity: Medium (0x0A -> 0x07)
    // - Shutdown: Normal Operation (0x0C -> 0x01)
    
    return HAL_OK;
}

HAL_StatusTypeDef MAX7219_WriteReg_IT(MAX7219_HandleTypeDef *hmax, uint8_t reg, uint8_t data) {
    if (hmax == NULL || hmax->hspi == NULL) {
        return HAL_ERROR;
    }
    if (hmax->state != MAX7219_STATE_READY) {
        return HAL_BUSY;
    }
    
    hmax->state = MAX7219_STATE_BUSY_TX;
    hmax->current_line = 0; // Not a multi-line pattern write
    
    hmax->tx_buffer[0] = reg;
    hmax->tx_buffer[1] = data;
    
    // Pull CS Low to select chip
    CS_Low(hmax);
    
    // Start non-blocking SPI transmission
    HAL_StatusTypeDef status = HAL_SPI_Transmit_IT(hmax->hspi, hmax->tx_buffer, 2);
    if (status != HAL_OK) {
        CS_High(hmax);
        hmax->state = MAX7219_STATE_READY;
    }
    
    return status;
}

HAL_StatusTypeDef MAX7219_DisplayPattern_IT(MAX7219_HandleTypeDef *hmax, const uint8_t *pattern) {
    if (hmax == NULL || hmax->hspi == NULL || pattern == NULL) {
        return HAL_ERROR;
    }
    if (hmax->state != MAX7219_STATE_READY) {
        return HAL_BUSY;
    }
    
    hmax->state = MAX7219_STATE_BUSY_TX;
    
    // Prepare frame buffer with (register, data) pairs for all 8 digits (lines)
    for (int i = 0; i < 8; i++) {
        hmax->frame_buffer[i * 2] = MAX7219_REG_DIGIT0 + i; // Register (0x01 to 0x08)
        hmax->frame_buffer[i * 2 + 1] = pattern[i];        // Row bitmap
    }
    
    hmax->current_line = 1; // Mark that we are doing multi-line transmission
    
    // Pull CS Low to start transfer of the first line
    CS_Low(hmax);
    
    // Transmit the first digit-value pair (2 bytes)
    HAL_StatusTypeDef status = HAL_SPI_Transmit_IT(hmax->hspi, &(hmax->frame_buffer[0]), 2);
    if (status != HAL_OK) {
        CS_High(hmax);
        hmax->state = MAX7219_STATE_READY;
    }
    
    return status;
}

void MAX7219_TxCpltCallback(MAX7219_HandleTypeDef *hmax) {
    if (hmax == NULL) {
        return;
    }
    
    // Pull CS High to load the command data into the MAX7219 shift register
    CS_High(hmax);
    
    // If it was a multi-line pattern write, continue with the remaining rows
    if (hmax->current_line > 0 && hmax->current_line < 8) {
        // Wait a small delay or proceed immediately.
        // Pulses on CS must occur between every register write.
        CS_Low(hmax);
        
        // Send next digit pair (index = current_line * 2)
        HAL_StatusTypeDef status = HAL_SPI_Transmit_IT(
            hmax->hspi, 
            &(hmax->frame_buffer[hmax->current_line * 2]), 
            2
        );
        
        if (status == HAL_OK) {
            hmax->current_line++;
        } else {
            CS_High(hmax);
            hmax->state = MAX7219_STATE_READY;
        }
    } else {
        // Single write or all 8 lines completed
        hmax->current_line = 0;
        hmax->state = MAX7219_STATE_READY;
    }
}
