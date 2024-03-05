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

hub75_default_ns = cg.esphome_ns.namespace('hub75_default')

HUB75DefaultDisplay = hub75_default_ns.class_(
    'HUB75DefaultDisplay', 
    hub75_base.HUB75Display    
)

CONFIG_SCHEMA = hub75_base.HUB75_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(HUB75DefaultDisplay),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await hub75_base.setup_hub75_display(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA],
            [(HUB75DefaultDisplay.operator("ref"), "it")],
            return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
