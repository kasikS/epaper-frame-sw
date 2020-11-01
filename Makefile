BINARY = ramka
DEVICE = stm32l152r8
OBJS   = delay.o serial.o sdcard.o

LIBNAME = opencm3_stm32l1
DEFS    += -DSTM32L1

#FP_FLAGS        ?= -msoft-float
#ARCH_FLAGS      = -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd

OOCD            ?= openocd
OOCD_INTERFACE  ?= stlink-v2
OOCD_TARGET     ?= stm32l1

include rules.mk
