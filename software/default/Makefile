###############################################################################
# Makefile for the project rgb
###############################################################################

## General Flags
PROJECT = rgb
# MCU = atmega168
MCU = attiny2313
TARGET = rgb.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -std=gnu99 -Wall -gdwarf-2  -Os -fsigned-char
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += 

## Programmer Options
PROGRAMMER     = usbtiny	#	Must be updated with the type of programmer that you use!
AVRDUDE_TARGET = t2313
#AVRDUDE_TARGET = atmega168
PORT		   = usb

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = rgb.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) rgb.hex rgb.eep size

## Compile
rgb.o: ../rgb.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) rgb.elf dep/* rgb.hex rgb.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

## Rules for building the .eeprom rom images

eeprom: ehex ebin esrec


ehex:  $(PROJECT)_eeprom.hex
#ebin:  $(PROJECT)_eeprom.bin
esrec: $(PROJECT)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

#%_eeprom.srec: %.elf
#	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@


## command to program chip (invoked by running "make program")
program:
	avrdude -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P $(PORT) -v -e -b 115200  \
	 -U flash:w:$(PROJECT).hex
#	 -U lfuse:w:0x64:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
#   ATtiny2313 Default

# -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 512 kHz internal
# -U lfuse:w:0xe4:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m  8 MHz Internal

#	 -U lfuse:w:0x64:m  -U hfuse:w:0xDB:m \
#	 -U efuse:w:0xff:m -U eeprom:w:$(PROJECT)_eeprom.hex  \

# Original fuse settings:
#	 -U lfuse:w:0x64:m  -U hfuse:w:0xDF:m	-U efuse:w:0xff:m
