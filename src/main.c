/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define SPI_NODE DT_NODELABEL(spi1)
#define CS_PIN 15  // P0.15 for nRF52840DK

// Loopback configuration
#define LOOPBACK_MODE 0  // Set to 1 for loopback, 0 for normal operation

int main(void)
{
	const struct device *spi_dev = DEVICE_DT_GET(SPI_NODE);
	
	if (!device_is_ready(spi_dev)) {
		LOG_ERR("SPI device not ready.");
		return 0;
	}

	// Configure CS pin
	const struct device *cs_gpio = DEVICE_DT_GET(DT_NODELABEL(gpio0));
	if (!device_is_ready(cs_gpio)) {
		LOG_ERR("GPIO device not ready.");
		return 0;
	}

	// Configure CS pin as output and set it high initially
	gpio_pin_configure(cs_gpio, CS_PIN, GPIO_OUTPUT_HIGH);
	LOG_INF("CS pin configured as output");

#if LOOPBACK_MODE
	LOG_INF("=== LOOPBACK MODE ENABLED ===");
	LOG_INF("Connect MOSI (P0.13) to MISO (P0.14) for loopback testing");
	LOG_INF("CS pin will be ignored in loopback mode");
#endif

	uint8_t tx_buffer[] = "Hello from nRF";
	uint8_t rx_buffer[sizeof(tx_buffer)];

	const struct spi_buf tx_buf = {
		.buf = tx_buffer,
		.len = sizeof(tx_buffer)
	};
	const struct spi_buf_set tx_bufs = {
		.buffers = &tx_buf,
		.count = 1
	};

	struct spi_buf rx_buf = {
		.buf = rx_buffer,
		.len = sizeof(rx_buffer),
	};

	const struct spi_buf_set rx_bufs = {
		.buffers = &rx_buf,
		.count = 1
	};

	struct spi_config spi_cfg = {
		.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_HOLD_ON_CS | SPI_LOCK_ON,
		.frequency = 1000000,  // 1 MHz
		.slave = 0,
	};

#if LOOPBACK_MODE
	// For loopback, we don't need chip select control
	spi_cfg.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8);
#endif

	while (1) {
#if LOOPBACK_MODE
		LOG_INF("--- Loopback Test Cycle ---");
#else
		// Assert CS (set low) before transmission
		gpio_pin_set(cs_gpio, CS_PIN, 0);
		LOG_INF("CS asserted (low)");
#endif
    LOG_INF("Transmitting: %s", tx_buffer);
		int error = spi_transceive(spi_dev, &spi_cfg, &tx_bufs, &rx_bufs);
		if (error) {
			LOG_ERR("SPI transceive error: %d", error);
		} else {
#if LOOPBACK_MODE
			LOG_INF("Loopback received: %s", rx_buffer);
			// Verify loopback data matches transmitted data
			if (memcmp(tx_buffer, rx_buffer, sizeof(tx_buffer)) == 0) {
				LOG_INF("✓ Loopback test PASSED - data matches");
			} else {
				LOG_ERR("✗ Loopback test FAILED - data mismatch");
			}
#else
			LOG_INF("Received: %s", rx_buffer);
#endif
		}
		
#if !LOOPBACK_MODE
		// Deassert CS (set high) after transmission
		gpio_pin_set(cs_gpio, CS_PIN, 1);
		LOG_INF("CS deasserted (high)");
#endif
		
		k_sleep(K_MSEC(1000));
	}
	return 0;
} 