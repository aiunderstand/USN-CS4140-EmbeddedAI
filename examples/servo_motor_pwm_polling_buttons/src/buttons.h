/* SPDX-License-Identifier: Apache-2.0 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <stdbool.h>

/**
 * @brief Initialise Button 2 (sw2, left) and Button 3 (sw3, right) as inputs.
 *
 * @return 0 on success, negative errno on failure.
 */
int buttons_init(void);

/** @brief Return true if Button 2 (left) is currently pressed. */
bool button_left_pressed(void);

/** @brief Return true if Button 3 (right) is currently pressed. */
bool button_right_pressed(void);

#endif /* BUTTONS_H_ */
