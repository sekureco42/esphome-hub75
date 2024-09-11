import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import hub75_base, time
from esphome.const import CONF_WIDTH, CONF_HEIGHT, CONF_DELAY, \
    CONF_ID, CONF_LAMBDA, CONF_PAGES, CONF_BRIGHTNESS, CONF_RGB_ORDER, \
    CONF_PIN_A, CONF_PIN_B, CONF_PIN_C, CONF_PIN_D, \
    CONF_CHIPSET, CONF_MULTIPLEXER, CONF_TIME_ID


AUTO_LOAD = ["hub75_base"]
DEPENDENCIES = ["hub75_base"]

# Additional configurable pins for HUB75 display
CONF_PIN_E  = 'pin_e'
CONF_MIN_BRIGHTNESS = "min_brightness"
CONF_MAX_BRIGHTNESS = "max_brightness"

hub75_default_ns = cg.esphome_ns.namespace('hub75_default')

HUB75DefaultDisplay = hub75_default_ns.class_(
    'HUB75DefaultDisplay', 
    hub75_base.HUB75Display    
)

CONFIG_SCHEMA = hub75_base.HUB75_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(HUB75DefaultDisplay),
        cv.Optional(CONF_BRIGHTNESS, default=200): cv.int_range(min=0, max=255),
        cv.Optional(CONF_MIN_BRIGHTNESS, default=0): cv.int_range(min=0, max=255),
        cv.Optional(CONF_MAX_BRIGHTNESS, default=255): cv.int_range(min=0, max=255),
        cv.Optional(CONF_PIN_E): pins.gpio_output_pin_schema,
        cv.Optional(CONF_RGB_ORDER, default="RGB"): cv.string,
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await hub75_base.setup_hub75_display(var, config)

    cg.add(var.set_brightness(config[CONF_BRIGHTNESS]))
    cg.add(var.set_min_brightness(config[CONF_MIN_BRIGHTNESS]))
    cg.add(var.set_max_brightness(config[CONF_MAX_BRIGHTNESS]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA],
            [(HUB75DefaultDisplay.operator("ref"), "it")],
            return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
