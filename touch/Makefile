CFLAGS += -I../common
STUBS := Display Touch

#no need to modify the following

PROJECT ?= $(shell basename $(CURDIR))
TITLEID ?= VSDK00001
ASSETS ?= assets
PHONY := all package clean
all: package
package: $(PROJECT).vpk
clean:
	rm -rf $(PROJECT).*
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
$(PROJECT).vpk: $(PROJECT).sfo $(PROJECT).fself
	vita-pack-vpk -s $< -b $(word 2,$^) $(foreach A,$(call rwildcard,$(ASSETS),*.*), --add $A=$(A:$(ASSETS)/%=%)) $@
$(PROJECT).fself: $(PROJECT).velf
	vita-make-fself -c -s $< $@
$(PROJECT).sfo:
	vita-mksfoex -s TITLE_ID="$(TITLEID)" "$(PROJECT)" $@
$(PROJECT).velf: $(PROJECT).elf
	vita-elf-create $< $@
$(PROJECT).elf: $(wildcard src/*.c)
	arm-vita-eabi-gcc -Wl,-q $(CFLAGS) -o $@ $^ $(LIBS) $(patsubst %,-lSce%_stub,$(STUBS)) $(patsubst %,-lSce%_stub_weak,$(STUBS_WEAK))
