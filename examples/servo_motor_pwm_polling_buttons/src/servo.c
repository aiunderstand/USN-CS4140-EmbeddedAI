/* SPDX-License-Identifier: Apache-2.0 */

#include "servo.h"

#include <zephyr/sys/printk.h>
#include <zephyr/drivers/pwm.h>

/* Reuse the board's existing pwm_led1 node (pwm20 ch0, 20 ms period, P1.10).
 * No custom overlay or binding file required. */
static const struct pwm_dt_spec spec = PWM_DT_SPEC_GET(DT_NODELABEL(pwm_led1));

static uint32_t pulse_width;

int servo_init(void)
{
	if (!pwm_is_ready_dt(&spec)) {
		printk("Error: PWM device %s is not ready\n", spec.dev->name);
		return -ENODEV;
	}

	pulse_width = SERVO_MIN_PULSE;

	return pwm_set_pulse_dt(&spec, pulse_width);
}

void servo_step_left(void)
{
	if (pulse_width > SERVO_MIN_PULSE) {
		pulse_width -= SERVO_STEP;
	}
	pwm_set_pulse_dt(&spec, pulse_width);
	printk("Left  — pulse: %u us\n", pulse_width / 1000U);
}

void servo_step_right(void)
{
	if (pulse_width < SERVO_MAX_PULSE) {
		pulse_width += SERVO_STEP;
	}
	pwm_set_pulse_dt(&spec, pulse_width);
	printk("Right — pulse: %u us\n", pulse_width / 1000U);
}
