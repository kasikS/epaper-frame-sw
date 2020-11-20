BINARY = ramka
DEVICE = stm32l052c6
OBJS   = delay.o serial.o sdcard.o rtc.o epaper.o power.o

LIBNAME = opencm3_stm32l0
DEFS    += -DSTM32L0

#FP_FLAGS        ?= -msoft-float
#ARCH_FLAGS      = -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd

OOCD            ?= openocd
OOCD_INTERFACE  ?= stlink-v2
OOCD_TARGET     ?= stm32l0

include rules.mk
