###################################
#         PROJECT NAME            
###################################
PROJECT     := bootloader
MAIN_SRC    := bootloader.c

###################################
#        ARCHITECTURE             
###################################
MCU         := cortex-m4
FPU         := -mfpu=fpv4-sp-d16
FLOAT_ABI   := -mfloat-abi=hard

###################################
#      STARTUP & LINKER           
###################################
STARTUP_FILE := startup_stm32f40xx.s
LD_SCRIPT    := stm32f4_flash.ld

###################################
#          TOOLCHAINS             
###################################
TOOLCHAINS_PATH    := /home/bdang/toolchains/gcc-arm-none-eabi-14.3
PREFIX_TOOLCHAINS  := arm-none-eabi
CC  := $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-gcc
LD  := $(CC)
AS  := $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-as
CP  := $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-objcopy
SZ  := $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-size

###################################
#         DIRECTORIES             
###################################
BUILD_DIR   := build

# Include dirs
INCDIR += ./CMSIS/inc
INCDIR += ./SPL/inc
INCDIR += .

# SPL source dirs (chỉ chọn file cần)
SPL_SRC := \
    SPL/src/misc.c \
    SPL/src/stm32f4xx_flash.c \
    SPL/src/stm32f4xx_rcc.c \
    SPL/src/stm32f4xx_syscfg.c \
	SPL/src/system_stm32f4xx.c 

###################################
#       SOURCE FILES              
###################################
C_SOURCES := $(MAIN_SRC) $(SPL_SRC)

###################################
#            OBJECTS              
###################################
OBJECTS := $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)
STARTUP_OBJECT := $(BUILD_DIR)/startup.o
ALL_OBJECTS := $(OBJECTS) $(STARTUP_OBJECT)

###################################
#             FLAGS               
###################################
CFLAGS := -mcpu=$(MCU) -mthumb $(FPU) $(FLOAT_ABI)
CFLAGS += -Wall -Wextra -O0 -g3
CFLAGS += -DSTM32F401xx -DUSE_STDPERIPH_DRIVER -DSTM32F40XX
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += $(addprefix -I,$(INCDIR))

ASFLAGS := -mcpu=$(MCU) -mthumb $(FPU) $(FLOAT_ABI) -g3

LDFLAGS := -mcpu=$(MCU) -mthumb $(FPU) $(FLOAT_ABI)
LDFLAGS += -T$(LD_SCRIPT)
LDFLAGS += --specs=nano.specs --specs=nosys.specs
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map,--cref
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--no-warn-rwx-segments

###################################
#         BUILD RULES             
###################################

.PHONY: all clean flash debug bin hex

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).hex
	@echo "Bootloader build complete!"

bin: $(BUILD_DIR)/$(PROJECT).bin
hex: $(BUILD_DIR)/$(PROJECT).hex

# Build .o from .c
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Build startup .s
$(STARTUP_OBJECT): $(STARTUP_FILE)
	@mkdir -p $(BUILD_DIR)
	@echo "AS $<"
	@$(CC) $(ASFLAGS) -c $< -o $@

# Link ELF
$(BUILD_DIR)/$(PROJECT).elf: $(ALL_OBJECTS)
	@echo "LD $@"
	@$(LD) $(ALL_OBJECTS) $(LDFLAGS) -o $@
	@$(SZ) $@

# Generate BIN
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo "OBJCOPY $@"
	@$(CP) -O binary $< $@

# Generate HEX
$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	@echo "OBJCOPY $@"
	@$(CP) -O ihex $< $@

clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)

flash: $(BUILD_DIR)/$(PROJECT).bin
	@echo "Flashing bootloader to 0x08000000..."
	st-flash write $< 0x08000000

debug: $(BUILD_DIR)/$(PROJECT).elf
	gdb-multiarch $< \
	-ex "target extended-remote localhost:3333" \
	-ex "monitor reset halt" \
	-ex "load" \
	-ex "break main" \
	-ex "continue"
