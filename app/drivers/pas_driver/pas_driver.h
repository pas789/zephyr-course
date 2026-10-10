
#ifndef PAS_DRIVER_H_
#define PAS_DRIVER_H_

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

struct pas_driver_api {
    struct sensor_driver_api sensor_api;
    int (*enable_toggle_counter)(const struct device *dev, bool enable);
};

static inline int pas_enable_toggle_counter(const struct device *dev, bool enable)
{
    const struct pas_driver_api *api =
        (const struct pas_driver_api *)dev->api;

    if (api == NULL || api->enable_toggle_counter == NULL) {
        return -ENOSYS;
    }

    return api->enable_toggle_counter(dev, enable);
}

#endif /* PAS_DRIVER_H_ */