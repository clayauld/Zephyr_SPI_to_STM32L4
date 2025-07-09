# Zephyr SPI to STM32L4 Zephyr Project

This project demonstrates SPI (Serial Peripheral Interface) communication using Zephyr RTOS. The application acts as an SPI master that sends data to an STM32L4 device and receives responses.

## Project Overview

The application implements:
- SPI master configuration with 125kHz frequency (minimum for nRF52840)
- 8-bit word size communication with Mode 0 (CPOL=0, CPHA=0)
- **Hardware chip select (CS) control using SPI driver and code configuration**
- Continuous data transmission with 10-second intervals
- Enhanced console logging with hex and ASCII output
- Specific response validation from STM32L4 target

## Prerequisites

- Zephyr RTOS development environment
- Python virtual environment with Zephyr SDK
- Supported hardware (nRF52840DK or compatible board)
- STM32L4 target device for SPI communication

## Project Structure

```
Zephyr_SPI_to_STM32L4/
├── boards/           # Board-specific configurations
│   └── nrf52840dk_nrf52840.overlay  # Custom SPI2 pin assignments with hardware CS
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
- **GPIO**: GPIO support for hardware chip select control
- **UART Console**: Console output for monitoring
- **Printk**: Kernel print support

### Build Configuration

- **Default Board**: `nrf52840dk/nrf52840`
- **Build Strategy**: Uses `--pristine` flag for clean builds
- **Virtual Environment**: Automatically activates `../.venv`

### SPI Configuration

- **Mode**: Master mode with Mode 0 (CPOL=0, CPHA=0)
- **Frequency**: 125 kHz (minimum for nRF52840)
- **Word Size**: 8 bits
- **Chip Select**: Hardware chip select enabled (P0.15, SPI2)
- **Target Device**: STM32L4 SPI Slave
- **Data**: Sends "Hello from nRF" message
- **Expected Response**: "Hi from STM32"
- **Interval**: 10-second transmission cycles

## Hardware Chip Select (CS) Configuration

**To enable hardware CS, you must configure BOTH:**

1. **Devicetree overlay** (`boards/nrf52840dk_nrf52840.overlay`):
   - Set the CS pin in the SPI2 node:
     ```dts
     &spi2 {
         status = "okay";
         pinctrl-0 = <&spi2_default>;
         pinctrl-1 = <&spi2_sleep>;
         pinctrl-names = "default", "sleep";
         cs-gpios = <&gpio0 15 GPIO_ACTIVE_LOW>;
     };
     ```
2. **Code configuration** (`src/main.c`):
   - Set the `.cs` field in your `struct spi_config`:
     ```c
     struct spi_config spi_cfg = {
         .frequency = 125000,
         .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
         .slave = 0,
         .cs = {
             .gpio = GPIO_DT_SPEC_GET(SPI_NODE, cs_gpios),
         },
     };
     ```
- **If you do not set `.cs` in code, the driver will NOT toggle the CS line!**


### Pin Configuration

The default SPI configuration uses:
- **SPI Node**: `spi2` (configurable in main.c)
- **Slave Select**: 0 (configurable in main.c)
- **Chip Select**: Hardware chip select on P0.15

#### Default Pins for nRF52840DK (SPI2)

| Signal | Pin Name | Pin Number | Description |
|--------|----------|------------|-------------|
| SCK    | P0.12    | 12         | Serial Clock |
| MOSI   | P0.13    | 13         | Master Out Slave In |
| MISO   | P0.14    | 14         | Master In Slave Out |
| CS     | P0.15    | 15         | Hardware Chip Select |

**Note**: These are custom SPI2 pin assignments for the nRF52840DK using a board overlay. The CS pin (P0.15) is configured for hardware chip select control through both devicetree and code.

### STM32L4 Target Configuration

The application is designed to communicate with an STM32L4 device configured as an SPI slave:

- **SPI Mode**: Slave mode with Mode 0 (CPOL=0, CPHA=0)
- **Frequency**: Must support 125kHz master clock
- **Word Size**: 8 bits
- **Expected Response**: "Hi from STM32" (14 bytes)

## Application Behavior

### Transmission Cycle

The application performs the following cycle every 10 seconds:

1. **Transaction Start**: Hardware CS automatically pulled low by SPI driver
2. **Data Transmission**: Sends "Hello from nRF" (14 bytes)
3. **Data Reception**: Receives response from STM32L4
4. **Transaction End**: Hardware CS automatically pulled high by SPI driver
5. **Validation**: Checks if received data matches expected response
6. **Logging**: Outputs detailed transaction information

### Expected Output

**Normal Operation:**
```
=== nRF52840 SPI Master - Hardware CS Test ===
Target: STM32L4 SPI Slave
Frequency: 125 kHz
Mode: CPOL=0, CPHA=0 (Mode 0)
Data size: 8-bit
Chip Select: Hardware CS enabled
Test: Full buffer transmission
Send: 'Hello from nRF'
Expect: 'Hi from STM32'
==============================================

SPI device ready

Starting continuous SPI transmission test...
Will send: 'Hello from nRF' (14 bytes) every 10 seconds

=== Transaction 1 ===
Sending: 'Hello from nRF' (14 bytes)
SPI transaction completed successfully!
Sent: 'Hello from nRF'
Received: 48 69 20 66 72 6F 6D 20 53 54 4D 33 32 00
Received ASCII: Hi from STM32
SUCCESS: Received expected response 'Hi from STM32'
Waiting 10 seconds before next transaction...
```

**Error Conditions:**
```
ERROR: SPI device not ready
ERROR: SPI transaction failed: -5
ERROR: Unexpected response received
```

## Monitoring and Debugging

### Console Output

The application provides detailed logging:
- **INFO**: Successful SPI transactions and device initialization
- **ERROR**: SPI communication failures and device errors
- **Received Data**: Both hex and ASCII representation of received data
- **Validation**: Automatic response validation against expected data

### Common Issues

1. **SPI Device Not Ready**
   - Check hardware connections
   - Verify board configuration
   - Ensure target device is powered

2. **Communication Errors**
   - Verify SPI frequency compatibility (125kHz)
   - Check word size configuration (8-bit)
   - Ensure proper slave select configuration
   - Verify STM32L4 is configured as SPI slave

3. **Unexpected Response**
   - Check STM32L4 SPI slave configuration
   - Verify the slave is sending "Hi from STM32"
   - Check timing and clock polarity settings

4. **Hardware CS Issues**
   - **Both devicetree and code must configure CS**
   - Verify CS pin is properly connected
   - Check board overlay configuration
   - Ensure no conflicts with other GPIO usage

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

1. **Log to File:**
   ```bash
   # Redirect serial output to a file
   make monitor | tee spi_log.txt
   
   # Or use screen with logging
   screen -L /dev/ttyUSB0 115200
   ```

2. **Filter Output:**
   ```bash
   # Show only SPI-related messages
   make monitor | grep -i spi
   
   # Show only error messages
   make monitor | grep -i error
   
   # Show only transaction messages
   make monitor | grep -i transaction
   ```

## Customization

### Modifying SPI Parameters

Edit `src/main.c` to change:
- **Frequency**: Modify `spi_cfg.frequency` (currently 125000 Hz)
- **Word Size**: Change `SPI_WORD_SET()` parameter (currently 8)
- **Slave Select**: Update `spi_cfg.slave` (currently 0)
- **Transmission Data**: Modify `tx_message` content
- **Expected Response**: Update the response validation logic
- **Transaction Interval**: Change `k_sleep(K_MSEC(10000))` value

### Modifying Hardware CS Configuration

To change the chip select pin, edit both:
- `boards/nrf52840dk_nrf52840.overlay`: update the `cs-gpios` line
- `src/main.c`: update the `.cs.gpio` field in `spi_config`

### Adding New Features

1. **Additional SPI Devices**: Add new SPI configurations
2. **Data Processing**: Implement custom data handling
3. **Error Handling**: Add retry mechanisms
4. **Configuration**: Add runtime configuration options
5. **Multiple Slaves**: Implement multi-slave communication

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
- Ensure STM32L4 is properly configured as SPI slave

### STM32L4 Configuration Requirements

The STM32L4 target device must be configured as an SPI slave with:
- **Mode**: SPI Slave Mode 0 (CPOL=0, CPHA=0)
- **Frequency**: Must accept 125kHz master clock
- **Word Size**: 8 bits
- **Response**: Must send "Hi from STM32" when receiving "Hello from nRF"

### Hardware CS Troubleshooting

1. **CS Pin Not Working:**
   - Both devicetree and code must configure CS
   - Verify the CS pin is not used by other peripherals
   - Check the board overlay configuration
   - Ensure the pin is properly connected to the target device

2. **CS Timing Issues:**
   - Hardware CS provides automatic timing
   - No manual delays needed
   - CS is automatically controlled by the SPI driver

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
- Ensure STM32L4 slave configuration is correct 