
call "x86_64-w64-mingw32-g++" %* -g -Wall -std=c++11 -mms-bitfields -m64 -march=x86-64 -I./boost_1.62.0/include -I./vips-dev-w64-all-8.4.1/include -I./vips-dev-w64-all-8.4.1/include/glib-2.0 -I./vips-dev-w64-all-8.4.1/lib/glib-2.0/include src/*.cpp test/*.cpp -L./boost_1.62.0/lib-w64 -L./vips-dev-w64-all-8.4.1/lib -lvips-cpp -lvips -lgsf-1 -lz -ljpeg -lxml2 -lfftw3 -lm -lMagickWand-6.Q16 -llcms2 -lopenslide -ltiff -lpng16 -lexif -lMagickCore-6.Q16 -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lintl -lboost_program_options-mgw62-mt-1_62 -lboost_filesystem-mgw62-mt-1_62 -lboost_system-mgw62-mt-1_62 -lboost_unit_test_framework-mgw62-mt-1_62 -o target/test.exe
:: added -lboost_unit_test_framework-mgw62-mt-1_62
::  -lpangoft2-1.0 -lcfitsio
