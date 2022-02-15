###################################################
# definitions                                     #
###################################################
CC = g++
EXEC = zeitkatze 
SRC_DIR = src
# make sure submodules (if they exist) in the include dir are included too
INCL_DIR = include 
CFLAGS = -I$(INCL_DIR) -O3 -std=c++14 -Wall
LDFLAGS = -lstdc++fs
SOURCES = $(wildcard $(SRC_DIR)/*cpp) \
	main.cpp	
OBJECTS = $(SOURCES:%.cpp=%.o)
RM = rm -rf

###################################################
# project development                             #
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

###################################################
# release                                         #
###################################################
PREFIX = /usr

.PHONY: install
install: $(EXEC) 
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/$(EXEC)

.PHONY: uninstall
uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(EXEC)
