/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "servo.h"
#include "buttons.h"

/* Polling interval — also acts as a simple debounce delay. */
#define POLL_MS 150

int main(void)
{
	int ret;

	printk("Servo motor control — Button 2: left, Button 3: right\n");

	ret = servo_init();
	if (ret < 0) {
		printk("Error %d: servo init failed\n", ret);
		return 0;
	}

	ret = buttons_init();
	if (ret < 0) {
		printk("Error %d: buttons init failed\n", ret);
		return 0;
	}

	printk("Ready — initial pulse: %u us\n", SERVO_MIN_PULSE / 1000U);

	while (1) {
		if (button_left_pressed()) {
			servo_step_left();
		} else if (button_right_pressed()) {
			servo_step_right();
		}
		k_msleep(POLL_MS);
	}

	return 0;
}
