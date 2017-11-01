WIN32_GCC=x86_64-w64-mingw32-g++
LINUX_GCC=g++
RELEASE_FLAGS=-O3 -s
SHARED_FLAGS=-g -Wall -std=c++11
CPP_FILES=main.cpp src/*.cpp
TEST_CPP_FILES=src/*.cpp test/*.cpp

WIN32_INCLUDE=-mms-bitfields -m64 -march=x86-64 -I./boost_1.62.0/include -I./vips-dev-w64-all-8.4.1/include -I./vips-dev-w64-all-8.4.1/include/glib-2.0 -I./vips-dev-w64-all-8.4.1/lib/glib-2.0/include
WIN32_LIBS=-L./boost_1.62.0/lib-w64 -L./vips-dev-w64-all-8.4.1/lib -lvips-cpp -lvips -lgsf-1 -lz -ljpeg -lxml2 -lfftw3 -lm -lMagickWand-6.Q16 -llcms2 -lopenslide -ltiff -lpng16 -lexif -lMagickCore-6.Q16 -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lintl -lboost_program_options-mgw62-mt-1_62 -lboost_filesystem-mgw62-mt-1_62 -lboost_system-mgw62-mt-1_62
WIN32_FLAGS=$(SHARED_FLAGS) $(WIN32_INCLUDE) $(CPP_FILES) $(WIN32_LIBS)
WIN32_TEST_FLAGS=$(SHARED_FLAGS) $(WIN32_INCLUDE) $(TEST_CPP_FILES) $(WIN32_LIBS)
LINUX_FLAGS=$(SHARED_FLAGS) $(CPP_FILES) `pkg-config vips-cpp --cflags --libs` -lboost_program_options -lboost_filesystem -lboost_system

mac-release :
	mkdir -p target-mac
	$(LINUX_GCC) $(RELEASE_FLAGS) $(LINUX_FLAGS) -I/usr/local/include -o target-mac/atlasbuilder -pedantic

linux-release :
	mkdir -p target-linux
	$(LINUX_GCC) $(RELEASE_FLAGS) $(LINUX_FLAGS) -o target-linux/atlasbuilder

linux :
	mkdir -p target-linux
	$(LINUX_GCC) $(LINUX_FLAGS) -o target-linux/atlasbuilder

linux-test:
	$(LINUX_GCC) $(LINUX_FLAGS) -lboost_unit_test_framework -o target-linux/test

win32-release :
	$(WIN32_GCC) $(RELEASE_FLAGS) $(WIN32_FLAGS) -DWIN32 -o target/atlasbuilder.exe

win32 :
	$(WIN32_GCC) $(WIN32_FLAGS) -DWIN32 -o target/atlasbuilder.exe
	
win32-test :
	$(WIN32_GCC) $(WIN32_TEST_FLAGS) -DWIN32 -lboost_unit_test_framework-mgw62-mt-1_62 -o target/test.exe

install :
	mkdir -p /usr/bin
	cp -f target-linux/atlasbuilder /usr/bin/atlasbuilder

deb :
	checkinstall -D \
		--fstrans=yes \
		--pkgsource="https://github.com/peteward44/atlasbuilder/" \
		--pkglicense="MIT" \
		--deldesc=no \
		--nodoc \
		--maintainer="Pete Ward \\<peteward44@gmail.com\\>" \
		--pkgarch=`dpkg --print-architecture` \
		--pkgversion="1.1.0" \
		--pkgrelease="SNAPSHOT" \
		--pkgname=atlasbuilder \
		--requires="libc6 \(\>= 2.4\),libgcc1 \(\>= 1:4.1.1\),libstdc++6 \(\>= 4.1.1\),libvips42 \(\>= 8.2.0\)"

