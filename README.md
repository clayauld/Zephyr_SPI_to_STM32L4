# Zephyr SPI to STM32L4 Zephyr Project

This project demonstrates SPI (Serial Peripheral Interface) communication using Zephyr RTOS. The application acts as an SPI master that sends data to an STM32L4 device and receives responses.

## Project Overview

The application implements:
- SPI master configuration with 1MHz frequency
- 8-bit word size communication
- Continuous data transmission with 1-second intervals
- Console logging for debugging and monitoring

## Prerequisites

- Zephyr RTOS development environment
- Python virtual environment with Zephyr SDK
- Supported hardware (nRF series or compatible board)
- STM32L4 target device for SPI communication

## Project Structure

```
Zephyr_SPI_to_STM32L4/
├── boards/           # Board-specific configurations
├── src/
│   └── main.c       # Main application source
├── CMakeLists.txt    # CMake configuration
├── prj.conf         # Zephyr project configuration
├── Makefile         # Build automation
└── README.md        # This file
```

## Configuration

### Project Configuration (`prj.conf`)

The project is configured with:
- **SPI Support**: Full SPI driver support with nRFX backend
- **Logging**: Comprehensive logging system for debugging
- **GPIO**: GPIO support for SPI control signals
- **UART Console**: Console output for monitoring
- **Printk**: Kernel print support

### Build Configuration

- **Default Board**: `nrf52840dk/nrf52840`
- **Build Strategy**: Uses `--pristine` flag for clean builds
- **Virtual Environment**: Automatically activates `../.venv`

### SPI Configuration

- **Mode**: Master mode
- **Frequency**: 1 MHz
- **Word Size**: 8 bits
- **Chip Select**: Hardware chip select enabled (P0.15)
- **Loopback Mode**: Available for testing (configurable in main.c)
- **Data**: Sends "Hello from nRF" message
- **Interval**: 1-second transmission cycles

## Building and Running

### Using Makefile (Recommended)

1. **Build the project (with pristine build):**
   ```bash
   make build
   ```

2. **Flash to target device:**
   ```bash
   make flash
   ```

3. **Monitor serial output:**
   ```bash
   make monitor
   ```

4. **Clean build artifacts:**
   ```bash
   make clean
   ```

5. **Full rebuild:**
   ```bash
   make rebuild
   ```

6. **Build and flash in one step:**
   ```bash
   make build-flash
   ```

### Manual Build Commands

If you prefer manual commands:

```bash
# Activate virtual environment
source ../.venv/bin/activate

# Build the project (with pristine to avoid conflicts)
west build -b <your_board> . --pristine

# Flash to device
west flash

# Monitor output
west espressif monitor
```

## Hardware Setup

### SPI Connections

Connect your target STM32L4 device with the following SPI pins:

| Signal | Description |
|--------|-------------|
| SCK    | Serial Clock |
| MOSI   | Master Out Slave In |
| MISO   | Master In Slave Out |
| CS     | Chip Select (if needed) |

### Pin Configuration

The default SPI configuration uses:
- **SPI Node**: `spi1` (configurable in main.c)
- **Slave Select**: 0 (configurable in main.c)

#### Default Pins for nRF52840DK

When using the nRF52840DK board, the default SPI1 pins are:

| Signal | Pin Name | Pin Number |
|--------|----------|------------|
| SCK    | P0.12    | 12         |
| MOSI   | P0.13    | 13         |
| MISO   | P0.14    | 14         |
| CS     | P0.15    | 15 (hardware chip select) |

**Note**: These are custom SPI1 pin assignments for the nRF52840DK using a board overlay. The CS pin (P0.15) is configured for hardware chip select operation.

### Loopback Testing

For testing SPI communication without an external device, you can enable loopback mode:

1. **Enable Loopback Mode**: Set `LOOPBACK_MODE` to `1` in `src/main.c`
2. **Hardware Connection**: Connect MOSI (P0.13) to MISO (P0.14) with a jumper wire
3. **Build and Flash**: Use `make build-flash` to deploy the loopback firmware
4. **Monitor Output**: Use `make monitor` to see the loopback test results

**Loopback Features**:
- Automatic data verification (transmitted vs received)
- Pass/fail status reporting
- Disabled chip select control (not needed for loopback)
- Enhanced logging for debugging

**Expected Output in Loopback Mode**:
```
=== LOOPBACK MODE ENABLED ===
Connect MOSI (P0.13) to MISO (P0.14) for loopback testing
CS pin will be ignored in loopback mode
--- Loopback Test Cycle ---
Transmitting: Hello from nRF
Loopback received: Hello from nRF
✓ Loopback test PASSED - data matches
```

## Monitoring and Debugging

### Console Output

The application provides detailed logging:
- **INFO**: Successful SPI transactions
- **ERROR**: SPI communication failures
- **Received Data**: Echo of received data from STM32L4

### Common Issues

1. **SPI Device Not Ready**
   - Check hardware connections
   - Verify board configuration
   - Ensure target device is powered

2. **Communication Errors**
   - Verify SPI frequency compatibility
   - Check word size configuration
   - Ensure proper slave select configuration

## Serial Logging and Terminal Connection

### Serial Terminal Setup

The application outputs detailed logging information through the UART console. To connect to the serial terminal:

#### Using the Makefile (Recommended)
```bash
make monitor
```

#### Manual Serial Terminal Connection

1. **Identify the serial port:**
   ```bash
   # List available serial devices
   ls /dev/ttyUSB* /dev/ttyACM* /dev/ttyS*
   
   # Or use dmesg to find the device
   dmesg | grep -i usb
   ```

2. **Connect using a serial terminal program:**

   **Using screen:**
   ```bash
   screen /dev/ttyUSB0 115200
   ```

   **Using minicom:**
   ```bash
   minicom -D /dev/ttyUSB0 -b 115200
   ```

   **Using picocom:**
   ```bash
   picocom /dev/ttyUSB0 -b 115200
   ```

   **Using PuTTY (Windows):**
   - Connection type: Serial
   - Serial line: COM port (e.g., COM3)
   - Speed: 115200

### Serial Configuration

- **Baud Rate**: 115200
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1
- **Flow Control**: None

### nRF52840DK Serial Pins

The nRF52840DK board uses the following default pins for serial communication (UART):

| Signal | Pin Name | Pin Number | Description |
|--------|----------|------------|-------------|
| TX     | P0.06    | 6          | UART TX (to host) |
| RX     | P0.08    | 8          | UART RX (from host) |

**Important Notes:**
- These pins are connected to the onboard USB-to-UART converter
- The USB connection provides both power and serial communication
- No external wiring is required for serial logging when using the USB connection
- The board appears as a USB CDC device (typically `/dev/ttyACM0` on Linux)

**Hardware Connection (if using external UART):**
- Connect a USB-to-UART adapter to pins P0.06 (TX) and P0.08 (RX)
- Ensure proper voltage levels (3.3V logic)
- Connect ground between the board and adapter

### Expected Serial Output

The application provides real-time logging with the following information:

**Normal Operation:**
```
*** Booting Zephyr OS build v3.7.1 ***
SPI Master Example
SPI device SPI_1 initialized successfully
--- SPI Transaction ---
Transmitting: Hello from nRF
Received: [response from STM32L4]
Transaction completed successfully
```

**Loopback Mode:**
```
*** Booting Zephyr OS build v3.7.1 ***
SPI Master Example
=== LOOPBACK MODE ENABLED ===
Connect MOSI (P0.13) to MISO (P0.14) for loopback testing
CS pin will be ignored in loopback mode
--- Loopback Test Cycle ---
Transmitting: Hello from nRF
Loopback received: Hello from nRF
✓ Loopback test PASSED - data matches
```

**Error Conditions:**
```
ERROR: SPI device not ready
ERROR: SPI transaction failed: -5
ERROR: Failed to initialize SPI device
```

### Serial Terminal Troubleshooting

1. **No Output Visible:**
   - Check if the device is properly connected
   - Verify the correct serial port is being used
   - Ensure the baud rate is set to 115200
   - Try pressing Enter or Ctrl+C to wake up the terminal

2. **Garbled Output:**
   - Verify baud rate is exactly 115200
   - Check for hardware flow control settings
   - Ensure no other programs are using the serial port

3. **Permission Denied:**
   ```bash
   # Add user to dialout group (Linux)
   sudo usermod -a -G dialout $USER
   
   # Or use sudo for temporary access
   sudo screen /dev/ttyUSB0 115200
   ```

4. **Device Not Found:**
   - Check USB cable connection
   - Verify device drivers are installed
   - Try different USB ports
   - Restart the terminal program

### Advanced Serial Monitoring

For more detailed debugging, you can:

1. **Enable Verbose Logging:**
   - Modify `prj.conf` to include additional logging levels
   - Add `CONFIG_LOG_DEFAULT_LEVEL=4` for debug level logging

2. **Log to File:**
   ```bash
   # Redirect serial output to a file
   make monitor | tee spi_log.txt
   
   # Or use screen with logging
   screen -L /dev/ttyUSB0 115200
   ```

3. **Filter Output:**
   ```bash
   # Show only SPI-related messages
   make monitor | grep -i spi
   
   # Show only error messages
   make monitor | grep -i error
   ```

## Customization

### Modifying SPI Parameters

Edit `src/main.c` to change:
- **Frequency**: Modify `spi_cfg.frequency`
- **Word Size**: Change `SPI_WORD_SET()` parameter
- **Chip Select Pin**: Update `CS_PIN` define (default: 15 for P0.15)
- **Loopback Mode**: Set `LOOPBACK_MODE` to `1` for testing, `0` for normal operation
- **Slave Select**: Update `spi_cfg.slave`
- **Transmission Data**: Modify `tx_buffer` content

### Adding New Features

1. **Additional SPI Devices**: Add new SPI configurations
2. **Data Processing**: Implement custom data handling
3. **Error Handling**: Add retry mechanisms
4. **Configuration**: Add runtime configuration options

## Development Workflow

1. **Setup Environment**: Ensure Zephyr SDK is properly configured
2. **Modify Code**: Edit `src/main.c` as needed
3. **Build**: Use `make build` for compilation (automatically uses pristine build)
4. **Flash**: Deploy to target with `make flash`
5. **Monitor**: Observe output with `make monitor`
6. **Debug**: Use logging and console output for troubleshooting

### Quick Development Cycle

For rapid development iterations:
```bash
make build-flash  # Build and flash in one command
make monitor      # Monitor the output
```

## Troubleshooting

### Build Issues
- Ensure virtual environment is activated (Makefile handles this automatically)
- Check Zephyr SDK installation
- Verify board support
- If board name conflicts occur, the Makefile automatically uses pristine builds

### Runtime Issues
- Check hardware connections
- Verify target device configuration
- Monitor console output for error messages

## License

This project is based on Nordic Semiconductor ASA code and is licensed under Apache-2.0.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Support

For issues and questions:
- Check the console output for error messages
- Verify hardware connections
- Review Zephyr documentation
- Check board-specific documentation 