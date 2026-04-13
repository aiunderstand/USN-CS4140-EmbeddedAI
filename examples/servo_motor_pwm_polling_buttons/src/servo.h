/* SPDX-License-Identifier: Apache-2.0 */

#ifndef SERVO_H_
#define SERVO_H_

#include <zephyr/drivers/pwm.h>

/** Pulse width limits and step size (nanoseconds). */
#define SERVO_MIN_PULSE PWM_USEC(1000)
#define SERVO_MAX_PULSE PWM_USEC(2000)
#define SERVO_STEP      PWM_USEC(100)

/**
 * @brief Initialise the PWM output and drive the servo to the minimum position.
 *
 * @return 0 on success, negative errno on failure.
 */
int servo_init(void);

/**
 * @brief Step the servo one increment toward the minimum position.
 */
void servo_step_left(void);

/**
 * @brief Step the servo one increment toward the maximum position.
 */
void servo_step_right(void);

#endif /* SERVO_H_ */
