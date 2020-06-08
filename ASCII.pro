OBJECTS_DIR = ./obj

SOURCES += \
        main.cpp \
	    src/Options.cpp \
		src/RichText.cpp 

HEADERS += \
		src/Options.hpp \
		src/RichText.hpp

LIBS += -lboost_program_options -L/usr/lib -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-system -lsfml-window
