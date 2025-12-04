###################################
#           PROJECT NAME          
###################################
PROJECT     := noma
MAIN_SRC    := main.c

###################################
#           ARCHITECTURE          
###################################
MCU         := cortex-m4
FPU         := -mfpu=fpv4-sp-d16
FLOAT_ABI   := -mfloat-abi=hard

###################################
#        	STARTUP & LINKER          
###################################
LD_SCRIPT    := stm32f4_flash.ld
STARTUP_FILE := startup_stm32f40xx.s

####################################
# 		 	TOOLCHAINS
####################################
TOOLCHAINS_PATH   	:= /home/bdang/toolchains/gcc-arm-none-eabi-14.3
PREFIX_TOOLCHAINS 	:= arm-none-eabi
CC  				:= $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-gcc
LD  				:= $(CC)
AS  				:= $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-as
CP  				:= $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-objcopy
SZ  				:= $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-size

####################################
# 			DIRECTORIES
####################################

BUILD_DIR   	:= z_build
APP_BUILD_DIR	:= $(BUILD_DIR)/a_application
MID_BUILD_DIR	:= $(BUILD_DIR)/b_middleware
HRD_BUILD_DIR	:= $(BUILD_DIR)/c_hardware
THR_BUILD_DIR	:= $(BUILD_DIR)/y_thirdparty
ROOT_BUILD_DIR	:= $(BUILD_DIR)/root

LINKER_DIR  	:= ./c_hardware/Device/linker
STARTUP_DIR 	:= ./c_hardware/Device/startup

#	APPLICATION
INCDIR += ./a_application
INCDIR += ./a_application/gpio_task
INCDIR += ./a_application/usart_task

#	MIDDLEWARE
INCDIR += ./b_middleware/at_engine
INCDIR += ./b_middleware/callback
INCDIR += ./b_middleware/debug_driver
INCDIR += ./b_middleware/event_driver
INCDIR += ./b_middleware/gpio_driver
INCDIR += ./b_middleware/gsm_driver
INCDIR += ./b_middleware/processor
INCDIR += ./b_middleware/string_handler
INCDIR += ./b_middleware/typedef
INCDIR += ./b_middleware/usart_driver
INCDIR += ./b_middleware/flash_driver
INCDIR += ./b_middleware/ota_engine


#	HARDWARE
INCDIR += ./c_hardware/CMSIS/inc
INCDIR += ./c_hardware/Device/inc
INCDIR += ./c_hardware/HAL/inc
INCDIR += ./c_hardware/SPL/inc

#	THIRD PARTY
INCDIR += ./y_thirdparty/lwrb

####################################
# 			FIND SOURCE .C
####################################
APP_C_SOURCES := $(shell find ./a_application -name "*.c")
MID_C_SOURCES := $(shell find ./b_middleware -name "*.c")
HRD_C_SOURCES := $(shell find ./c_hardware -name "*.c")
THR_C_SOURCES := $(shell find ./y_thirdparty -name "*.c")

# Add main.c from root directory
ROOT_C_SOURCES := ./main.c

####################################
#			OBJECTS
####################################
APP_OBJECTS := $(APP_C_SOURCES:./a_application/%.c=$(APP_BUILD_DIR)/%.o)
MID_OBJECTS := $(MID_C_SOURCES:./b_middleware/%.c=$(MID_BUILD_DIR)/%.o)
HRD_OBJECTS := $(HRD_C_SOURCES:./c_hardware/%.c=$(HRD_BUILD_DIR)/%.o)
THR_OBJECTS := $(THR_C_SOURCES:./y_thirdparty/%.c=$(THR_BUILD_DIR)/%.o)
ROOT_OBJECTS := $(ROOT_C_SOURCES:./%.c=$(ROOT_BUILD_DIR)/%.o)

STARTUP_OBJECT := $(BUILD_DIR)/startup/startup.o

ALL_OBJECTS := $(APP_OBJECTS) $(MID_OBJECTS) $(HRD_OBJECTS) $(THR_OBJECTS) $(ROOT_OBJECTS) $(STARTUP_OBJECT)

####################################
#			FLAGS
####################################
# Common C flags for all files
CFLAGS := -mcpu=$(MCU) -mthumb $(FPU) $(FLOAT_ABI)
CFLAGS += -Wall -Wextra -O0 -g3
CFLAGS += -DSTM32F401xx -DSTM32F40XX
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += $(addprefix -I,$(INCDIR))

# Assembly flags
ASFLAGS := -mcpu=$(MCU) -mthumb $(FPU) $(FLOAT_ABI) -g

# Linker flags
LDFLAGS := -mcpu=$(MCU) -mthumb $(FPU) $(FLOAT_ABI)
LDFLAGS += -T$(LINKER_DIR)/$(LD_SCRIPT)
LDFLAGS += --specs=nano.specs
LDFLAGS += --specs=nosys.specs
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map,--cref
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--no-warn-rwx-segments

####################################
#			BUILD RULES
####################################

.PHONY: all clean flash debug bin hex

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).hex
	@echo "Build complete!"

bin: $(BUILD_DIR)/$(PROJECT).bin
hex: $(BUILD_DIR)/$(PROJECT).hex

# Application objects
$(APP_BUILD_DIR)/%.o: ./a_application/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Middleware objects
$(MID_BUILD_DIR)/%.o: ./b_middleware/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Hardware objects
$(HRD_BUILD_DIR)/%.o: ./c_hardware/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Third party objects
$(THR_BUILD_DIR)/%.o: ./y_thirdparty/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Root objects (main.c)
$(ROOT_BUILD_DIR)/%.o: ./%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Startup assembly
$(STARTUP_OBJECT): $(STARTUP_DIR)/$(STARTUP_FILE)
	@mkdir -p $(BUILD_DIR)/startup
	@echo "AS $<"
	@$(CC) $(ASFLAGS) -c $< -o $@

# Link
$(BUILD_DIR)/$(PROJECT).elf: $(ALL_OBJECTS)
	@echo "LD $@"
	@$(LD) $(ALL_OBJECTS) $(LDFLAGS) -o $@
	@$(SZ) $@

# Generate hex
$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	@echo "OBJCOPY $@"
	@$(CP) -O ihex $< $@

# Generate bin
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo "OBJCOPY $@"
	@$(CP) -O binary $< $@

clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)

flash: $(BUILD_DIR)/$(PROJECT).bin
	@echo "Flashing to STM32..."
	st-flash write $< 0x8000000

debug: $(BUILD_DIR)/$(PROJECT).elf
	gdb-multiarch $< \
	-ex "target extended-remote localhost:3333" \
	-ex "monitor reset halt" \
	-ex "load" \
	-ex "break main" \
	-ex "continue"