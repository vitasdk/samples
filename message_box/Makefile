# You should only use Makefile-based build if you know what you're doing.
# For most vitasdk projects, CMake is a better choice. See CMakeLists.txt for an example.

PHONY := all package clean
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

CC := arm-vita-eabi-gcc
CXX := arm-vita-eabi-g++
STRIP := arm-vita-eabi-strip

PROJECT_TITLE := message_box_sample
PROJECT_TITLEID := VSDK00024

PROJECT := message_box_sample
CFLAGS += -Wl,-q -I../common
CXXFLAGS += -Wl,-q -std=c++11 -I../common

SRC_C :=$(call rwildcard, src/, *.c)
SRC_CPP :=$(call rwildcard, src/, *.cpp)

OBJ_DIRS := $(addprefix out/, $(dir $(SRC_C:src/%.c=%.o))) $(addprefix out/, $(dir $(SRC_CPP:src/%.cpp=%.o)))
OBJS := $(addprefix out/, $(SRC_C:src/%.c=%.o)) $(addprefix out/, $(SRC_CPP:src/%.cpp=%.o))

# Needed by psvDebugScreenPrintf
LIBS += -lSceSysmodule_stub -lSceCommonDialog_stub -lSceAppUtil_stub -lSceGxm_stub -lSceDisplay_stub

all: package

package: $(PROJECT).vpk

$(PROJECT).vpk: eboot.bin param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
	$(PROJECT).vpk

eboot.bin: $(PROJECT).velf
	vita-make-fself $(PROJECT).velf eboot.bin

param.sfo:
	vita-mksfoex -s TITLE_ID="$(PROJECT_TITLEID)" "$(PROJECT_TITLE)" param.sfo

$(PROJECT).velf: $(PROJECT).elf
	$(STRIP) -g $<
	vita-elf-create $< $@

$(PROJECT).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

$(OBJ_DIRS):
	mkdir -p $@

out/%.o : src/%.cpp | $(OBJ_DIRS)
	arm-vita-eabi-g++ -c $(CXXFLAGS) -o $@ $<

out/%.o : src/%.c | $(OBJ_DIRS)
	arm-vita-eabi-gcc -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(PROJECT).velf $(PROJECT).elf $(PROJECT).vpk param.sfo eboot.bin $(OBJS)
	rm -r $(abspath $(OBJ_DIRS))
