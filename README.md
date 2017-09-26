# Atlasbuilder

Command line 2D png texture atlas generator. Uses MaxRects algorithm code borrowed from https://github.com/juj/RectangleBinPack
Use libvips for image processing as it's crazy fast https://github.com/jcupitt/libvips

## usage

```
usage: atlasbuilder [options] <input files...>
example: atlasbuilder --output=myoutputname input1.png input2.png

Allowed options:
  --help                        produce help message
  --version                     display version
  --response-file arg           read in command line options from file
                                (separated by newlines). Can be specified with
                                '@name', too
  --input-files arg             input file(s)
  --resolution arg              resolution to output (default is 1.0)
  --output arg                  set output name
  --output-width arg            set maximum output image width
  --output-height arg           set maximum output image height
  --fail-if-too-big arg         fail build if the output-width and
                                output-height will be exceeded
  --rotation-enabled arg        enable rotation on subimages
  --trim-enabled arg            enable trim on subimages
  --padding arg                 padding to insert between each sub image in
                                atlas
  --boundary-alignment arg      boundary-alignment to align each sub image in
                                atlas
  --scale-manifest-values arg   if using a resolution other than 1, use scaled
                                x,y,w,h values in the manifest
  --trim-boundary-alignment arg boundary-alignment to align when trimming
```

## building on windows

Download mingw-w64 (Get version 4.0.6: https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/)

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
run build-mingw-release.cmd or ming32-make win32-release


## building on Mac

You'll need to use Homebrew (or other package manager) to install the boost dependency. We've used Homebrew in our case.

```
brew install boost
```

Make sure you have XCode installed and updated. If you don't, after the installation is completed, you'll need to open XCode once, accept the Terms & Conditions and then the building packages will be installed

After getting everything installed, just run

```
make mac-release
```


## building on linux

```
sudo add-apt-repository ppa:dhor/myway
sudo add-apt-repository ppa:boost-latest/ppa
sudo apt-get update
sudo apt-get install libvips-dev libboost-program-options-dev libboost-system-dev libboost-filesystem-dev

make linux-release
```

## running tests

```
make linux-test
target-linux/test
```

```
ming32-make win32-test
target/test.exe
```

## build .deb file

```
make deb
```

