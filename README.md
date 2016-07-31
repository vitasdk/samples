# vitasdk code samples

Every directory contains a single sample. To build all samples, add the toolchain `bin/` directory to your `$PATH`, then run `./build.sh`. To clean all samples, run `./clean.sh`. To build a single sample, `cd` to its directory and type `make`.

Always use the `*_debug.bat` (e.g. `run_homebrew_unity_debug.bat`) launcher to get debug output from Vita.

## List of samples

* `hello_world`: A minimal Makefile-based C project
* `hello_cpp_world`: A minimal Makefile-based C++ project

## Notes
- icon0.png, startup.png and bg.png must be using indexed palettes.
- For some reasons, some PNG files created by GIMP makes the .vpk installation crash.

## License

All code and build scripts in this repo is licensed under the terms of [CC0 1.0 Universal](https://creativecommons.org/publicdomain/zero/1.0/).
