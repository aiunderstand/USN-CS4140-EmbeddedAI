/*
 * OV7670 + FIFO Camera Demo — nRF54L15 DK (Zephyr)
 *
 * Captures QQVGA (160×120) RGB565 frames once per second.
 * Set DUMP_FRAME_UART to 1 to hex-dump each frame over UART.
 * See README.md for hardware wiring and Board Configurator requirements.
 * See src/fifo.c for the full pin mapping.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ov7670.h"
#include "fifo.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

/* Set to 1 to hex-dump every captured frame over UART */
#define DUMP_FRAME_UART  1

static uint8_t frame_buf[IMG_SIZE] __aligned(4);

int main(void)
{
	printk("\n*** OV7670 FIFO Camera Demo ***\n");

	if (ov7670_init() != 0) {
		return -EIO;
	}

	if (fifo_init() != 0) {
		return -EIO;
	}

	k_msleep(300);   /* auto-exposure settle */

	LOG_INF("Ready — capturing QQVGA %dx%d RGB565 (%u bytes/frame)",
		IMG_W, IMG_H, (unsigned)IMG_SIZE);

	for (uint32_t n = 1; ; n++) {
		fifo_capture(frame_buf, IMG_SIZE);
		LOG_INF("Frame #%u captured (%u bytes @ %p)",
			n, (unsigned)IMG_SIZE, (void *)frame_buf);

#if DUMP_FRAME_UART
		/* Hex dump — pipe the console to a file on the host. */
		printk("\r\n---FRAME_START---\r\n");
		for (size_t i = 0; i < IMG_SIZE; i++) {
			printk("%02x", frame_buf[i]);
			if ((i & 31) == 31) {
				printk("\r\n");
			}
		}
		printk("\r\n---FRAME_END---\r\n");
#endif

		k_msleep(1000);
	}

	return 0;
}
