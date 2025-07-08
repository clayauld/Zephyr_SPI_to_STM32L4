# Makefile for SPI to STM32L4 Zephyr Project
# This Makefile automates the build process with virtual environment activation

# Configuration
VENV_PATH ?= ../.venv
VENV_ACTIVATE = $(VENV_PATH)/bin/activate
BOARD ?= nrf52840dk/nrf52840
BUILD_DIR = build
PROJECT_NAME = nrf_spi_master

# Default target
.PHONY: all
all: build

# Check if virtual environment exists
.PHONY: check-venv
check-venv:
	@if [ ! -f "$(VENV_ACTIVATE)" ]; then \
		echo "Error: Virtual environment not found at $(VENV_PATH)"; \
		echo "Please create a virtual environment with Zephyr SDK first:"; \
		echo "  python3 -m venv $(VENV_PATH)"; \
		echo "  source $(VENV_ACTIVATE)"; \
		echo "  pip install west"; \
		echo "  west init -m https://github.com/zephyrproject-rtos/zephyr --mr v3.7.1"; \
		echo "  west update"; \
		echo "  west zephyr-export"; \
		echo "  pip install -r ~/.local/share/zephyr-sdk/scripts/requirements.txt"; \
		exit 1; \
	fi

# Activate virtual environment and run command
define run_with_venv
	@echo "Activating virtual environment..."
	@bash -c "source $(VENV_ACTIVATE) && $(1)" || . $(VENV_ACTIVATE) && $(1)
endef

# Build the project (with pristine to avoid board name conflicts)
.PHONY: build
build: check-venv
	@echo "Building $(PROJECT_NAME) for board $(BOARD)..."
	$(call run_with_venv,west build -b $(BOARD) . --pristine)

# Flash the project to target device
.PHONY: flash
flash: check-venv
	@echo "Flashing $(PROJECT_NAME) to $(BOARD)..."
	$(call run_with_venv,west flash)

# Build and flash in one step
.PHONY: build-flash
build-flash: build flash

# Monitor serial output
.PHONY: monitor
monitor: check-venv
	@echo "Starting serial monitor..."
	$(call run_with_venv,west espressif monitor)

# Alternative monitor command for different boards
.PHONY: monitor-alt
monitor-alt: check-venv
	@echo "Starting alternative serial monitor..."
	$(call run_with_venv,west espressif monitor --baud-rate 115200)

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Build artifacts cleaned."

# Full rebuild (clean + build)
.PHONY: rebuild
rebuild: clean build

# Show project information
.PHONY: info
info:
	@echo "=== SPI to STM32L4 Zephyr Project ==="
	@echo "Project Name: $(PROJECT_NAME)"
	@echo "Target Board: $(BOARD)"
	@echo "Build Directory: $(BUILD_DIR)"
	@echo "Virtual Environment: $(VENV_PATH)"
	@echo ""
	@echo "Available targets:"
	@echo "  build       - Build the project (with pristine)"
	@echo "  flash       - Flash to target device"
	@echo "  build-flash - Build and flash in one step"
	@echo "  monitor     - Monitor serial output"
	@echo "  monitor-alt - Alternative monitor command"
	@echo "  clean       - Clean build artifacts"
	@echo "  rebuild     - Full rebuild (clean + build)"
	@echo "  info        - Show this information"

# Show help
.PHONY: help
help: info

# Check Zephyr environment
.PHONY: check-env
check-env: check-venv
	@echo "Checking Zephyr environment..."
	$(call run_with_venv,west --version)
	$(call run_with_venv,zephyr-env-check)

# List available boards
.PHONY: boards
boards: check-venv
	@echo "Available boards:"
	$(call run_with_venv,west boards)

# Configure for different board
.PHONY: configure
configure:
	@echo "Usage: make BOARD=<board_name> build"
	@echo "Example: make BOARD=nrf52840dk/nrf52840 build"
	@echo ""
	@echo "Available boards:"
	$(call run_with_venv,west boards)

# Development workflow targets
.PHONY: dev-build
dev-build: build
	@echo "Development build completed."

.PHONY: dev-flash
dev-flash: build-flash
	@echo "Development flash completed."

.PHONY: dev-monitor
dev-monitor: monitor
	@echo "Development monitoring started."

# Quick development cycle
.PHONY: dev-cycle
dev-cycle: dev-build dev-flash dev-monitor

# Install dependencies (if needed)
.PHONY: install-deps
install-deps:
	@echo "Installing Zephyr dependencies..."
	@if [ ! -f "$(VENV_ACTIVATE)" ]; then \
		echo "Creating virtual environment..."; \
		python3 -m venv $(VENV_PATH); \
	fi
	$(call run_with_venv,pip install --upgrade pip)
	$(call run_with_venv,pip install west)
	@echo "Dependencies installed. Please run 'west init' and 'west update' manually."

# Setup project (first time)
.PHONY: setup
setup: install-deps
	@echo "Project setup completed."
	@echo "Next steps:"
	@echo "1. Run: source $(VENV_ACTIVATE)"
	@echo "2. Run: west init -m https://github.com/zephyrproject-rtos/zephyr --mr v3.7.1"
	@echo "3. Run: west update"
	@echo "4. Run: west zephyr-export"
	@echo "5. Run: pip install -r ~/.local/share/zephyr-sdk/scripts/requirements.txt"

# Default target
.DEFAULT_GOAL := build 