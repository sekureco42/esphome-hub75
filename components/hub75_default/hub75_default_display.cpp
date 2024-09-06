#include "esphome.h"
#include "esphome/components/hub75_default/hub75_default_display.h"

// The esphome ESP_LOGx macros expand to reference esp_log_printf_, but do so
// without using its namespace. https://github.com/esphome/issues/issues/3196
// The workaround is to pull that particular function into this namespace.
using esphome::esp_log_printf_;

namespace esphome {
  namespace hub75_default {

    static const char *TAG = "hub75_default";

    void HUB75DefaultDisplay::setup() {
      ESP_LOGCONFIG(TAG, "Starting setup...");
      this->display_name_ = "HUB75d";
      HUB75Display::setup();

      ESP_LOGI(TAG, "Finished Setup");
    }

    void HUB75DefaultDisplay::update() {
      HUB75Display::update();

      if (this->enabled_) {
        if (this->auto_clear_enabled_) {
          this->clear();
        }

        if (this->page_ != nullptr) {
          this->page_->get_writer()(*this);
        } else if (this->writer_.has_value()) {
          (*this->writer_)(*this);
        }
        else {
          update_();
        }
        this->dma_display_->flipDMABuffer();
      }
      else {
        this->dma_display_->clearScreen();
      }

      // remove all not ended clipping regions
      while (is_clipping()) {
        end_clipping();
      }
    }

    void HUB75DefaultDisplay::update_() { 
      HUB75Display::update_();
    }

  }  // namespace hub75_default
}  // namespace esphome
