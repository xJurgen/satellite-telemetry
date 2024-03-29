VERSION = 1.0
DATETIME := $(shell date --iso-8601=seconds)

Q 				:= @
MFLAGS 			+= --no-print-dir


OPT_FLAGS 		?= -O2

CFLAGS 			+= -Wall -Wextra -Werror -Wno-char-subscripts -pedantic\
        		$(OPT_FLAGS) -std=gnu99 -g3 -MD \
        		-I.

LDSCRIPT		= blackpill.ld
BINARY			= satellite_telemetry

LD_FLAGS 		+= $(OPT_FLAGS)

LIBNAME         = opencm3_stm32f4
DEFS            += -DSTM32F4

FP_FLAGS        ?= -mfloat-abi=hard -mfpu=fpv4-sp-d16
ARCH_FLAGS      = -mthumb -mcpu=cortex-m4 $(FP_FLAGS)

CROSS_COMPILE	?= arm-none-eabi-
CC				= $(CROSS_COMPILE)gcc
OBJCOPY			= $(CROSS_COMPILE)objcopy

#OPT_FLAGS       = -Os

CFLAGS 			+= $(ARCH_FLAGS) $(DEFS) \
					-DDISCOVERY_STLINK -I libopencm3/include -I .

LDFLAGS_BOOT 	:= $(LDFLAGS) --specs=nano.specs -specs=nosys.specs \
        			-l$(LIBNAME) -Wl,--defsym,_stack=0x20005000 \
        			-Wl,-T,$(LDSCRIPT) -nostartfiles -lc \
        			-Wl,-Map=mapfile $(ARCH_FLAGS) -Wl,-gc-sections \
        			-L libopencm3/lib -u _printf_float
LDFLAGS 		= $(LDFLAGS_BOOT)


SRC				= main.c adc_control.c
DEP				= $(SRC:.c=.d)
OBJ				= $(SRC:.c=.o)

.PHONY: FORCE

all: lib $(BINARY).bin

version.h: FORCE
	$(Q)echo "  CREATE  version.h"
	$(Q)echo "#define FIRMWARE_VERSION \"FW VERSION: $(VERSION)\"" > $@
	$(Q)echo "#define BUILD_TIME \"BUILD TIME: $(DATETIME)\"" >> $@

lib:
	$(Q)$(MAKE) $(MFLAGS) -C libopencm3 TARGETS=stm32/f4

host_clean:
	rm mapfile $(OBJ) $(DEP) version.h
clean: host_clean
	rm $(BINARY).*

$(BINARY).elf: version.h $(OBJ)
	@echo "	LD	$@"
	$(Q)$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c FORCE
	@echo "	CC	$<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

%.bin: %.elf
	@echo "	OBJCOPY	$@"
	$(Q)$(OBJCOPY) -O binary $^ $@

%.hex: %
	@echo "	OBJCOPY $@"
	$(Q)$(OBJCOPY) -O ihex $^ $@

init_flash: $(BINARY).bin
	st-flash --reset write $(BINARY).bin 0x8000000

-include *.d
