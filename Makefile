#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!   
#
##############################################################################################
#
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
#
# Included originally in the yagarto projects. Original Author : Michael Fischer
# Modified to suit our purposes by Hussam Al-Hertani
# Use at your own risk!!!!!
##############################################################################################
# Start of default section
#
CCPREFIX = arm-none-eabi-
CC   = $(CCPREFIX)gcc
CP   = $(CCPREFIX)objcopy
AS   = $(CCPREFIX)gcc -x assembler-with-cpp
GDBTUI = $(CCPREFIX)gdbtui
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S
MCU  = cortex-m0
 
# List all C defines here
#DDEFS =
DDEFS = -DSTM32F031
#
# Define project name and Ram/Flash mode here
PROJECT        = cx10
 
# List C source files here

SRC  = ./src/main.c
SRC += ./src/led.c
SRC += ./src/gpio.c
SRC += ./src/spi.c
SRC += ./src/nrf24l01.c
SRC += ./src/rx_bk2423.c
SRC += ./src/misc.c
SRC += ./src/pid.c
SRC += ./src/imu.c
SRC += ./src/flight.c
SRC += ./src/timers.c
SRC += ./src/atan.c

SRC += ./src/stm32f0xx_rcc.c
SRC += ./src/stm32f0xx_gpio.c
SRC += ./src/stm32f0xx_i2c.c
SRC += ./src/system_stm32f0xx.c

SRC += ./cleanflight/drivers/system.c
SRC += ./cleanflight/drivers/bus_i2c_stm32f0xx.c
SRC += ./cleanflight/drivers/accgyro_mpu6050.c

# List assembly startup source file here
STARTUP = ./startup/startup_stm32f031.s
 
# List all include directories here
INCDIRS = ./inc ./cleanflight ./cleanflight/drivers
              
# List the user directory to look for the libraries here
LIBDIRS +=
 
# List all user libraries here
LIBS =
 
# Define optimisation level here
OPT = -Os
 

# Define linker script file here
LINKER_SCRIPT = ./linker/stm32f0_linker.ld

 
INCDIR  = $(patsubst %,-I%, $(INCDIRS))
LIBDIR  = $(patsubst %,-L%, $(LIBDIRS))
#LIB     = $(patsubst %,-l%, $(LIBS))
##reference only flags for run from ram...not used here
##DEFS    = $(DDEFS) $(UDEFS) -DRUN_FROM_FLASH=0 -DVECT_TAB_SRAM

## run from Flash
DEFS    = $(DDEFS) -DRUN_FROM_FLASH=1

OBJS  = $(STARTUP:.s=.o) $(SRC:.c=.o)
MCFLAGS = -mcpu=$(MCU)
 
ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb  -Wa,-amhls=$(<:.s=.lst) 
CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb   -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS) -std=c99
LDFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb -nostartfiles -T$(LINKER_SCRIPT) -Wl,-Map=$(PROJECT).map,--cref,--no-warn-mismatch $(LIBDIR)
 
PWD = `pwd`

#
# makefile rules
#
 
all: $(OBJS) $(PROJECT).elf  $(PROJECT).hex $(PROJECT).bin
	$(TRGT)size $(PROJECT).elf
 
%.o: %.c
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@

%.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

%.elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%.hex: %.elf
	$(HEX) $< $@
	
%.bin: %.elf
	$(BIN)  $< $@
	
flash_openocd: $(PROJECT).bin
	openocd -f ~/src/openocd/stlink-swd.cfg -c "init" -c "reset halt" -c "sleep 100" -c "wait_halt 2" -c "flash write_image erase $(PROJECT).bin 0x08000000" -c "sleep 100" -c "verify_image $(PROJECT).bin 0x08000000" -c "sleep 100" -c "reset run" -c shutdown

flash_stlink: $(PROJECT).bin
	st-flash write $(PROJECT).bin 0x8000000

erase_openocd:
	openocd -f ~/src/openocd/stlink-swd.cfg -c "init" -c "reset halt" -c "sleep 100" -c "stm32f1x mass_erase 0" -c "sleep 100" -c shutdown 

erase_stlink:
	st-flash erase

kill_gdbtui:
	killall $(GDBTUI)

openocd: $(PROJECT).elf flash_openocd
	xterm -e openocd -f ~/src/openocd/stlink-swd.cfg -c "init" -c "halt" -c "reset halt" &

connect_openocd:
	$(GDBTUI) --eval-command="target remote localhost:3333" $(PROJECT).elf 

flash_running: $(PROJECT).bin
	echo -ne "init\nreset halt\nsleep 100\nwait_halt 2\nflash write_image erase $(PWD)/$(PROJECT).bin 0x08000000\nsleep 100\nverify_image $(PWD)/$(PROJECT).bin 0x08000000\nsleep 100\nreset run\n" | nc localhost 4444

halt:
	echo "reset halt" | nc localhost 4444

#debug_openocd: $(PROJECT).elf flash_openocd
#	xterm -e openocd -f ~/src/openocd/stlink-swd.cfg -c "init" -c "halt" -c "reset halt" &
#	$(GDBTUI) --eval-command="target remote localhost:3333" $(PROJECT).elf 

debug_openocd: openocd connect_openocd kill_gdbtui connect_openocd

debug_stlink: $(PROJECT).elf
	xterm -e st-util &
	$(GDBTUI) --eval-command="target remote localhost:4242"  $(PROJECT).elf -ex 'load'
		
clean:
	-rm -rf $(OBJS)
	-rm -rf $(PROJECT).elf
	-rm -rf $(PROJECT).map
	-rm -rf $(PROJECT).hex
	-rm -rf $(PROJECT).bin
	-rm -rf $(SRC:.c=.lst)
	-rm -rf $(ASRC:.s=.lst)
# *** EOF ***
