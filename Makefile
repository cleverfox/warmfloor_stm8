VERSION_SIZE=64
VERSION_ADDR=0xFFC0
CC=/opt/sdcc/bin/sdcc
CFLAGS=-c -mstm8 -DSTM8S103 -DVERSION_SIZE=$(VERSION_SIZE) -DVERSION_ADDR=$(VERSION_ADDR) -I inc --opt-code-size
LDFLAGS=-mstm8 -L src #stm8s_minilib.lib
SOURCES=modbus_slave.c floor.c eeprom.c src/stm8s_tim2.c src/stm8s_uart1_mini.c lib.c
#src/stm8s_flash.c
# rx_ringbuffer.c statistics.c eeprom.c vs1053.c uid.c cmd.c 
OBJECTS=$(SOURCES:.c=.rel)
OBJECTS_LINK=$(SOURCES:.c=.rel)
EXECUTABLE=out.ihx
EXECUTABLE_HEX=out.hex
EXECUTABLE_BIN=out.bin
HEX_N_PATCH=test.hex
BIN_N_PATCH=test.bin
VERSION_INFO=version.txt

#all: stm8minilib $(SOURCES) $(EXECUTABLE)
all: $(SOURCES) $(EXECUTABLE)

remmacinstall:
	ssh viruzzz@joule.cleverfox.ru 'cd stm8/floorheater; make'
	scp viruzzz@joule.cleverfox.ru:stm8/floorheater/out.bin .
	scp out.bin root@100.64.100.2:
	ssh root@100.64.100.2 stm8flash -c stlinkv2 -p stm8s103 -w out.bin
	

macinstall: all
	scp out.bin root@100.64.100.2:
	ssh root@100.64.100.2 stm8flash -c stlinkv2 -p stm8s103 -w out.bin

install: all
	sudo /home/viruzzz/bin/stm8flash -c stlinkv2 -p stm8s103 -w out.bin
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS_LINK) -o $@
	packihx $(EXECUTABLE) > $(EXECUTABLE_HEX)
	sdobjcopy -I ihex -O binary $(EXECUTABLE_HEX) $(EXECUTABLE_BIN)

stm8minilib:
	make -C src

%.rel: %.c
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -f *.rst *.rel *.lst *.ihx *.sym *.asm *.lk *.map $(EXECUTABLE)

