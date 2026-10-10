#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "pas_driver.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    const struct device *dev = DEVICE_DT_GET_ANY(pas_driver);
    struct sensor_value val;
    bool counting = true;

    if (!device_is_ready(dev)) {
        return -ENODEV;
    }

    pas_enable_toggle_counter(dev, true);

    while (1) {
        (void)sensor_sample_fetch(dev); /* LED on */
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
        (void)sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &val); /* LED off */
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        LOG_INF("toggle count: %d (%s)", val.val1, counting ? "counting" : "frozen");

        /* after 10 toggles, stop counting */
        if (counting && val.val1 >= 10) {
            pas_enable_toggle_counter(dev, false);
            counting = false;
        }
    }

    return 0;
}
