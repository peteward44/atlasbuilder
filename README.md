# Atlasbuilder

Command line 2D png texture atlas generator. Uses MaxRects algorithm code borrowed from https://github.com/juj/RectangleBinPack
Use libvips for image processing as it's crazy fast https://github.com/jcupitt/libvips


## building on windows

Download mingw-w64

During install:
Architecture: x86_64
threads: posix
exceptions: seh

Run "mingw32 Installation Manager"

mingw32-gcc-g++
mingw32-gmp
mingw32-pthreads-w32

Installation -> Apply changes

Add C:\Program Files\mingw-w64\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin to PATH
reboot

## building on linux

```
sudo add-apt-repository ppa:dhor/myway
sudo add-apt-repository ppa:boost-latest/ppa
sudo apt-get update
sudo apt-get install libvips-dev libboost-program-options-dev libboost-system-dev libboost-filesystem-dev
./build-linux.sh
```
