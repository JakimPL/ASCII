OBJECTS_DIR = ./obj

DEFINES += MAGICKCORE_HDRI_ENABLE 
DEFINES += MAGICKCORE_QUANTUM_DEPTH=16
DEFINES += LOG_LEVEL=2

SOURCES += \
        main.cpp \
		src/Functions.cpp \
	src/ImageCell.cpp \
		src/ImageData.cpp \
		src/Log.cpp \
	    src/Options.cpp

HEADERS += \
		main.hpp \
		src/Functions.hpp \
		src/ImageCell.hpp \
		src/ImageData.hpp \
		src/Log.hpp \
		src/Options.hpp \
		src/Types.hpp
		
INCLUDEPATH += /home/dev38/Biblioteki/ImageMagick/Magick++/lib/
INCLUDEPATH += /home/dev38/Biblioteki/ImageMagick/
INCLUDEPATH += /usr/include/ImageMagick-7/

LIBS += -lboost_program_options -lboost_filesystem -ljpeg -lpng -lMagick++-7.Q16HDRI

QMAKE_CXXFLAGS+=-Wno-ignored-qualifiers
