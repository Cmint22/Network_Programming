CC = gcc
CFLAGS = -O2 -Wall

TARGET = resolver

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

OBJECTS = resolver.o

resolver.o: resolver.c
	$(CC) $(CFLAGS) -c resolver.c

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJECTS)
