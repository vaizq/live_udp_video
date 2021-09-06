CC = g++
CC_C = gcc


ODIR := bin
SDIR := src
IDIR := include

SOURCES := $(wildcard $(SDIR)/*.cpp) 
OBJ := $(patsubst $(SDIR)/%,%,$(SOURCES))
OBJ := $(patsubst %.cpp,%.o,$(OBJ))
OBJ := $(addprefix $(ODIR)/,$(OBJ))


LIBS := -lpthread `pkg-config --cflags --libs opencv4`
INCLUDES := -I$(IDIR)
CFLAGS := $(INCLUDES) $(LIBS) -g

all: server client

client : $(SDIR)/client.cpp 
	$(CC) -o $@ $^ $(CFLAGS) 

# Build object files from sources
server : $(SDIR)/server.cpp
	$(CC) -o $@ $^ $(CFLAGS) 


.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(ODIR)/*.out ./*.out
