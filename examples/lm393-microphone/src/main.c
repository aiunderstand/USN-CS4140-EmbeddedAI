/*
 * LM393 Microphone ADC reader for nRF54L15 DK
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

/* Get the ADC channel configuration from the devicetree overlay (P1.13) */
static const struct adc_dt_spec mic_channel =
	ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);

int main(void)
{
	int err;
	uint16_t buf;
	/* Where the ADC sample will be stored */
	struct adc_sequence sequence = {
		.buffer = &buf,
		.buffer_size = sizeof(buf),
	};

	if (!adc_is_ready_dt(&mic_channel)) {
		printk("ADC controller not ready\n");
		return 0;
	}

	/* Configure the ADC channel with gain, reference, and resolution from the overlay */
	err = adc_channel_setup_dt(&mic_channel);
	if (err < 0) {
		printk("Could not setup ADC channel (%d)\n", err);
		return 0;
	}

	while (1) {
		/* Prepare the sequence and trigger a single ADC sample */
		adc_sequence_init_dt(&mic_channel, &sequence);
		err = adc_read_dt(&mic_channel, &sequence);
		if (err < 0) {
			printk("ADC read error (%d)\n", err);
		} else {
			int32_t val_mv = (int32_t)buf;
			printk("raw: %d", val_mv);
			/* Convert the raw ADC value to millivolts using the channel config */
			if (adc_raw_to_millivolts_dt(&mic_channel, &val_mv) == 0) {
				printk(" = %d mV", val_mv);
			}
			printk("\n");
		}

		k_sleep(K_MSEC(50));
	}
	return 0;
}
