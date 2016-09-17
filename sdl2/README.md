Requirement
===========

SDL2 taken from https://github.com/vitadev/SDL-Vita

Build SDL2
----------

$ make -f Makefile.vita
then
cp     libSDL2.a         ${VITASDK}/arm-vita-eabi/lib/
cp -Rv include           ${VITASDK}/arm-vita-eabi/include/SDL2

