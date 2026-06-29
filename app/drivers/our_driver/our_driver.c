#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT our_driver
#define DRIVER_LED_ALIAS DT_ALIAS(app_led)

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

struct our_driver_config {
    struct gpio_dt_spec led_gpio;
};

static const struct our_driver_config my_config = {
    .led_gpio = GPIO_DT_SPEC_INST_GET(0, led_gpios),
};

struct our_driver_data {
    bool data;
};

static struct our_driver_data my_data = {
    .data = true,
};

static int channel_get_my_impl(
    const struct device *dev,
    enum sensor_channel chan,
    struct sensor_value *val)
{
    const struct our_driver_config *cfg = dev->config;
    struct our_driver_data *state = dev->data;

    int ret = gpio_pin_set_dt(&cfg->led_gpio, 0);
    if (ret < 0) {
        return 0;
    }
    state->data = !state->data;
    LOG_INF("LED state: %s\n", state->data ? "ON" : "OFF");
    return 0;
}


static int sample_fetch_my_impl(const struct device *dev, enum sensor_channel chan)
{
    const struct our_driver_config *cfg = dev->config;
    struct our_driver_data *val = dev->data;

    int ret = gpio_pin_set_dt(&cfg->led_gpio, 1);
    if (ret < 0) {
        return 0;
    }
    val->data = !val->data;
    LOG_INF("LED state: %s\n", val->data ? "ON" : "OFF");
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


static DEVICE_API(sensor, my_api) = {
    .channel_get = channel_get_my_impl,
    .sample_fetch = sample_fetch_my_impl,
};


DEVICE_DT_INST_DEFINE(0, init, NULL, &my_data, &my_config, POST_KERNEL, 80, &my_api);
