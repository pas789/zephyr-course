#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>

static int cmd_sample_fetch_handler(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = shell_device_get_binding(argv[1]);

    if (!dev) {
        shell_error(sh, "Could not find device %s", argv[1]);
        return -EFAULT;
    }

    int ret = sensor_sample_fetch(dev);

    if (ret != 0) {
        shell_error(sh, "Could not fetch sample, %d", ret);
        return -EFAULT;
    }

    return 0;
}

static int cmd_channel_get_handler(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = shell_device_get_binding(argv[1]);

    if (!dev) {
        shell_error(sh, "Could not find device %s", argv[1]);
        return -EFAULT;
    }

    struct sensor_value val;
    int ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &val);

    if (ret != 0) {
        shell_error(sh, "Could not get channel, %d", ret);
        return -EFAULT;
    }

    shell_info(sh, "%d", val.val1);

    return 0;
}

static int cmd_info_handler(const struct shell *sh, size_t argc, char **argv)
{
    const char *name = "pas_driver0";
    const struct device *dev = shell_device_get_binding(name);

    if (dev == NULL) {
        shell_error(sh, "Device %s not found!", name);
        return -ENODEV;
    }

    if (!device_is_ready(dev)) {
        shell_error(sh, "%s is not ready!", name);
        return -ENODEV;
    }

    shell_info(sh, "%s is ready!", name);

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(pas_driver_subcmd,
                               [0]=SHELL_CMD_ARG(fetch, NULL, "Calls sensor_sample_fetch().", cmd_sample_fetch_handler, 2, 0),
                               [1]=SHELL_CMD_ARG(read, NULL, "Calls sensor_channel_get().", cmd_channel_get_handler, 2, 0),
                               [2]=SHELL_CMD_ARG(info, NULL, "Prints the device name and ready state.", cmd_info_handler, 1, 0),
                               [3]=SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(sensor, &pas_driver_subcmd, "pas driver set of commands", NULL);
