/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <string.h>

// SPI Configuration
#define SPI_NODE DT_NODELABEL(spi2)

// Message configuration
#define MESSAGE_SIZE 14
static const uint8_t tx_message[] = "Hello from nRF";
static uint8_t rx_message[MESSAGE_SIZE];

// SPI device
static const struct device *spi_dev;

int main(void)
{
    int ret;
    
    printk("=== nRF52840 SPI Master - Full Buffer Hardware CS Test ===\n");
    printk("Target: STM32L4 SPI Slave\n");
    printk("Frequency: 125 kHz\n");
    printk("Mode: CPOL=0, CPHA=0 (Mode 0)\n");
    printk("Data size: 8-bit\n");
    printk("Chip Select: Hardware CS enabled\n");
    printk("Test: Full buffer transmission\n");
    printk("Send: 'Hello from nRF'\n");
    printk("Expect: 'Hi from STM32'\n");
    printk("==============================================\n\n");

    // Get SPI device
    spi_dev = DEVICE_DT_GET(SPI_NODE);
    if (!device_is_ready(spi_dev)) {
        printk("ERROR: SPI device not ready\n");
        return -1;
    }
    printk("SPI device ready\n");

    printk("\nStarting continuous SPI transmission test...\n");
    printk("Will send: '%s' (%d bytes) every 10 seconds\n", tx_message, MESSAGE_SIZE);

    // SPI configuration for full buffer transmission with hardware CS
    struct spi_config spi_cfg = {
        .frequency = 125000,  // 125 kHz - minimum for nRF52840
        .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
        .slave = 0,
        .cs = {
            .gpio = GPIO_DT_SPEC_GET(SPI_NODE, cs_gpios),
        },
    };

    // Prepare SPI buffers
    struct spi_buf tx_buf = {
        .buf = (void*)tx_message,
        .len = MESSAGE_SIZE
    };
    
    struct spi_buf rx_buf = {
        .buf = rx_message,
        .len = MESSAGE_SIZE
    };
    
    const struct spi_buf_set tx_set = {
        .buffers = &tx_buf,
        .count = 1
    };
    
    const struct spi_buf_set rx_set = {
        .buffers = &rx_buf,
        .count = 1
    };

    int transaction_count = 0;
    
    while (1) {
        transaction_count++;
        printk("\n=== Transaction %d ===\n", transaction_count);
        printk("Sending: '%s' (%d bytes)\n", tx_message, MESSAGE_SIZE);

        // Transmit full buffer with hardware CS control
        ret = spi_transceive(spi_dev, &spi_cfg, &tx_set, &rx_set);
        
        if (ret < 0) {
            printk("ERROR: SPI transaction failed: %d\n", ret);
        } else {
            printk("SPI transaction completed successfully!\n");
            printk("Sent: '%s'\n", tx_message);
            printk("Received: ");
            for (int i = 0; i < MESSAGE_SIZE; i++) {
                printk("%02X ", rx_message[i]);
            }
            printk("\n");
            
            // Print received data as ASCII
            printk("Received ASCII: ");
            for (int i = 0; i < MESSAGE_SIZE; i++) {
                if (rx_message[i] >= 32 && rx_message[i] <= 126) {
                    printk("%c", rx_message[i]);
                } else {
                    printk("\\x%02X", rx_message[i]);
                }
            }
            printk("\n");
            
            // Check if we received the expected response
            if (strncmp((char*)rx_message, "Hi from STM32", 14) == 0) {
                printk("SUCCESS: Received expected response 'Hi from STM32'\n");
            } else {
                printk("ERROR: Unexpected response received\n");
            }
        }
        
        printk("Waiting 10 seconds before next transaction...\n");
        k_sleep(K_MSEC(10000));  // Wait 10 seconds
    }
    
    return 0;
} 