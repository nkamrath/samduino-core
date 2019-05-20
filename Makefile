#make file for agi sim
TOOLCHAIN = arm-none-eabi-
AS = $(TOOLCHAIN)as
CC = $(TOOLCHAIN)gcc
CXX = $(TOOLCHAIN)g++
LD = $(TOOLCHAIN)ld
OBJ_COPY = $(TOOLCHAIN)objcopy

PROJECT = samduino

OUTPUT_DIR = output
CURRENT_DIR = ${CURDIR}

INCLUDE_DIRS = -I$(CURRENT_DIR)/src/core -I$(CURRENT_DIR)/src/core/ASF/thirdparty/freertos/freertos-10.0.0/Source/include/ -I$(CURRENT_DIR)/src/core/config/ -I$(CURRENT_DIR)/src/core/ASF/ -I$(CURRENT_DIR)/src/core/ASF/common/ -I$(CURRENT_DIR)/src/core/ASF/common/boards/ -I$(CURRENT_DIR)/src/core/ASF/common/boards/user_board/ -I$(CURRENT_DIR)/src/core/ASF/common/components/ -I$(CURRENT_DIR)/src/core/ASF/common/components/memory/ -I$(CURRENT_DIR)/src/core/ASF/common/components/memory/sd_mmc/ -I$(CURRENT_DIR)/src/core/ASF/common/services/ -I$(CURRENT_DIR)/src/core/ASF/common/services/clock/ -I$(CURRENT_DIR)/src/core/ASF/common/services/clock/sam4s/ -I$(CURRENT_DIR)/src/core/ASF/common/services/delay/ -I$(CURRENT_DIR)/src/core/ASF/common/services/delay/sam/ -I$(CURRENT_DIR)/src/core/ASF/common/services/gpio/ -I$(CURRENT_DIR)/src/core/ASF/common/services/gpio/sam_gpio/ -I$(CURRENT_DIR)/src/core/ASF/common/services/ioport/ -I$(CURRENT_DIR)/src/core/ASF/common/services/ioport/sam/ -I$(CURRENT_DIR)/src/core/ASF/common/services/sleepmgr/ -I$(CURRENT_DIR)/src/core/ASF/common/services/sleepmgr/sam/ -I$(CURRENT_DIR)/src/core/ASF/common/services/storage/ -I$(CURRENT_DIR)/src/core/ASF/common/services/storage/ctrl_access/ -I$(CURRENT_DIR)/src/core/ASF/common/services/usb/ -I$(CURRENT_DIR)/src/core/ASF/common/services/usb/class/ -I$(CURRENT_DIR)/src/core/ASF/common/services/usb/class/cdc/ -I$(CURRENT_DIR)/src/core/ASF/common/services/usb/class/cdc/device/ -I$(CURRENT_DIR)/src/core/ASF/common/services/usb/udc/ -I$(CURRENT_DIR)/src/core/ASF/common/services/twi/ -I$(CURRENT_DIR)/src/core/ASF/common/utils/ -I$(CURRENT_DIR)/src/core/ASF/common/utils/interrupt/ -I$(CURRENT_DIR)/src/core/ASF/common/utils/stdio/ -I$(CURRENT_DIR)/src/core/ASF/common/utils/stdio/stdio_usb/ -I$(CURRENT_DIR)/src/core/ASF/sam/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/adc/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/dacc/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/hsmci/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/matrix/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/pdc/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/pdc/pdc_uart_example/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/pio/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/pmc/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/rstc/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/rstc/example1/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/rtc/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/tc/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/tc/tc_capture_waveform_example/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/uart/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/spi/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/twi/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/udp/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/usart/ -I$(CURRENT_DIR)/src/core/ASF/sam/drivers/efc/ -I$(CURRENT_DIR)/src/core/ASF/sam/services/flash_efc/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/cmsis/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/cmsis/sam4s/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/cmsis/sam4s/include/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/cmsis/sam4s/include/component/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/cmsis/sam4s/include/instance/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/cmsis/sam4s/include/pio/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/cmsis/sam4s/source/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/cmsis/sam4s/source/templates/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/cmsis/sam4s/source/templates/gcc/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/header_files/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/linker_scripts/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/linker_scripts/sam4s/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/linker_scripts/sam4s/sam4s8/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/linker_scripts/sam4s/sam4s8/gcc/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/make/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/preprocessor/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/syscalls/ -I$(CURRENT_DIR)/src/core/ASF/sam/utils/syscalls/gcc/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/CMSIS/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/CMSIS/Include/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/CMSIS/Lib/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/CMSIS/Lib/GCC/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/fatfs/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/fatfs/fatfs-port-r0.09/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/fatfs/fatfs-port-r0.09/sam/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/fatfs/fatfs-r0.09/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/fatfs/fatfs-r0.09/doc/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/fatfs/fatfs-r0.09/doc/en/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/fatfs/fatfs-r0.09/doc/img/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/fatfs/fatfs-r0.09/src/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/fatfs/fatfs-r0.09/src/option/ -I$(CURRENT_DIR)/src/core/ASF/thirdparty/freertos/freertos-10.0.0/Source/portable/GCC/ARM_CM3/

DEFINES = -DANVEL_SAMPLE_PLUGIN_EXPORT -D_LINUX -DANVEL_CONFIG_DEBUG

ASFLAGS = -c -g -x assembler-with-cpp
CCFLAGS = -D__SAM4S8B__ -DARM_MATH_CM4=true -mthumb -DBOARD=USER_BOARD -DDEBUG -Dscanf=iscanf -Dprintf=iprintf -DUDD_ENABLE -c -g -O1 -fdata-sections -ffunction-sections -mlong-calls -g3 -Wall -mcpu=cortex-m4 -c -pipe -fno-strict-aliasing -std=gnu99 -ffunction-sections -fdata-sections
CXXFLAGS = -D__SAM4S8B__ -DARM_MATH_CM4=true -mthumb -DBOARD=USER_BOARD -DDEBUG -Dscanf=iscanf -Dprintf=iprintf -DUDD_ENABLE -c -g -Os  -O1 -ffunction-sections -fno-rtti -fno-exceptions -mlong-calls -g3 -mcpu=cortex-m4

LINKER_FLAGS = -Wl,--start-group -larm_cortexM4l_math -lm  -Wl,--end-group -e Reset_Handler -mthumb -mcpu=cortex-m4 -Wl,--gc-sections
LIBS = -L$(CURRENT_DIR)/src/core/ASF/thirdparty/CMSIS/Lib/GCC -larm_cortexM4l_math -lm
LINKER_SCRIPT = -T$(CURRENT_DIR)/src/core/ASF/sam/utils/linker_scripts/sam4s/sam4s8/gcc/flash.ld

OBJ_COPY_FLAGS = -O binary

AS_SRCS=$(shell find . -name '*.asm')
CC_SRCS=$(shell find -L . -name '*.c')
CXX_SRCS=$(shell find . -name '*.cpp')

AS_OBJS=$(patsubst %.asm,$(OUTPUT_DIR)/%.0,$(AS_SRCS))
CC_OBJS=$(patsubst %.c,$(OUTPUT_DIR)/%.o,$(CC_SRCS))
CXX_OBJS=$(patsubst %.cpp,$(OUTPUT_DIR)/%.o,$(CXX_SRCS))

all: $(OUTPUT_DIR)/$(PROJECT)
	

$(OUTPUT_DIR)/$(PROJECT): $(CC_OBJS) $(CXX_OBJS)
	@echo compiling main
	@$(CC) $(LINKER_FLAGS) $(DEFINES) $(CC_OBJS) $(CXX_OBJS) $(LIBS) $(LINKER_SCRIPT) -o $@.out
	@echo creating binary $@.bin
	@$(OBJ_COPY) $@.out $(OBJ_COPY_FLAGS) $@.bin

$(OUTPUT_DIR)/%.o: %.c
	@echo compiling $<
	@mkdir -p $(@D)
	@$(CC) $(CCFLAGS) $(INCLUDE_DIRS) $(DEFINES) -MMD -MF $@.d -c $< -o $@

$(OUTPUT_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(DEFINES) -MMD -MF $@.d -c $< -o $@
	

rebuild: clean all

clean:
	rm -rf output

.PHONY: all clean