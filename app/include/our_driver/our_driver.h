#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DT_DRV_COMPAT our_driver
#define DRIVER_LED_ALIAS DT_ALIAS(app_led)





struct our_driver_config {
    struct gpio_dt_spec led_gpio;
};


struct our_driver_data {
    bool state;
    int counter;
};

struct our_driver_api {
    struct sensor_driver_api sensor_api;
    void (*calibrate)(const struct device *dev);
};

void our_driver_calibrate(const struct device *dev);

#ifdef __cplusplus
}
#endif

