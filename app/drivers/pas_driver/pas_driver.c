#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "pas_driver.h"

#define DT_DRV_COMPAT pas_driver
#define LED_NODE DT_ALIAS(app_led)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(pas_driver, LOG_LEVEL_INF);

struct pas_data {
    bool led_state;
    bool counter_enabled;
    uint32_t toggle_count;
};

static void pas_count_transition(struct pas_data *data)
{
    if (data->counter_enabled) {
        data->toggle_count++;
    }
}

static int sensor_sample_fetch_my_impl(const struct device *dev,
                                    enum sensor_channel chan) {

    (void)chan;

    /* turn on the led */
    struct pas_data *data = dev->data;
    if (!data->led_state) {
        if (gpio_pin_set_dt(&led, 1) < 0) {
            LOG_ERR("Failed to turn LED on");
            return -EIO;
        }
        data->led_state = true;
        pas_count_transition(data);
        LOG_INF("LED: ON");
    }

    return 0;
}

static int channel_get_my_impl(const struct device *dev,
                            enum sensor_channel chan,
                            struct sensor_value *val) {
    (void)chan;

    /* turn off the led */
    struct pas_data *data = dev->data;
    if (data->led_state) {
        if (gpio_pin_set_dt(&led, 0) < 0) {
            LOG_ERR("Failed to turn LED off");
            return -EIO;
        }
        data->led_state = false;
        pas_count_transition(data);
        LOG_INF("LED: OFF");
    }

    val->val1 = (int32_t)data->toggle_count;
    val->val2 = 0;

    return 0;
}

static int pas_enable_toggle_counter_impl(const struct device *dev, bool enable)
{
    struct pas_data *data = dev->data;

    data->counter_enabled = enable;
    LOG_INF("toggle counter %s", enable ? "enabled" : "disabled");

    return 0;
}

static const struct pas_driver_api api_pas_assignment = {
    .sensor_api = {
        .sample_fetch = sensor_sample_fetch_my_impl,
        .channel_get = channel_get_my_impl,
    },
    .enable_toggle_counter = pas_enable_toggle_counter_impl,
};

static int init(const struct device *dev) {

    struct pas_data *data = dev->data;

    if (!gpio_is_ready_dt(&led))
    {
        LOG_ERR("GPIO not ready");
        return -ENODEV;
    }

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0)
    {
        LOG_ERR("Failed to configure LED pin");
        return -EIO;
    }

    data->led_state = false;
    data->counter_enabled = false;
    data->toggle_count = 0;
    LOG_INF("Device Initialized");

    return 0;
}

static struct pas_data pas_data_0 = {
    .led_state = false,
    .counter_enabled = false,
    .toggle_count = 0,
};

DEVICE_DT_INST_DEFINE(0, init, NULL, &pas_data_0, NULL, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY, &api_pas_assignment);
