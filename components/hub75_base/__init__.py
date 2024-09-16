import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome import pins
from esphome.components import display, light
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
    CONF_HEIGHT,
    CONF_WIDTH,
    CONF_UPDATE_INTERVAL,
    CONF_BRIGHTNESS,
    CONF_PIN_A,
    CONF_PIN_B,
    CONF_PIN_C,
    CONF_PIN_D,
    CONF_CHIPSET,
    CONF_MIN_BRIGHTNESS,
    CONF_MAX_BRIGHTNESS,  
    CONF_RGB_ORDER,  
)
from esphome.const import __version__ as ESPHOME_VERSION

CODEOWNERS = ["@sekureco42"]

# Additional configurable pins for HUB75 display
CONF_PIN_E  = 'pin_e'
CONF_PIN_OE = 'pin_oe'
CONF_PIN_LAT = 'pin_latch'

CONF_PIN_R1 = 'pin_r1'
CONF_PIN_G1 = 'pin_g1'
CONF_PIN_B1 = 'pin_b1'
CONF_PIN_R2 = 'pin_r2'
CONF_PIN_G2 = 'pin_g2'
CONF_PIN_B2 = 'pin_b2'

CONF_PIN_CLK = 'pin_clk'

# Display specific configurable items
CONF_CHAIN_LENGTH = 'chain_length'
CONF_I2SSPEED = 'i2sspeed'
CONF_LATCH_BLANKING = 'latch_blanking'
CONF_CLOCK_PHASE = 'clock_phase'

hub75_base_ns = cg.esphome_ns.namespace("hub75_base")
HUB75Display = hub75_base_ns.class_(
    "HUB75Display", cg.PollingComponent, display.Display)

shift_driver = cg.global_ns.namespace("HUB75_I2S_CFG").enum("shift_driver")
DRIVERS = {
    "SHIFTREG": shift_driver.SHIFTREG,
    "FM6124": shift_driver.FM6124,
    "FM6126A": shift_driver.FM6126A,
    "ICN2038S": shift_driver.ICN2038S,
    "MBI5124": shift_driver.MBI5124,
    "SM5266": shift_driver.SM5266P
}

clk_speed = cg.global_ns.namespace("HUB75_I2S_CFG").enum("clk_speed")
CLOCK_SPEEDS = {
    "HZ_8M": clk_speed.HZ_8M,
    "HZ_10M": clk_speed.HZ_10M,
    "HZ_15M": clk_speed.HZ_15M,
    "HZ_20M": clk_speed.HZ_20M
}

HUB75_SCHEMA = (
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(HUB75Display),
            cv.Optional(CONF_WIDTH, default=64): cv.positive_int,
            cv.Optional(CONF_HEIGHT, default=32): cv.positive_int,
            cv.Optional(CONF_CHAIN_LENGTH, default=1): cv.positive_int,
            cv.Optional(CONF_BRIGHTNESS, default=100): cv.int_range(min=0, max=255),
            cv.Optional(CONF_MIN_BRIGHTNESS, default=0): cv.uint8_t,
            cv.Optional(CONF_MAX_BRIGHTNESS, default=255): cv.uint8_t,
            cv.Optional(
                CONF_UPDATE_INTERVAL, default="16ms"
            ): cv.positive_time_period_milliseconds,

            cv.Optional(CONF_PIN_R1, default=25): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_G1, default=26): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_B1, default=27): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_R2, default=14): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_G2, default=12): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_B2, default=13): pins.gpio_output_pin_schema,

            cv.Optional(CONF_PIN_A, default=23): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_B, default=19): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_C, default=5): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_D, default=17): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_E, default=-1): pins.gpio_output_pin_schema,

            cv.Optional(CONF_PIN_LAT, default=4): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_OE, default=15): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PIN_CLK, default=16): pins.gpio_output_pin_schema,
            cv.Optional(CONF_RGB_ORDER, default="RGB"): cv.string,

            cv.Optional(CONF_CHIPSET): cv.enum(
                DRIVERS, upper=True, space="_"
            ),

            cv.Optional(CONF_I2SSPEED): cv.enum(
                CLOCK_SPEEDS, upper=True, space="_"
            ),

            cv.Optional(CONF_LATCH_BLANKING): cv.positive_int,
            cv.Optional(CONF_CLOCK_PHASE, default=False): cv.boolean,
        }
    )
)

async def setup_hub75_display(var, config):
    cg.add(var.set_panel_width(config[CONF_WIDTH]))
    cg.add(var.set_panel_height(config[CONF_HEIGHT]))
    cg.add(var.set_chain_length(config[CONF_CHAIN_LENGTH]))
    cg.add(var.set_brightness(config[CONF_BRIGHTNESS]))
    cg.add(var.set_min_brightness(config[CONF_MIN_BRIGHTNESS]))
    cg.add(var.set_max_brightness(config[CONF_MAX_BRIGHTNESS]))

    R1_pin = await cg.gpio_pin_expression(config[CONF_PIN_R1])

    if config[CONF_RGB_ORDER] == 'RGB':
        G1_pin = await cg.gpio_pin_expression(config[CONF_PIN_G1])
        B1_pin = await cg.gpio_pin_expression(config[CONF_PIN_B1])
        G2_pin = await cg.gpio_pin_expression(config[CONF_PIN_G2])
        B2_pin = await cg.gpio_pin_expression(config[CONF_PIN_B2])
    else:
        G1_pin = await cg.gpio_pin_expression(config[CONF_PIN_B1])
        B1_pin = await cg.gpio_pin_expression(config[CONF_PIN_G1])
        G2_pin = await cg.gpio_pin_expression(config[CONF_PIN_B2])
        B2_pin = await cg.gpio_pin_expression(config[CONF_PIN_G2])

    R2_pin = await cg.gpio_pin_expression(config[CONF_PIN_R2])


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
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    cg.add_library("SPI", None)
    cg.add_library("Wire", None)
    cg.add_library("Adafruit BusIO", None)
    cg.add_library("Adafruit GFX Library", None)
    cg.add_library("ESP32 HUB75 LED MATRIX PANEL DMA Display", None)
