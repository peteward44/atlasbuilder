# Atlasbuilder

Command line 2D png texture atlas generator. Uses MaxRects algorithm code borrowed from https://github.com/juj/RectangleBinPack
Use libvips for image processing as it's crazy fast https://github.com/jcupitt/libvips

## usage

### example

```atlasbuilder --output=myoutputname input1.png input2.png```

### --help output

```
  atlasbuilder.exe [input-files...] {OPTIONS}

    Atlasbuilder

  OPTIONS:

      -h, --help                        Display this help menu
      --output=[output]                 set output name
      -v, --version                     Output version number
      --output-width=[output-width]     set maximum output image width
      --output-height=[output-height]   set maximum output image height
      Enable or disable rotation:
        --rotation-enabled                Enable sub image rotation
        --rotation-disabled               Disable sub image rotation
      --rotation-anticlockwise          Rotate the sub images anti-clockwise
                                        instead of clockwise
      Enable or disable trim:
        --trim-enabled                    Enable sub image trimming
        --trim-disabled                   Disable sub image trimming
      --fail-if-too-big                 fail build if the output-width and
                                        output-height will be exceeded
      --output-pow2                     final output image should always be a
                                        power of 2
      --scale-manifest-values           if using a resolution other than 1, use
                                        scaled x,y,w,h values in the manifest
      --padding=[padding]               padding to insert between each sub image
                                        in atlas
      --boundary-alignment=[boundary-alignment]
                                        boundary-alignment to align each sub
                                        image in atlas
      --trim-boundary-alignment=[trim-boundary-alignment]
                                        boundary-alignment to align when
                                        trimming
      --resolution=[resolution]         resolution to output (default is 1.0)
      --manifest-format=[manifest-format]
                                        format to output the JSON manifest,
                                        either 'hash', 'array' or 'legacy'
      --no-output-image                 Do not output the final image
      --no-output-json                  Do not output the final json manifest
      --resize-kernel=[resize-kernel]   Algorithm to use when resizing images.
                                        Either 'nearest', 'linear', 'cubic',
                                        'lanczos2' or 'lanczos3'
      --recursive                       When a directory is specified on the
                                        command line, traverse recursively
      input-files...                    Input files to process
      "--" can be used to terminate flag options and force all following
      arguments to be treated as positional options

    2.0.0
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

Make sure you have XCode installed and updated. If you don't, after the installation is completed, you'll need to open XCode once, accept the Terms & Conditions and then the building packages will be installed

After getting everything installed, just run

```
make mac-release
```


## building on linux

```
sudo add-apt-repository ppa:dhor/myway
sudo apt-get update
sudo apt-get install libvips-dev

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
sudo apt-get install checkinstall
make deb
```

