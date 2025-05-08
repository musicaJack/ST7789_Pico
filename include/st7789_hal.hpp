#pragma once

#include <cstdint>
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "st7789_config.hpp"

namespace st7789 {

// Hardware Abstraction Layer class - handles all hardware-related operations
class HAL {
private:
    Config _config;
    bool _initialized;
    
    // DMA related members
    int _dma_tx_channel;
    uint16_t* _dma_buffer;
    size_t _dma_buffer_size;
    bool _dma_enabled;
    bool _dma_busy;
    
    // Private methods
    void initDma();
    void cleanupDma();
    bool waitForDmaComplete(uint32_t timeout_ms = 1000);
    
public:
    HAL();
    virtual ~HAL();
    
    // Initialize hardware
    bool init(const Config& config);
    
    // Basic IO operations
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void writeDataBulk(const uint8_t* data, size_t len);
    
    // DMA operations
    bool writeDataDma(const uint16_t* data, size_t len);
    bool isDmaBusy() const { return _dma_busy; }
    bool isDmaEnabled() const { return _dma_enabled; }
    void abortDma();
    
    // Hardware control
    void reset();
    void setBacklight(bool on);
    void setBrightness(uint8_t brightness); // If hardware supports PWM
    
    // Spinlock and delay
    void delay(uint32_t ms);
    
    // Get configuration
    const Config& getConfig() const { return _config; }
    
    // Rotation and size settings
    void setWidth(uint16_t width) { _config.width = width; }
    void setHeight(uint16_t height) { _config.height = height; }
    void setRotation(Rotation rotation) { _config.rotation = rotation; }
    
    // Friend declaration - allows interrupt handler to access private members
    friend void dma_complete_handler();
};

} // namespace st7789 