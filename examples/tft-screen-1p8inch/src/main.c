#include <zephyr/kernel.h>

#include "tft_display.h"
#include "usnlogo.h"

int main(void)
{
	const struct device *display = TFT_DEVICE();

	if (tft_init(display) != 0) {
		printk("Display not ready\n");
		return -1;
	}

	tft_fill_screen(display, TFT_COLOR_BLACK);
	tft_draw_image(display, 0, 0, USNLOGO_WIDTH, USNLOGO_HEIGHT,
		       usnlogo_rgb565);

	/* Example bounding box overlaid on the logo */
	tft_draw_bounding_box(display, 40, 10, 80, 55, "USN 0.97");

	printk("Display ready\n");
	return 0;
}
