# ==============================================================================
# User-configurable options
# ==============================================================================
CONFIG ?= Release

# Linuxならデフォルトで静的リンクON
ifeq ($(shell uname -s),Linux)
    STATIC ?= ON
else
    STATIC ?= OFF
endif

TOP_DIR := $(CURDIR)
CXX ?= g++
CC  ?= gcc

# ==============================================================================
# Supported targets
# ==============================================================================
TARGETS := application example extra test

# ==============================================================================
# Default target (build all)
# ==============================================================================
.PHONY: all $(TARGETS) run-test clean
all: $(TARGETS)

# ==============================================================================
# Generic build rule for each target
# ==============================================================================
$(TARGETS):
	@echo "=== Building target: $@ ==="
	$(eval BUILD_DIR := $(TOP_DIR)/build/$@/$(CONFIG))
	mkdir -p $(BUILD_DIR)
	@echo "CMake configure..."
	cd $(BUILD_DIR) && \
	cmake \
	    -DCMAKE_BUILD_TYPE=$(CONFIG) \
	    -DCMAKE_CXX_COMPILER=$(CXX) \
	    -DCMAKE_C_COMPILER=$(CC) \
	    -DTOP_DIR=$(TOP_DIR) \
	    -DLINK_STATIC=$(STATIC) \
	    $(TOP_DIR)/cmake/$@
	@echo "Building..."
	cmake --build $(BUILD_DIR) -- -j4

# ==============================================================================
# Run all test executables (without changing directory)
# ==============================================================================
.PHONY: run-test
run-test: test
	@echo "=== Running all test executables via CTest ==="
	ctest --test-dir $(TOP_DIR)/build/test/$(CONFIG) --output-on-failure

# ==============================================================================
# Debug shortcut
# ==============================================================================
.PHONY: debug
debug:
	$(MAKE) all CONFIG=Debug

# ==============================================================================
# Clean
# ==============================================================================
.PHONY: clean
clean:
	@echo "=== Cleaning all build directories ==="
	rm -rf $(TOP_DIR)/build
