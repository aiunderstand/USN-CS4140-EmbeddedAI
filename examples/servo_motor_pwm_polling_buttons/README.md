# servo_motor_pwm_polling_buttons

Zephyr example for the **nRF54L15 DK** that controls a **Tower Pro MG995** servo motor
using PWM output and two on-board push buttons, polled from the main loop.

- **Button 2 (SW2)** — rotates the servo one step toward 0 °
- **Button 3 (SW3)** — rotates the servo one step toward 180 °

Each button press moves the servo by 100 µs (≈ 9 °). The main loop polls at
150 ms intervals, which also acts as a basic debounce delay.

---

## Hardware

### Requirements

| Item | Notes |
|------|-------|
| nRF54L15 DK | Any `nrf54l15dk_nrf54l15_cpuapp` build target |
| Tower Pro MG995 | 4.8 V – 7.2 V servo, 50 Hz PWM control signal |
| External 5 V supply | Required — do **not** power the servo from the DK's 3.3 V rail |
| Common ground wire | Connect DK GND to the external supply GND |

> **Note:** The MG995 draws up to ~500 mA under load. Always use a dedicated 5 V
> supply (e.g. a 5 V BEC or USB power module). The nRF54L15 DK GPIO logic level
> is 3.3 V, which is sufficient to drive the MG995 signal line.

---

### Pin Configuration

#### PWM Output — Servo Signal

| Function | nRF54L15 DK pin | DTS node | PWM peripheral |
|----------|----------------|----------|----------------|
| Servo PWM signal | **P1.10** | `pwm_led1` | pwm20 ch0 |

PWM parameters (from board DTS, no overlay required):

| Parameter | Value |
|-----------|-------|
| Period | 20 ms (50 Hz) |
| Min pulse (0 °) | 1000 µs |
| Max pulse (180 °) | 2000 µs |
| Step size | 100 µs (~9 °) |

#### Input Buttons

| Function | Label | Alias | nRF54L15 DK pin | Logic |
|----------|-------|-------|----------------|-------|
| Step left (toward 0 °) | Push button 2 | `sw2` | **P1.08** | Active-low |
| Step right (toward 180 °) | Push button 3 | `sw3` | **P0.04** | Active-low |

---

### Wiring Diagram

```
nRF54L15 DK          MG995 Servo
───────────          ───────────
P1.10 ─────────────► Signal (orange/yellow)
GND   ──┬──────────► GND    (brown/black)
        │
        └── External 5 V supply GND
External 5 V ──────► VCC    (red)
```

The two buttons (SW2 / SW3) are on-board — no external wiring needed.

---

## Building and Flashing

```bash
west build -b nrf54l15dk/nrf54l15/cpuapp --pristine
west flash
```

---

## Project Structure

```
servo_motor_pwm_polling_buttons/
├── CMakeLists.txt
├── prj.conf
├── boards/
│   └── nrf54l15dk_nrf54l15_cpuapp.overlay   # Empty — no overlay needed
└── src/
    ├── main.c      # Main loop: polls buttons, calls servo steps
    ├── servo.c/.h  # PWM driver wrapper (init, step_left, step_right)
    └── buttons.c/.h# GPIO input wrapper (init, pressed queries)
```

### Kconfig (`prj.conf`)

| Option | Purpose |
|--------|---------|
| `CONFIG_PWM=y` | PWM driver subsystem |
| `CONFIG_GPIO=y` | GPIO driver subsystem |
| `CONFIG_PRINTK=y` | `printk` logging over UART |
| `CONFIG_STDOUT_CONSOLE=y` | Route stdout to serial console |

---

## Serial Output

Connect a terminal at **115200 8N1** to see pulse-width logs:

```
Servo motor control — Button 2: left, Button 3: right
Ready — initial pulse: 1000 us
Left  — pulse: 900 us
Right — pulse: 1000 us
Right — pulse: 1100 us
```
