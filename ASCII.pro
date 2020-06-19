OBJECTS_DIR = ./obj

#DEFINES += MAGICKCORE_HDRI_ENABLE=0
DEFINES += MAGICKCORE_QUANTUM_DEPTH=16

SOURCES += \
        main.cpp \
		src/Functions.cpp \
	    src/Options.cpp

HEADERS += \
		main.hpp \
		src/Functions.hpp \
		src/Options.hpp
		
INCLUDEPATH += /home/dev38/Biblioteki/ImageMagick/Magick++/lib/
INCLUDEPATH += /home/dev38/Biblioteki/ImageMagick/
INCLUDEPATH += /usr/include/ImageMagick-7/

LIBS += -lboost_program_options -lboost_filesystem -ljpeg -lpng -lMagick++-7.Q16HDRI
