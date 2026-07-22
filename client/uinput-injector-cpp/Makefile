CC = g++
CFLAGS = -Wall -O2
TARGET = uinput_injector

all: $(TARGET)

$(TARGET): uinput_injector.cpp
	$(CC) $(CFLAGS) -o $(TARGET) uinput_injector.cpp

clean:
	rm -f $(TARGET)
