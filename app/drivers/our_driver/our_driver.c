#include <zephyr/logging/log.h>
#include <our_driver/our_driver.h>

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);


static const struct our_driver_config my_config = {
    .led_gpio = GPIO_DT_SPEC_INST_GET(0, led_gpios),
};


static struct our_driver_data my_data = {
    .state = true,
    .counter = 0,
};


static int channel_get_my_impl(
    const struct device *dev,
    enum sensor_channel chan,
    struct sensor_value *val)
{
    const struct our_driver_config *cfg = dev->config;
    struct our_driver_data *driver_data = dev->data;

    int ret = gpio_pin_set_dt(&cfg->led_gpio, 0);
    if (ret < 0) {
        return 0;
    }
    driver_data->state = !driver_data->state;
    LOG_INF("LED state: %s\n", driver_data->state ? "ON" : "OFF");
    return 0;
}

static inline void calibrate_my_impl(const struct device *dev)
{
    struct our_driver_data *driver_data = dev->data;
    driver_data->counter += 1;
    printf("Counter now %d\n", driver_data->counter);
}


static int sample_fetch_my_impl(const struct device *dev, enum sensor_channel chan)
{
    const struct our_driver_config *cfg = dev->config;
    struct our_driver_data *driver_data = dev->data;

    int ret = gpio_pin_set_dt(&cfg->led_gpio, 1);
    if (ret < 0) {
        return 0;
    }
    driver_data->state = !driver_data->state;
    LOG_INF("LED state: %s\n", driver_data->state ? "ON" : "OFF");
    return 0;
}


static int init(const struct device *dev)
{
    const struct our_driver_config *cfg = dev->config;
	if (!gpio_is_ready_dt(&cfg->led_gpio)) {
		return -ENODEV;
	}

	int ret = gpio_pin_configure_dt(&cfg->led_gpio, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return -ENODEV;
	}
    LOG_INF("Device initialized");
    return 0;
}


// static DEVICE_API(sensor, my_api) = {
//     .channel_get = channel_get_my_impl,
//     .sample_fetch = sample_fetch_my_impl,
// };

static const struct our_driver_api my_api = {
    .sensor_api = {
        .channel_get = channel_get_my_impl,
        .sample_fetch = sample_fetch_my_impl,
    },
    .calibrate = calibrate_my_impl,
};

void our_driver_calibrate(const struct device *dev)
{
    if (dev != NULL) {
        const struct our_driver_api *api = dev->api;

        if (api != NULL && api->calibrate != NULL) {
            api->calibrate(dev);
        }
    }
}

DEVICE_DT_INST_DEFINE(0, init, NULL, &my_data, &my_config, POST_KERNEL, 80, &my_api);
