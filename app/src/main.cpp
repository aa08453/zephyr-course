/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>

/* 1000 msec = 1 sec */
int delay = CONFIG_APP_HEARTBEAT_PERIOD_MS;

/* The devicetree node identifier for the "led0" alias. */
#define APP_LED DT_ALIAS(app_led)
const struct device* driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(APP_LED, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

namespace{
	void test()
	{
		struct sensor_value val;
		int ret = sensor_channel_get(driver, SENSOR_CHAN_AMBIENT_TEMP, &val);
		ret = sensor_sample_fetch(driver);
		// LOG_INF("Channel ret %d", ret);   
	}
}

int main(void)
{
	// int ret;
	// test();


	while (1) {
		test();
		// sensor_sample_fetch(driver);
		// printf("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(delay);
	}
	return 0;
}


