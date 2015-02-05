TARGET = app0005
CC = g++
CFLAGS = -c -Wall
SKAROOT = ../../SKA
SKAINCDIR = -I$(SKAROOT)/include
SKALIBDIR = -L$(SKAROOT)/lib
SKALIB = -lska
GLLIBS = -lglut -lGLU -lGL

SOURCES = AnimationControl.cpp CameraControl.cpp InputProcessing.cpp AppMain.cpp 

OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(SKALIBDIR) $(SKALIB) $(GLLIBS) -o $(TARGET)

%.o : %.cpp
	$(CC) $(CFLAGS) $(SKAINCDIR) $< -o $@

clean:
	-rm $(TARGET)
	-rm *.o
	-rm *~
	-rm system_log.txt
