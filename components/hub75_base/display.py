async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    cg.add(var.set_panel_width(config[CONF_WIDTH]))
    cg.add(var.set_panel_height(config[CONF_HEIGHT]))
    cg.add(var.set_chain_length(config[CONF_CHAIN_LENGTH]))
    cg.add(var.set_brightness(config[CONF_BRIGHTNESS]))

    R1_pin = await cg.gpio_pin_expression(config[CONF_PIN_R1])
    G1_pin = await cg.gpio_pin_expression(config[CONF_PIN_G1])
    B1_pin = await cg.gpio_pin_expression(config[CONF_PIN_B1])
    R2_pin = await cg.gpio_pin_expression(config[CONF_PIN_R2])
    G2_pin = await cg.gpio_pin_expression(config[CONF_PIN_G2])
    B2_pin = await cg.gpio_pin_expression(config[CONF_PIN_B2])

    A_pin = await cg.gpio_pin_expression(config[CONF_PIN_A])
    B_pin = await cg.gpio_pin_expression(config[CONF_PIN_B])
    C_pin = await cg.gpio_pin_expression(config[CONF_PIN_C])
    D_pin = await cg.gpio_pin_expression(config[CONF_PIN_D])

    if CONF_PIN_E in config:
        E_pin = await cg.gpio_pin_expression(config[CONF_PIN_E])
    else:
        E_pin = 0

    LAT_pin = await cg.gpio_pin_expression(config[CONF_PIN_LAT])
    OE_pin = await cg.gpio_pin_expression(config[CONF_PIN_OE])
    CLK_pin = await cg.gpio_pin_expression(config[CONF_PIN_CLK])

    cg.add(var.set_pins(R1_pin, G1_pin, B1_pin, R2_pin, G2_pin, B2_pin,
           A_pin, B_pin, C_pin, D_pin, E_pin, LAT_pin, OE_pin, CLK_pin))

    if CONF_CHIPSET in config:
        cg.add(var.set_driver(config[CONF_CHIPSET]))

    if CONF_I2SSPEED in config:
        cg.add(var.set_i2sspeed(config[CONF_I2SSPEED]))

    if CONF_LATCH_BLANKING in config:
        cg.add(var.set_latch_blanking(config[CONF_LATCH_BLANKING]))

    if CONF_CLOCK_PHASE in config:
        cg.add(var.set_clock_phase(config[CONF_CLOCK_PHASE]))

    if cv.Version.parse(ESPHOME_VERSION) < cv.Version.parse("2023.12.0"):
        await cg.register_component(var, config)
    await display.register_display(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBufferRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    cg.add_library("SPI", None)
    cg.add_library("Wire", None)
    cg.add_library("Adafruit BusIO", None)
    cg.add_library("Adafruit GFX Library", None)
    #cg.add_library("ESP32 HUB75 LED MATRIX PANEL DMA Display", "2.0.7")
    cg.add_library("ESP32 HUB75 LED MATRIX PANEL DMA Display", None)