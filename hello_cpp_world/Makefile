CC := arm-vita-eabi-gcc
CXX := arm-vita-eabi-g++
STRIP := arm-vita-eabi-strip

PROJECT := hello_cpp_world
CXXFLAGS += -std=c++11
OBJS := main.o

all: $(PROJECT).velf

$(PROJECT).velf: $(PROJECT).elf
	$(STRIP) -g $<
	vita-elf-create $< $@

$(PROJECT).elf: $(OBJS)
	$(CXX) -Wl,-q -o $@ $^

clean:
	-rm -f $(PROJECT).velf $(PROJECT).elf $(OBJS)
