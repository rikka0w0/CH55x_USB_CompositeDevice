# toolchain
CC = sdcc
OBJCOPY = objcopy
PACK_HEX = packihx

default: all


C_FILES= \
ch554_conf.c  \
main.c \
usb_desc.c \
usb_hid_desc.c \
usb_mal.c \
eeprom.c \
scsi_data.c \
usb_endp.c \
Delay.c \
usb_hid_keyboard.c \
usb_scsi.c \
i2c.c \
usb_bot.c \
usb_ep0.c \
usb_it.c \
usb_string_desc.c \
OLED/sdd1306_ascii.c \
OLED/sdd1306.c

TARGET = usb_k
ifndef FREQ_SYS
FREQ_SYS = 16000000
endif

ifndef XRAM_SIZE
XRAM_SIZE = 0x0400
endif

ifndef XRAM_LOC
XRAM_LOC = 0x0000
endif

ifndef CODE_SIZE
CODE_SIZE = 0x4000
endif

ROOT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
RELS := $(C_FILES:.c=.rel)
LFLAGS := $(CFLAGS)
CFLAGS := -V -mmcs51 --model-large \
	--xram-size $(XRAM_SIZE) --xram-loc $(XRAM_LOC) \
	--code-size $(CODE_SIZE) \
	-I$(ROOT_DIR)../include -DFREQ_SYS=$(FREQ_SYS) \
	$(EXTRA_FLAGS)

$(TARGET).ihx: $(RELS)
	$(CC) $(notdir $(RELS)) $(CFLAGS) -o $(TARGET).ihx

$(TARGET).hex: $(TARGET).ihx
	$(PACK_HEX) $(TARGET).ihx > $(TARGET).hex

$(TARGET).bin: $(TARGET).ihx
	$(OBJCOPY) -I ihex -O binary $(TARGET).ihx $(TARGET).bin

%.rel : %.c
	@echo Compiling $<
	$(CC) -c $(CFLAGS) $<


all: $(TARGET).bin $(TARGET).hex
clean:
	rm -rf *.asm
	rm -rf *.rel
	rm -rf *.sym
	rm -rf *.lst
	rm -rf *.gch
	find . -type f -name '*.gch' -delete
	rm -rf *.rst
	rm -rf *.mem
	rm -rf *.ihx
	rm -rf *.lk
	rm -rf *.bin
	rm -rf *.hex


.PHONY: all, clean
