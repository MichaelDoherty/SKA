TARGET = app1001
CC = g++
CFLAGS = -c -Wall
SKAROOT = ../../SKA
SKAINCDIR = -I$(SKAROOT)/include
SKALIBDIR = -L$(SKAROOT)/lib
SKALIB = -lska
GLLIBS = -lglut -lGLU -lGL
BOOSTLIBS = -lboost_system -lboost_filesystem

SOURCES = AppMain.cpp AnimationControl.cpp CameraControl.cpp InputProcessing.cpp Connector.cpp DataInput.cpp MotionGraph.cpp MotionGraphController.cpp testclass.cpp

OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(SKALIBDIR) $(SKALIB) $(GLLIBS) $(BOOSTLIBS) -o $(TARGET)

%.o : %.cpp
	$(CC) $(CFLAGS) $(SKAINCDIR) $< -o $@

clean:
	-rm $(TARGET)
	-rm *.o
	-rm *~
	-rm system_log.txt
	-rm GRAPHOUTPUT.dot
	-rm test.txt
