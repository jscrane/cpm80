t ?= esp32
p ?= emu8080

TERMINAL_SPEED := 115200
TERMINAL_EXTRA_FLAGS := -C serialout.txt
CPPFLAGS = -DDEBUGGING=0x00 -DCPU_DEBUG=false -DUNDOCUMENTED_OPS
CPPFLAGS += -DUSE_OWN_KBD
LIBRARIES = PS2KeyAdvanced PS2KeyMap SimpleTimer
FS_DIR := disks/$p
SD_DIR := \"/$p/\"

ifeq ($t, rp2040)
BOARD := adafruit_feather_dvi
FLASH := 8388608_2097152
CPPFLAGS += -DHARDWARE_H=\"hw/adafruit_feather_dvi.h\"
#CPPFLAGS += -DHARDWARE_H=\"hw/adafruit_feather_dvi.h\" -DPS2_SERIAL_KBD=\"UK\"
CPPFLAGS += -DDVI_BIT_DEPTH=1 -DDVI_RESOLUTION=DVI_RES_640x240p60
LIBRARIES += LittleFS PicoDVI Adafruit_GFX Adafruit_BusIO Wire
endif

ifeq ($t, tivac)
BOARD := EK-LM4F120XL
CPPFLAGS += -DHARDWARE_H=\"hw/stellarpad-example.h\" -DPS2_SERIAL_KBD=\"UK\"
LIBRARIES += UTFT SD SpiRAM
endif

ifeq ($t, esp8266)
BOARD := d1_mini
BAUD := 921600
EESZ := 4M1M
F_CPU := 80
CPPFLAGS += -DUSER_SETUP_LOADED -DILI9341_DRIVER -DTFT_CS=PIN_D8 -DTFT_DC=PIN_D1 \
	-DTFT_RST=-1 -DSPI_FREQUENCY=40000000 -DLOAD_GLCD -DLOAD_GFXFF \
	-DHARDWARE_H=\"hw/esp8bit.h\" -DPS2_SERIAL_KBD=\"UK\"
LIBRARIES += TFT_eSPI SpiRAM FS LittleFS
endif

ifeq ($t, esp32)
UPLOADSPEED := 921600
TERMINAL_SPEED = 115200
CPPFLAGS += -DUSE_Z80
LIBRARIES += FS Network

ifeq ($b, lilygo)
BOARD := ttgo-t7-v14-mini32
SERIAL_PORT := /dev/ttyACM0
CPPFLAGS += -DPS2_SERIAL_KBD=\"UK\" -DHARDWARE_H=\"hw/ttgo-t7-v14-mini32.h\" -DVGA_BIT_DEPTH=1 -DVGA_RESOLUTION=VGAMode::MODE640x350
LIBRARIES += SPIFFS ESP32Lib

else
ifeq ($b, wrover)
BOARD := esp32wrover
else
BOARD := lolin32
endif

CPPFLAGS += -DUSER_SETUP_LOADED -DILI9341_DRIVER -DTFT_CS=5 -DTFT_DC=2 -DTFT_RST=-1 -DSPI_FREQUENCY=40000000 -DLOAD_GLCD \
	-DHARDWARE_H=\"hw/esp32-example.h\" -DUSE_SD -DPROGRAMS=$(SD_DIR)
LIBRARIES += SD TFT_eSPI
endif
endif

include $t.mk
