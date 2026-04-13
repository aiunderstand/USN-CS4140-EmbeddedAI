/* SPDX-License-Identifier: Apache-2.0 */

#include "buttons.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

static const struct gpio_dt_spec btn_left  = GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios);
static const struct gpio_dt_spec btn_right = GPIO_DT_SPEC_GET(DT_ALIAS(sw3), gpios);

static int configure_button(const struct gpio_dt_spec *btn)
{
	if (!gpio_is_ready_dt(btn)) {
		printk("Error: GPIO %s not ready\n", btn->port->name);
		return -ENODEV;
	}

	return gpio_pin_configure_dt(btn, GPIO_INPUT);
}

int buttons_init(void)
{
	int ret;

	ret = configure_button(&btn_left);
	if (ret < 0) {
		return ret;
	}

	return configure_button(&btn_right);
}

bool button_left_pressed(void)
{
	return gpio_pin_get_dt(&btn_left) > 0;
}

bool button_right_pressed(void)
{
	return gpio_pin_get_dt(&btn_right) > 0;
}
