CC := arm-vita-eabi-gcc
STRIP := arm-vita-eabi-strip

PROJECT := hello_world
OBJS := main.o

all: $(PROJECT).velf

$(PROJECT).velf: $(PROJECT).elf
	$(STRIP) -g $<
	vita-elf-create $< $@

$(PROJECT).elf: $(OBJS)
	$(CC) -Wl,-q -o $@ $^

clean:
	-rm -f $(PROJECT).velf $(PROJECT).elf $(OBJS)
