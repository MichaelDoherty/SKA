CC = g++
CFLAGS = -c -Wall
INCDIR = -I../include
SRCDIR = ../src
TARGETDIR = ../lib
TARGET = libska.a

SOURCES = \
Blender.cpp \
Bone.cpp \
MotionSequence.cpp \
MotionSequenceController.cpp \
MultiSequenceController.cpp \
RawMotionController.cpp \
Skeleton.cpp \
Camera.cpp \
SystemLog.cpp \
SystemTimer.cpp \
AMC_Reader.cpp \
AMC_Writer.cpp \
ASF_Reader.cpp \
ASF_Writer.cpp \
BVH_Reader.cpp \
DataManager.cpp \
FileSystem.cpp \
ParsingUtilities.cpp \
SKM_ReaderWriter.cpp \
SKS_ReaderWriter.cpp \
EasyBMP.cpp \
Graphics2D.cpp \
GraphicsInterface.cpp \
Lights.cpp \
Textures.cpp \
InputFilter.cpp \
InputManager.cpp \
Matrix4x4.cpp \
Point2D.cpp \
Quaternion.cpp \
RandomGenerator.cpp \
Vector3D.cpp \
CodedModels.cpp \
ModelFactory.cpp \
SphereModels.cpp \
BoneSequence.cpp \
Object.cpp \
QObject.cpp \
Rotator.cpp \
FFT.cpp \
Signals.cpp 

OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	ar cq $(TARGET) *.o
	cp $(TARGET) $(TARGETDIR)

%.o : $(SRCDIR)/Animation/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
%.o : $(SRCDIR)/Camera/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
%.o : $(SRCDIR)/Core/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
%.o : $(SRCDIR)/DataManagement/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
%.o : $(SRCDIR)/EasyBMP/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
%.o : $(SRCDIR)/Graphics/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
%.o : $(SRCDIR)/Input/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
%.o : $(SRCDIR)/Math/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
%.o : $(SRCDIR)/Models/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
%.o : $(SRCDIR)/Objects/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
%.o : $(SRCDIR)/Signals/%.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@

clean:
	-rm $(TARGET)
	-rm *.o
	-rm *~
