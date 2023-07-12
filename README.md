![](https://raw.githubusercontent.com/SuperFromND/evelyn/master/res/logo.svg)
---
***Evelyn*** is a tool for creating dummy PlayStation 1 save files with custom icons. It was created specifically for the [PocketStation](https://en.wikipedia.org/wiki/PocketStation) peripheral, which has the ability to display save icons on a black-and-white LCD.

## Usage
Evelyn is a command line tool that takes a single PNG file as input. Preferrably, this PNG should be 16 x 16 and only contain the colors black and white.
```bash
> evelyn.exe input.png
```
Evelyn will output an extensionless, 8KB (8192 bytes) save file. This file can then be either copied to an actual PlayStation memory card using [uLaunchELF/wLaunchELF](https://github.com/ps2homebrew/wLaunchELF) or similar methods, or converted to a digital memory card using [a tool such as this website](https://savefileconverter.com/#/ps1/emulator) for viewing in emulators like [DuckStation](https://www.duckstation.org/). The actual save file's data itself is blank and shouldn't be recognized by any commercial PlayStation games.

### Limitations
Partially due to the nature of the PocketStation and partially due to frustration with figuring out how color palettes are stored and encoded, Evelyn currently only outputs ___black and white___ icons. It would be nice to support proper colors in the future, but this is by far the easiest to support for now.

## Building
Evelyn should be able to compile on pretty much anything that supports SDL 2.0 or later. Assuming you already have both a working C/C++ compiler and SDL2 installed, you should be able to compile pretty easily:
```
git clone https://github.com/SuperFromND/evelyn.git
cd evelyn
make
make install // Windows users only!
```
Note that this tool has only been tested on an x86 Windows machine; other OSes and architectures might not necessarily work!

## Licensing
Evelyn's source code, as well as its iconography, are both [available under the MIT License.](https://raw.githubusercontent.com/SuperFromND/evelyn/main/LICENSE) <3