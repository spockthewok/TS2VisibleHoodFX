CC = cl
CFLAGS = /LD /EHsc /MD /Zi /GL /D WIN32
LDFLAGS = /link /DEBUG
TARGET = TS2VisibleHoodFX.asi

all: $(TARGET)

$(TARGET): *.obj
    $(CC) $(CFLAGS) *.obj /Fe:$(TARGET) $(LDFLAGS)

.cpp.obj:
    $(CC) $(CFLAGS) /c $<

clean:
    del /Q *.obj *.pdb *.ilk $(TARGET)