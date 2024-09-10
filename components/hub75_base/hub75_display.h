#pragma once

#include "esphome.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/version.h"
#include "esphome/components/display/display_buffer.h"

// The esphome ESP_LOGx macros expand to reference esp_log_printf_, but do so
// without using its namespace. https://github.com/esphome/issues/issues/3196
// The workaround is to pull that particular function into this namespace.
using esphome::esp_log_printf_;
// This is needed for the underlaying library "ESP32 HUB75 LED MATRIX PANEL DMA Display"

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

namespace esphome {
namespace hub75_base {

static const Color COLOR_RED            = Color(255, 0, 0);
static const Color COLOR_RED_LIGHT      = Color(130, 0, 0);
static const Color COLOR_RED_LIGHTER    = Color(30, 0, 0);
static const Color COLOR_GREEN          = Color(0, 255, 0);
static const Color COLOR_GREEN_LIGHT    = Color(0, 130, 0);
static const Color COLOR_GREEN_LIGHTER  = Color(0, 30, 0);
static const Color COLOR_BLUE           = Color(0, 0, 255);
static const Color COLOR_BLUE_LIGHT     = Color(32, 64, 160);
static const Color COLOR_BLUE_LIGHTER   = Color(16, 32, 64);
static const Color COLOR_YELLOW         = Color(255, 255, 0);
static const Color COLOR_ORANGE         = Color(255, 165, 0);
static const Color COLOR_CYAN           = Color(0, 255, 255);
static const Color COLOR_MAGENTA        = Color(255, 0, 255);
static const Color COLOR_GRAY           = Color(102, 102, 102);
static const Color COLOR_WHITE          = Color::WHITE;
static const Color COLOR_BLACK          = Color::BLACK;
const Color backgroundColor             = COLOR_BLACK;

class HUB75Display;

#if ESPHOME_VERSION_CODE >= VERSION_CODE(2023, 12, 0)
class HUB75Display : public display::Display {
#else
class HUB75Display : public PollingComponent, public display::DisplayBuffer {
#endif  // VERSION_CODE(2023, 12, 0)
  public:
    virtual void setup();
    void dump_config() override;
    virtual void update();

    void set_panel_height(int panel_height) { this->height_ = panel_height; }
    void set_panel_width(int panel_width) { this->width_ = panel_width; }
    void set_chain_length(int chain_length) { this->chain_length_ = chain_length; }

    void set_pins(InternalGPIOPin *R1_pin, InternalGPIOPin *G1_pin, InternalGPIOPin *B1_pin, InternalGPIOPin *R2_pin, InternalGPIOPin *G2_pin, InternalGPIOPin *B2_pin, InternalGPIOPin *A_pin, InternalGPIOPin *B_pin, InternalGPIOPin *C_pin, InternalGPIOPin *D_pin, InternalGPIOPin *E_pin, InternalGPIOPin *LAT_pin, InternalGPIOPin *OE_pin, InternalGPIOPin *CLK_pin) {
      // Set the e pin to -1 as required by the library if it is not used
      int8_t e = -1;
      if (E_pin != NULL)
        e = E_pin->get_pin();

      this->pins_ = {
          static_cast<int8_t>(R1_pin->get_pin()),
          static_cast<int8_t>(G1_pin->get_pin()),
          static_cast<int8_t>(B1_pin->get_pin()),
          static_cast<int8_t>(R2_pin->get_pin()),
          static_cast<int8_t>(G2_pin->get_pin()),
          static_cast<int8_t>(B2_pin->get_pin()),
          static_cast<int8_t>(A_pin->get_pin()),
          static_cast<int8_t>(B_pin->get_pin()),
          static_cast<int8_t>(C_pin->get_pin()),
          static_cast<int8_t>(D_pin->get_pin()),
          e,
          static_cast<int8_t>(LAT_pin->get_pin()),
          static_cast<int8_t>(OE_pin->get_pin()),
          static_cast<int8_t>(CLK_pin->get_pin())};
    }

    void set_driver(HUB75_I2S_CFG::shift_driver driver) {
      this->user_defined_driver_ = true;
      this->driver_ = driver;
    };

    void set_i2sspeed(HUB75_I2S_CFG::clk_speed speed) {
      this->user_defined_i2sspeed_ = true;
      this->i2sspeed_ = speed;
    };

    void set_latch_blanking(int latch_blanking) { this->latch_blanking_ = latch_blanking; };

    void set_clock_phase(bool clock_phase) {
      this->user_defined_clock_phase_ = true;
      this->clock_phase_ = clock_phase;
    }

    display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }

    // START: override methods from base class Display to use native performant functions of HUB75 DMA display
    void fill(Color color) override;
    void clear() { this->dma_display_->clearScreen(); };
    void filled_rectangle(int x1, int y1, int width, int height, Color color = display::COLOR_ON);
    void draw_pixel_at(int x, int y, Color color) override;
    // END: override methods from base class Display to use native performant functions of HUB75 DMA display

    void set_state(bool state) { this->enabled_ = state; };
    void set_brightness(uint8_t brightness);
    void set_brightness(uint8_t brightness, bool with_fade);
    void set_min_brightness(uint8_t min_brightness) { this->min_brightness_ = min_brightness; }
    void set_max_brightness(uint8_t max_brightness) { this->max_brightness_ = max_brightness; }

    MatrixPanel_I2S_DMA* get_display() { return this->dma_display_; }

    void set_time(time::RealTimeClock *time);
    time::RealTimeClock* get_time() { return this->time_; };

    // Home Assistant Service Call Definition
    void on_set_brightness(int brightness);

  protected:
    std::string display_name_ = "HUB75";
    MatrixPanel_I2S_DMA *dma_display_{nullptr};
    HUB75_I2S_CFG::i2s_pins pins_;

    bool user_defined_driver_{false};
    HUB75_I2S_CFG::shift_driver driver_;

    bool user_defined_i2sspeed_{false};
    HUB75_I2S_CFG::clk_speed i2sspeed_;

    int8_t latch_blanking_ = -1;
    bool user_defined_clock_phase_{false};
    bool clock_phase_{false};

    uint8_t chain_length_{1};
    uint8_t brightness_{0};
    uint8_t min_brightness_{0};
    uint8_t max_brightness_{255};
    uint8_t brightness_destination_{0};
    unsigned long _lastTime = millis();
    uint8_t brightness_fade_speed_{50}; // Fixed delay of 20ms

    bool enabled_{true};
    uint16_t width_{64};
    uint16_t height_{32};
    bool double_buffer_enabled_{true};

    int get_width_internal() override { return width_; };
    int get_height_internal() override { return height_; };
    virtual void update_();
    virtual void start_screen_();
    void update_brightness_(unsigned long timeInMillis);

    time::RealTimeClock *time_;

    uint16_t frameCounter_ = 0;
    uint32_t frameTime_ = millis();
};

}  // namespace hub75_base
}  // namespace esphome
