#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include <our_driver/our_driver.h>

static int cmd_channel_get_handler(const struct shell* sh, int arg, char** argv)
{
    const struct device* dev = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    if (!dev){
        shell_error(sh, "Couldn't find device %s", argv[1]);
        return -EFAULT;
    }
    struct sensor_value val;
    int ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &val);
    if (ret){
        shell_error(sh, "Couldn't get channel, got this %d", ret);
        return -EFAULT;
    }

    struct our_driver_data *driver_data = dev->data;

    shell_info(sh, "%d", driver_data->counter);
    return 0;
}

static int cmd_sample_fetch_handler(const struct shell* sh, int arg, char** argv)
{
    const struct device* dev = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    if (!dev){
        shell_error(sh, "Couldn't find device %s", argv[1]);
        return -EFAULT;
    }
    int ret = sensor_sample_fetch(dev);
    if (ret){
        shell_error(sh, "Couldn't get channel, got this %d", ret);
        return -EFAULT;
    }
    return 0;
}

static int cmd_info_handler(const struct shell* sh, int arg, char** argv)
{
    const struct device* dev = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    if (!device_is_ready(dev)){
        shell_error(sh, "Couldn't find device %s", argv[1]);
        return -EFAULT;
    }
    shell_info(sh, "Device our_driver0 Initialized");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    our_driver_subcmd,
    SHELL_CMD_ARG(read, NULL, "Get channel for device", cmd_channel_get_handler, 1, 0),
    SHELL_CMD_ARG(fetch, NULL, "Fetch data", cmd_sample_fetch_handler, 1, 0),
    SHELL_CMD_ARG(info, NULL, "About device", cmd_info_handler, 1, 0),
    SHELL_SUBCMD_SET_END,
);

SHELL_CMD_REGISTER(our_driver, &our_driver_subcmd, "our driver subcommands", NULL);