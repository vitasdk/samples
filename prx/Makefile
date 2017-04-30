TARGET      = example
TYPE        = game #system etc

BASE_LIBS   = -lSceKernel_stub -lSceDisplay_stub
LIBS        = $(BASE_LIBS)

SRCS        = example.c
OBJS        = $(patsubst %.c,%.o, $(SRCS))

CC          = arm-vita-eabi-gcc
CXX         = arm-vita-eabi-g++
AR          = arm-vita-eabi-ar
DEPFLAGS    = -MT $@ -MMD -MP -MF $*.d
INCLUDES    =

CFLAGS      = -Wl,-q $(DEPFLAGS) $(INCLUDES) -Wall -Werror -pedantic -O3 -std=c11   -nostartfiles
CXXFLAGS    = -Wl,-q $(DEPFLAGS) $(INCLUDES) -Wall -Werror -pedantic -O3 -std=c++0x -nostartfiles
ASFLAGS     = $(CFLAGS)

all: $(TARGET).suprx

$(TARGET).suprx: $(TARGET).velf
	vita-make-fself $< $@

%.velf: %.elf
	vita-elf-create $< $@

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -rf $(TARGET).vpk $(TARGET).velf $(TARGET).elf $(OBJS)

send: $(TARGET).suprx
	curl --ftp-create-dirs -T $(TARGET).suprx ftp://$(PSVITAIP):1337/ux0:/prx/$(TYPE)/
	@echo "Sent."

.PHONY: send clean
