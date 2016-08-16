# vitasdk code samples

## Prerequisites

In order to build a vita sample, you need to add the toolchain `bin/` directory to your `$PATH`.

## Building

Every samples directories should contains a README file and a Makefile.
To build a sample, place yourself into this directory and use the `make` command to build it.

## Running

To run a sample:
- send the generated `.vpk` to your vita :
	- start an FTP server on your vita (for example with VitaShell by pressing the select button)
	- upload the vpk to the vita using Curl (for example `curl -T *.vpk ftp://192.168.12.34:1337/ux0:/`)
	- if curl return `fatal: No names found, cannot describe anything` it mean that you are trying to overwrite a folder with a file, add a `/` to the end of your url to explain that you want to upload IN this folder
- install the .vpk on your vita using a vpk installer (for example using the VitaShell by pressing the X button on the .vpk)
- this will create a new folder in the `ux0:/app/` folder (it name depend on the .vpk)
- for further upload, you can directly skip the .vpk upload/install part, and directly re-install your binary (eboot.bin) inside the created /app/ folder

## Building everything

Use the following command to build every samples

```
for f in */Makefile; do make -C ${f%/*} all; done
```

## Notes
- icon0.png, startup.png and bg.png must be using indexed palettes.
- For some reasons, some PNG files created by GIMP makes the .vpk installation crash.

## License

All code and build scripts in this repo is licensed under the terms of [CC0 1.0 Universal](https://creativecommons.org/publicdomain/zero/1.0/).
