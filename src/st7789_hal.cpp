#include "st7789_hal.hpp"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

namespace st7789 {

// Define global variable for DMA interrupt handling
static HAL* current_hal_instance = nullptr;

// DMA transfer completion handler
void dma_complete_handler() {
    // Ensure we have a valid instance
    if (current_hal_instance) {
        // Clear interrupt flag
        dma_hw->ints0 = 1u << current_hal_instance->_dma_tx_channel;
        
        // Update status
        current_hal_instance->_dma_busy = false;
    }
}

HAL::HAL() : 
    _initialized(false),
    _dma_tx_channel(-1),
    _dma_buffer(nullptr),
    _dma_buffer_size(0),
    _dma_enabled(false),
    _dma_busy(false) {
}

HAL::~HAL() {
    cleanupDma();
}

bool HAL::init(const Config& config) {
    _config = config;
    
    // Initialize GPIO 
    gpio_init(_config.pin_cs);
    gpio_init(_config.pin_dc);
    gpio_init(_config.pin_reset);
    gpio_init(_config.pin_bl);
    
    // Set direction
    gpio_set_dir(_config.pin_cs, GPIO_OUT);
    gpio_set_dir(_config.pin_dc, GPIO_OUT);
    gpio_set_dir(_config.pin_reset, GPIO_OUT);
    gpio_set_dir(_config.pin_bl, GPIO_OUT);
    
    // Initial state
    gpio_put(_config.pin_cs, 1);     // Not selected
    gpio_put(_config.pin_dc, 1);     // Data mode
    gpio_put(_config.pin_reset, 1);  // Not reset
    gpio_put(_config.pin_bl, 0);     // Backlight off
    
    // Initialize SPI
    spi_init(_config.spi_inst, _config.spi_speed_hz);
    gpio_set_function(_config.pin_sck, GPIO_FUNC_SPI);
    gpio_set_function(_config.pin_din, GPIO_FUNC_SPI);
    
    // Reset display
    reset();
    
    // Initialize DMA (if enabled)
    if (_config.dma.enabled) {
        initDma();
    }
    
    _initialized = true;
    return true;
}

void HAL::initDma() {
    // Save current instance for interrupt use
    current_hal_instance = this;
    
    // Allocate DMA channel
    _dma_tx_channel = dma_claim_unused_channel(true);
    if (_dma_tx_channel < 0) {
        printf("Failed to get DMA channel\n");
        _dma_enabled = false;
        return;
    }
    
    // Allocate DMA buffer
    _dma_buffer_size = _config.dma.buffer_size;
    _dma_buffer = (uint16_t*)malloc(_dma_buffer_size);
    if (!_dma_buffer) {
        printf("Failed to allocate DMA buffer\n");
        dma_channel_unclaim(_dma_tx_channel);
        _dma_tx_channel = -1;
        _dma_enabled = false;
        return;
    }
    
    // Configure DMA
    dma_channel_config dma_config = dma_channel_get_default_config(_dma_tx_channel);
    channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_16);
    channel_config_set_dreq(&dma_config, spi_get_dreq(_config.spi_inst, true));
    
    // Set DMA
    dma_channel_configure(
        _dma_tx_channel,
        &dma_config,
        &spi_get_hw(_config.spi_inst)->dr,  // Write to SPI data register
        NULL,                                // Source address will be set during transfer
        0,                                   // Transfer count will be set during transfer
        false                                // Do not start immediately
    );
    
    // Configure DMA completion interrupt
    dma_channel_set_irq0_enabled(_dma_tx_channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_complete_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    
    _dma_enabled = true;
    printf("DMA initialization successful, channel: %d\n", _dma_tx_channel);
}

void HAL::cleanupDma() {
    if (_dma_tx_channel >= 0) {
        // Stop any ongoing transfer
        dma_channel_abort(_dma_tx_channel);
        
        // Disable interrupt
        dma_channel_set_irq0_enabled(_dma_tx_channel, false);
        
        // Release channel
        dma_channel_unclaim(_dma_tx_channel);
        _dma_tx_channel = -1;
    }
    
    // Release buffer
    if (_dma_buffer) {
        free(_dma_buffer);
        _dma_buffer = nullptr;
    }
    
    _dma_enabled = false;
    _dma_busy = false;
}

void HAL::writeCommand(uint8_t cmd) {
    gpio_put(_config.pin_cs, 0);  // Selected chip
    gpio_put(_config.pin_dc, 0);  // Command mode
    spi_write_blocking(_config.spi_inst, &cmd, 1);
    gpio_put(_config.pin_cs, 1);  // Unselected
}

void HAL::writeData(uint8_t data) {
    gpio_put(_config.pin_cs, 0);  // Selected chip
    gpio_put(_config.pin_dc, 1);  // Data mode
    spi_write_blocking(_config.spi_inst, &data, 1);
    gpio_put(_config.pin_cs, 1);  // Unselected
}

void HAL::writeDataBulk(const uint8_t* data, size_t len) {
    if (len == 0) return;
    
    gpio_put(_config.pin_cs, 0);  // Selected chip
    gpio_put(_config.pin_dc, 1);  // Data mode
    spi_write_blocking(_config.spi_inst, data, len);
    gpio_put(_config.pin_cs, 1);  // Unselected
}

bool HAL::writeDataDma(const uint16_t* data, size_t len) {
    if (!_dma_enabled || !_dma_buffer || _dma_tx_channel < 0) {
        // If DMA is not available, fall back to normal method
        writeDataBulk((const uint8_t*)data, len * 2);
        return false;
    }
    
    // If DMA is busy, wait for completion
    if (_dma_busy) {
        if (!waitForDmaComplete()) {
            printf("DMA timeout, abort operation\n");
            abortDma();
            return false;
        }
    }
    
    // Set data/command pin to data mode
    gpio_put(_config.pin_dc, 1);
    // Set chip select pin
    gpio_put(_config.pin_cs, 0);
    
    // Handle large transfers
    size_t remaining = len;
    const uint16_t* src_ptr = data;
    
    while (remaining > 0) {
        // Determine transfer size
        size_t transfer_size = (remaining > _dma_buffer_size/2) ? _dma_buffer_size/2 : remaining;
        
        // Prepare data (swap bytes for RGB565 format)
        for (size_t i = 0; i < transfer_size; i++) {
            uint16_t color = src_ptr[i];
            // Swap bytes for RGB565 format
            _dma_buffer[i] = ((color & 0xFF) << 8) | (color >> 8);
        }
        
        // Mark DMA busy
        _dma_busy = true;
        
        // Configure and start DMA transfer
        dma_channel_set_read_addr(_dma_tx_channel, _dma_buffer, false);
        dma_channel_set_trans_count(_dma_tx_channel, transfer_size, true); // Start transfer
        
        // Wait for transfer to complete
        if (!waitForDmaComplete()) {
            printf("DMA transfer timeout\n");
            abortDma();
            gpio_put(_config.pin_cs, 1); // Release chip select
            return false;
        }
        
        // Update pointer and remaining
        src_ptr += transfer_size;
        remaining -= transfer_size;
    }
    
    // Release chip select
    gpio_put(_config.pin_cs, 1);
    return true;
}

bool HAL::waitForDmaComplete(uint32_t timeout_ms) {
    uint32_t start = to_ms_since_boot(get_absolute_time());
    while (_dma_busy) {
        // Check timeout
        if (to_ms_since_boot(get_absolute_time()) - start > timeout_ms) {
            return false;
        }
        // Yield CPU time
        tight_loop_contents();
    }
    return true;
}

void HAL::abortDma() {
    if (_dma_tx_channel >= 0) {
        dma_channel_abort(_dma_tx_channel);
    }
    _dma_busy = false;
}

void HAL::reset() {
    // Reset sequence
    gpio_put(_config.pin_reset, 0);  // Reset state
    delay(20);
    gpio_put(_config.pin_reset, 1);  // Normal state
    delay(120);  // Wait for reset to complete
}

void HAL::setBacklight(bool on) {
    gpio_put(_config.pin_bl, on ? 1 : 0);
}

void HAL::setBrightness(uint8_t brightness) {
    // If hardware supports PWM control backlight, this can be implemented here
    // Simple implementation: turn backlight on/off
    setBacklight(brightness > 0);
}

void HAL::delay(uint32_t ms) {
    sleep_ms(ms);
}

} // namespace st7789 