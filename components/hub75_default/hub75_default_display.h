#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/hub75_base/hub75_display.h"

namespace esphome {
  namespace hub75_default {

    class HUB75DefaultDisplay;
    using hub75defaultdisplay_writer_t = std::function<void(HUB75DefaultDisplay &)>;

    class HUB75DefaultDisplay : public hub75_base::HUB75Display {
      public:
        void setup() override;
        void update() override;
        void set_writer(const hub75defaultdisplay_writer_t &writer) { this->writer_ = writer; }
      protected:
        optional<hub75defaultdisplay_writer_t> writer_;
        void update_();
    };

  }  // namespace hub75_default
}  // namespace esphome
