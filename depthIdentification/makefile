SOURCE  := main.cc
SOURCE  += $(shell find include/ -name "*.cc")
CC      := g++
FLAGS   := -std=c++17 -w -I include
LD      := -lboost_program_options #-ltbb -lpthread
TARGET  := conn_depth
OUT	:= out

all: $(TARGET)

debug: $(SOURCE)
	$(CC) -g -O0 $(FLAGS) $(SOURCE) -o $(TARGET) $(LD)

$(TARGET): $(SOURCE)
	$(CC) -O3 $(FLAGS) $(SOURCE) -o $(TARGET) $(LD)

mefficient: $(SOURCE)
	$(CC) -g -O2 $(FLAGS) $(SOURCE) -o $(TARGET) $(LD)

clean:
	rm -f $(TARGET)
