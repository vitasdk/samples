CFLAGS += -I../common
STUBS := Display Ctrl Motion

#no need to modify the following

PROJECT := $(shell basename $(CURDIR))
PHONY := all package clean
all: package
package: $(PROJECT).vpk
clean:
	rm -rf $(PROJECT).*

$(PROJECT).vpk: $(PROJECT).sfo $(PROJECT).fself
	vita-pack-vpk -s $< -b $(word 2,$^) $(foreach A,$(wildcard sce_sys/*.*) $(wildcard sce_sys/*/*/*.*),--add $A=$A) $@
$(PROJECT).fself: $(PROJECT).velf
	vita-make-fself $< $@
$(PROJECT).sfo:
	vita-mksfoex -s TITLE_ID="VSDK00001" "$(PROJECT)" $@
$(PROJECT).velf: $(PROJECT).elf
	arm-vita-eabi-strip -g $<
	vita-elf-create $< $@
$(PROJECT).elf: $(wildcard src/*.c)
	arm-vita-eabi-gcc -Wl,-q $(CFLAGS) -o $@ $^ $(LIBS) $(patsubst %,-lSce%_stub,$(STUBS))
