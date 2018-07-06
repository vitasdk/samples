# vitasdk code samples

## Prerequisites

In order to build a vita sample, you need to add the toolchain `bin/` directory to your `$PATH`.

## Building

Every samples directory should include a CMake list.
To build a sample, place yourself into this directory and use the `cmake . && make` command to build it.

## Running

To run a sample:
- Send the generated `.vpk` to your vita:
	- Start an FTP server on your vita (for example, with VitaShell - by pressing the select button).
	- Upload the `.vpk` to the Vita using your FTP client (for example, with Curl - with `curl -T *.vpk ftp://YOUR_VITA_IP:1337/ux0:/`)
	- If Curl returns `fatal: No names found, cannot describe anything` it mean that you are trying to overwrite a folder with a file. Add a `/` to the end of your url to explain that you want to upload IN this folder.
- Install the `.vpk` on your vita using a vpk installer (for example, with VitaShell - by pressing the X button on the `.vpk`)
- This will create a new folder in the `ux0:/app/` for the sample.

## Building everything

Use the following command to build every samples:

```
mkdir build && cd build
cmake ..
make
```

## List of samples

* `audio`: Simple audio wave generator.
* `camera`: Demonstration of camera features.
* `common`: Common functions for samples.
* `ctrl`: A minimal controller (button) sample.
* `debug_print`: A minimal debug print sample.
* `debugscreen`: Debug text printing sample.
* `hello_cpp_world`: A minimal hello world sample in C++.
* `hello_world`: A minimal hello world sample.
* `ime`: Graphical dialog sample.
* `microphone`: Demonstration of microphone features.
* `motion`: Prints accelerometer data.
* `net_http`: A minimal HTTP download sample.
* `net_http_bsd`: A minimal HTTP download sample using BSD sockets.
* `net_libcurl`: A libcurl download sample.
* `power`: A minimal power sample.
* `pretty_livearea`: A minimal hello world sample with example livearea styling and features.
* `prx_loader`: Load/list prx modules.
* `prx_simple`: Minimal sample prx module.
* `redrectangle`: Example SDL rendering.
* `rtc`: A minimal RTC sample.
* `socket_ping`: ICMP ping using raw sockets.
* `soloud`: Plays an audio file and Text To Speech.
* `touch`: A minimal touch sample.

## Notes on images
- Images shall use indexed palettes (PNG-8 128 Dithered).
- The size of an image shall not exceed 420KB.
- For some reasons, some PNG files created by GIMP makes the .vpk installation crash.
- You can further minimize overhead by running your images through [pngquant](https://pngquant.org/).

## Notes on supporting files and folders
- File names shall not exceed 32B.
- Directory names shall not exceed 16B.
- Folder creation shall not exceed one level.

## Notes on XML
- UTF-8 character encoding, CRLF line termination.
- File size shall not exceed 32KB.
- Different visual styles are available, check the sample `pretty_livearea` for an example.

## License

All code and build scripts in this repo is licensed under the terms of [CC0 1.0 Universal](https://creativecommons.org/publicdomain/zero/1.0/).
