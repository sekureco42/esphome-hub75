#include "esphome.h"
#include "hub75_display.h"
#include <Fonts/TomThumb.h>

namespace esphome
{
  namespace hub75_base
  {

    static const char *const TAG = "hub75_base";

    void HUB75Display::setup() {
      ESP_LOGCONFIG(TAG, "Setting up HUB75Display...");

      // Module configuration
      HUB75_I2S_CFG mxconfig(
          this->width_,
          this->height_,
          this->chain_length_,
          this->pins_);

      if (this->user_defined_driver_)
        mxconfig.driver = this->driver_;

      if (this->user_defined_i2sspeed_)
        mxconfig.i2sspeed = this->i2sspeed_;

      if (this->latch_blanking_ >= 0)
        mxconfig.latch_blanking = this->latch_blanking_;

      if (this->user_defined_clock_phase_)
        mxconfig.clkphase = this->clock_phase_;

      // The min refresh rate correlates with the update frequency of the component
      mxconfig.min_refresh_rate = 1000 / this->update_interval_;

      //TODO: How to use double buffering properly?
      mxconfig.double_buff = this->double_buffer_enabled_;

      // Display Setup
      this->dma_display_ = new MatrixPanel_I2S_DMA(mxconfig);
      this->dma_display_->begin();
      uint8_t brightness = this->brightness_;
      this->set_brightness(0, false);
      this->set_brightness(brightness, true);
      this->dma_display_->clearScreen();

      // Now write some content to the display
      this->start_screen_();

      if (mxconfig.double_buff) {
        // Write same stuff to other buffer
        this->dma_display_->flipDMABuffer();
        this->start_screen_();
      }
    }

    void HUB75Display::on_set_brightness(int brightness) {
      this->set_brightness(brightness, true);
    }

    void HUB75Display::update() {
      unsigned long timeMillis = millis();
      this->update_brightness_(timeMillis);
    }

    void HUB75Display::dump_config() {
      ESP_LOGCONFIG(TAG, "HUB75Display:");

      ESP_LOGCONFIG(TAG, "  Display Width:    %u", this->width_);
      ESP_LOGCONFIG(TAG, "  Display Height:   %u", this->height_);
      ESP_LOGCONFIG(TAG, "  Brightness:       %u", this->brightness_);
      ESP_LOGCONFIG(TAG, "  Brightness (min): %d", this->min_brightness_);
      ESP_LOGCONFIG(TAG, "  Brightness (max): %d", this->max_brightness_);

      // Log pin settings
      ESP_LOGCONFIG(TAG, "  Pins: R1:%i, G1:%i, B1:%i, R2:%i, G2:%i, B2:%i", pins_.r1, pins_.g1, pins_.b1, pins_.r2, pins_.g2, pins_.b2);
      ESP_LOGCONFIG(TAG, "  Pins: A:%i, B:%i, C:%i, D:%i, E:%i", pins_.a, pins_.b, pins_.c, pins_.d, pins_.e);
      ESP_LOGCONFIG(TAG, "  Pins: LAT:%i, OE:%i, CLK:%i", pins_.lat, pins_.oe, pins_.b1, pins_.clk);

      LOG_UPDATE_INTERVAL(this);

      // Log driver settings
      switch (dma_display_->getCfg().driver) {
      case HUB75_I2S_CFG::shift_driver::SHIFTREG:
        ESP_LOGCONFIG(TAG, "  Driver: SHIFTREG");
        break;
      case HUB75_I2S_CFG::shift_driver::FM6124:
        ESP_LOGCONFIG(TAG, "  Driver: FM6124");
        break;
      case HUB75_I2S_CFG::shift_driver::FM6126A:
        ESP_LOGCONFIG(TAG, "  Driver: FM6126A");
        break;
      case HUB75_I2S_CFG::shift_driver::ICN2038S:
        ESP_LOGCONFIG(TAG, "  Driver: ICN2038S");
        break;
      case HUB75_I2S_CFG::shift_driver::MBI5124:
        ESP_LOGCONFIG(TAG, "  Driver: MBI5124");
        break;
      case HUB75_I2S_CFG::shift_driver::SM5266P:
        ESP_LOGCONFIG(TAG, "  Driver: SM5266P");
        break;
      }

      switch (dma_display_->getCfg().i2sspeed)
      {
      // case HUB75_I2S_CFG::clk_speed::HZ_8M:
      //   ESP_LOGCONFIG(TAG, "  I2SSpeed: HZ_8M (is the same like HZ_10M)");
      //   break;
      // case HUB75_I2S_CFG::clk_speed::HZ_16M:
      //   ESP_LOGCONFIG(TAG, "  I2SSpeed: HZ_16M (is the same like HZ_15M)");
      //   break;
      case HUB75_I2S_CFG::clk_speed::HZ_20M:
        ESP_LOGCONFIG(TAG, "  I2SSpeed: HZ_20M");
        break;
      }

      ESP_LOGCONFIG(TAG, "  Latch blanking: %i", dma_display_->getCfg().latch_blanking);

      ESP_LOGCONFIG(TAG, "  Clock Phase: %s", dma_display_->getCfg().clkphase ? "true" : "false");

      ESP_LOGCONFIG(TAG, "  Min refresh rate: %i", dma_display_->getCfg().min_refresh_rate);
    }

    void HUB75Display::set_brightness(uint8_t brightness) {
      this->set_brightness(brightness, false);
    }

    void HUB75Display::set_brightness(uint8_t brightness, bool with_fade) {
      if (brightness <= this->min_brightness_) {
        brightness = this->min_brightness_;
      }

      if (brightness >= this->max_brightness_) {
        brightness = this->max_brightness_;
      }

      this->brightness_destination_ = brightness;
      if (!with_fade) {
        this->brightness_ = brightness;

        if (this->dma_display_)
          this->dma_display_->setBrightness8(this->brightness_);      
      } 
    }

    void HOT HUB75Display::draw_absolute_pixel_internal(int x, int y, Color color) {
      // Reject invalid pixels
      if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0)
        return;

      // Update pixel value in buffer
      this->dma_display_->drawPixelRGB888(x, y, color.r, color.g, color.b);
    }

    void HUB75Display::fill(Color color) {
      // Wrap fill screen method
      dma_display_->fillScreenRGB888(color.r, color.g, color.b);
    }

    void HUB75Display::filled_rectangle(int x1, int y1, int width, int height, Color color) {
      // Wrap fill rectangle method
      dma_display_->fillRect(x1, y1, width, height, color.r, color.g, color.b);
    }

    void HUB75Display::update_() { 
      this->dma_display_->fillRect(0, 8, 64, 7, display::ColorUtil::color_to_565(backgroundColor));
      this->dma_display_->setCursor(0, 8);
      this->dma_display_->setTextColor(display::ColorUtil::color_to_565(COLOR_RED));

      ESPTime now = this->time_->now();
      if (now.is_valid()) {
        this->dma_display_->printf("%02d:%02d:%02d", now.hour, now.minute, now.second);
      }
      else {
        this->dma_display_->print("--:--:-- ?");
      }
    }

    void HUB75Display::start_screen_() {
      this->dma_display_->setFont(&TomThumb);
      this->dma_display_->setCursor(0, 0+5);
      this->dma_display_->setTextColor(display::ColorUtil::color_to_565(hub75_base::COLOR_GREEN_LIGHT));
      this->dma_display_->printf("-> %s Display", this->display_name_.c_str());

      this->dma_display_->setCursor(0, 16+8);
      this->dma_display_->setTextColor(display::ColorUtil::color_to_565(hub75_base::COLOR_BLUE_LIGHTER));
      this->dma_display_->print("(c) 2023 by");
      this->dma_display_->setFont();

      this->dma_display_->setCursor(0, 24);
      this->dma_display_->setTextColor(display::ColorUtil::color_to_565(hub75_base::COLOR_BLUE_LIGHT));
      this->dma_display_->print("sekureco42");
    }

    void HUB75Display::update_brightness_(unsigned long timeInMillis) {
      frameCounter_++;
      if ((timeInMillis - frameTime_) >= 1000) {
        ESP_LOGD(TAG, "%d frames per second!", frameCounter_);

        frameTime_ = timeInMillis;
        frameCounter_ = 0;
      }

      if (timeInMillis - _lastTime >= brightness_fade_speed_) {
        // Do the morph logic if required.
        if (brightness_ != brightness_destination_) {
          // 0 -> 255 256/20
          if (brightness_destination_ > brightness_) {
            brightness_ = brightness_ + 2;
            if (brightness_ > brightness_destination_) {
              brightness_ = brightness_destination_;
            }
          }
          if (brightness_destination_ < brightness_) {
            brightness_ = brightness_ - 2;
            if (brightness_ < brightness_destination_) {
              brightness_ = brightness_destination_;
            }
          }

          uint8_t brightness_destination_saved = brightness_destination_;
          this->set_brightness(brightness_);
          brightness_destination_ = brightness_destination_saved;
        }

        _lastTime = timeInMillis;
      }
    }

  }  // namespace hub75_base
}  // namespace esphome
