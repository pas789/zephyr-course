#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT pas_driver
#define LED_NODE DT_ALIAS(app_led)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(pas_driver, LOG_LEVEL_INF);

struct pas_data {
    bool led_state;
};

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
        LOG_INF("LED: ON");
    }

    return 0;
}

static int channel_get_my_impl(const struct device *dev,
                            enum sensor_channel chan,
                            struct sensor_value *val) {
    (void)chan;
    (void)val;

    /* turn off the led */
    struct pas_data *data = dev->data;
    if (data->led_state) {
        if (gpio_pin_set_dt(&led, 0) < 0) {
            LOG_ERR("Failed to turn LED off");
            return -EIO;
        }
        data->led_state = false;
        LOG_INF("LED: OFF");
    }

    return 0;
}

static DEVICE_API(sensor, api_pas_assignment) = {
    .sample_fetch = sensor_sample_fetch_my_impl,
    .channel_get = channel_get_my_impl,
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
    LOG_INF("Device Initialized");

    return 0;
}

static struct pas_data pas_data_0 = {
    .led_state = false,
};

DEVICE_DT_INST_DEFINE(0, init, NULL, &pas_data_0, NULL, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY, &api_pas_assignment);
