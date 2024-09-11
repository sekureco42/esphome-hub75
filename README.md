# esphome-hub75

## Introduction
This project provides the necessary `ESPHome` (https://esphome.io/) component to use the fabolous Trinity ESP32 board from Brian Lough. It is used to drive so called HUB75 LED Matrix panels.

The board provides beside of a direct interface for HUB75 panels two touch pads and an light sensor (LDR). For assembly and connection options of the board visit the corresponding page form Brian Lough.

- ESP32 Trinity Project from Brian Lough: https://github.com/witnessmenow/ESP32-Trinity
- You want to buy an ESP32 Trinity board: https://www.makerfabs.com/esp32-trinity.html
- Looking for HUB75 panel for your Trinity board: https://www.makerfabs.com/64x32-rgb-led-matrix-3mm-pitch.html

Important: Due this component is for ESPHome which is tightly integrated into Home Assistant an existing Home Assistant installation is essential.

Remark: Even you do not have a Trinity board you can use this component; just make sure that you connect your HUB75 panel correctly to your ESP32 board.

## Installation
### ESPHome Environment
To start with ESPHome you need a working environment. With following steps you get a working environment:
* `python3 -m venv venv`
* `source venv/bin/activate`
* `pip3 install esphome`

Please note that minimum required version of ESPHome is `2023.12.0` (the original component was written for older versions of ESPHome - but later not tested any more)!

### ESPHome Component
This section describes the installation of this component. As a pre-requisite Home Assistant and ESPHome must be already installed. Consult corresponding projects for further information how to install those products and make sure that they are working correctly in your environment before you continue.

* `git clone https://github.com/sekureco42/esphome-hub75`
* Rename `secrets.yaml.example` to `secrets.yaml`
* Adjust values in `secrets.yaml` and adapt it to your Home Assistant installation
* Connect your Trinity to your PC (USB-C cable)
* Compile, upload and run the custom component with `esphome run hub75default_minimal.yaml`
* Add your newly created ESPHome device to Home Assistant (should be auto-discovered, see https://www.home-assistant.io/integrations/esphome/)

If successfull you should get a simple display of a clock. A proper time will be only shown if a succesful connection to Home Assistant can be established (Be patient; takes some time - typical up to 2 minutes).

## Available Configurations
In the repository are different examples available. Those are described in this section.

### `hub75default_minimal.yaml`
This file shows the bare minimum to get up and running your display with Home Assistant. Before trying out other configurations or playing around with the possibilites use this configuration file to check if your complete chain is working properly and that you integrated it properly into Home Assistant.

### `hub75default_lambda.yaml`
With this configuration you will see how to create own content on the display. Due setting `auto_clear_enabled: false` the initial content of the screen is not cleared.

The given lambda function will print the time with seconds. It is given as a simple example for custom content.

### `hub75default_pages.yaml`
This configuration file shows the following features:
- Use of custom fonts for the display
- Use of different colors for printing text on the display
- Use custom lambda to draw content on the display
- Use's different pages to show content
- Uses timer to switch between pages
- Use of the Trinity onboard touch buttons (T8: `t8_touchpad`, T9: `t9_touchpad`). You could switch through pages of the display

### Available config keys
The component supports following config keys in the yaml file (all shown values are default values, keys from base class display not shown):
```
display:
  platform: hub75_default
  id: my_trinity_display
  width: 64
  height: 32
  chain_length: 1
  brightness: 100
  min_brightness: 0
  max_brightness: 255
  update_interval: 16ms
  pin_r1: 25
  pin_g1: 26
  pin_b1: 27
  pin_r2: 14
  pin_g2: 12
  pin_b2: 13
  pin_a: 23
  pin_b: 19
  pin_c: 5
  pin_d: 17
  pin_e: -1
  pin_latch: 4
  pin_oe: 15
  pin_clk: 16
  rgb_order: "RGB"
  chipset: "SHIFTREG"
  i2sspeed: "HZ_8M"
  latch_blanking: 1
  clock_phase: False
```

## Remarks
### Maximal refresh rate
With the config file `hub75default_minimal.yaml` you can reach up to `141` frames per second (by setting `update_interval: 5ms` in the display section). This is possible thanks to the new `ESP32 HUB75 LED MATRIX PANEL DMA Display` library version `3.x`. The old one (before `2.0.7` was used) reached only `107` frames per second.

Eventhough you could achieve such a high framerate you should only "use" the rate you need. By default the value is `16ms` (leads to `63` frames per second).

### Panel Color Order
Typical HUB75 panel are in `RGB` (default setting) or `RBG` order. To simplify setup of such panel instead of assigning the proper pins you could also try to set the following key:
```
rgb_order: "RBG"
```
This will switch internally the pin assignments accordingly.

## Additional Ideas
This section contains Ideas and links to appropriate websites with projects which could be also interesting for you in the context of this display.

- The Trinity board provides two touch buttons. For ESPHome integration see https://github.com/drudge/esphome-smartmatrix/blob/main/smartmatrix_trinity.yaml
- To display weather data is nice to have; an example how to fetch this data from Home Assistant is available here: https://esphome.io/cookbook/display_time_temp_oled.html
- The famous Morphing Clock code is also a nice addition to the display. An example implementation can be found here: https://github.com/bogd/esp32-morphing-clock/blob/main/code/src/digit.cpp
- Generic matrix display framework (especially for the Ulanzi TC001): https://github.com/Blueforcer/awtrix-light
- Optional logging (so be able to use latest and greatest ESP32-HUB75-MatrixPanel-DMA library):  https://github.com/TillFleisch/ESP32-HUB75-MatrixPanel-DMA#optional_logging
- Efficient weather state mapping: https://github.com/tejasitraj/esphome-tiny-display/blob/main/display-trial.yaml
- Matrix Fonts: https://github.com/trip5/Matrix-Fonts

## Acknowledges
- Fabulous ESP32 HUB75 LED MATRIX PANEL DMA Display library: https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA
- ESP32 Trinity Project from Brian Lough: https://github.com/witnessmenow/ESP32-Trinity
- Fonts in the fonts folder: those are included for convienience; credits go to:
  - https://github.com/apparentlymart/ledpixels/blob/master/tom-thumb.bdf
  - https://github.com/cmvnd/fonts
  - https://github.com/lubeda/EspHoMaTriXv2
- The nice online editor for pixel art (like weather and coffee cup animation): https://www.piskelapp.com
- Online converter for images to C++ code: https://javl.github.io/image2cpp/ (Hint: I was using following settings: Background color: Black, Invert image colors: Checked, Brightness / alpha threshold: 100, Draw mode: Horizontal - 2 bytes per pixel (565)).
- Contributors (no specific order)
  - disruptivepatternmaterial (https://github.com/disruptivepatternmaterial)
