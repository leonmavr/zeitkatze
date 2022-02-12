###################################################
# definitions                                     #
###################################################
CC = g++
EXEC = zeitkatze 
SRC_DIR = src
INCL_DIR = include 
# make sure submodules (if they exist) in the include dir are included too
CFLAGS = -O3 -std=c++17 -I$(INCL_DIR) -Wall
LDFLAGS = 
SOURCES = $(SRC_DIR)/zeitkatze.cpp main.cpp
OBJECTS = $(SOURCES:%.cpp=%.o)
RM = rm -rf

###################################################
# project                                         #
###################################################
all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LDFLAGS) 

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean

clean:
	$(RM) $(OBJECTS) 
	$(RM) $(EXEC)
