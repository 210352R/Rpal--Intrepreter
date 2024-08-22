# make file for run parser.cpp

CC = g++
CFLAGS = -Wall -g
TARGET = myrpal
OBJS = parser.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

parser.o: parser.cpp
	$(CC) $(CFLAGS) -c parser.cpp
				
clean:
	rm -f $(OBJS) $(TARGET)