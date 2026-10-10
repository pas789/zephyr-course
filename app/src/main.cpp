#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    const struct device *dev = DEVICE_DT_GET_ANY(pas_driver);
    struct sensor_value val;

    if (!device_is_ready(dev)) {
        return -ENODEV;
    }

    while (1) {
        (void)sensor_sample_fetch(dev); /* LED on */
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
        (void)sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &val); /* LED off */
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }

    return 0;
}
