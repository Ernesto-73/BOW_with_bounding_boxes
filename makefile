#Author: Javier Zhang
# Date: 12-10-2013     

CXX         = g++
CC          = $(CXX)
CPPFLAGS    = -g -Wall 
CXXFLAGS    = $(INCLUDE) `pkg-config --libs --cflags opencv` 
SOURCES     = ./src/
HEADERS     = ./include/
OBJS        = ./obj/
LIBS        = ./libs/
BIN         = ./bin/
INCLUDE     = -I$(HEADERS)
LIBRARY     = -L$(LIBS)
CPP_OBJS    = $(OBJS)

CPP_TEST    = $(SOURCES)train.cpp
LIBA        = $(LIBS)libtrain.a

$(OBJS)%.o : $(SOURCES)%.cpp $(HEADERS)%.h
	@echo Building $@...
	@$(CC) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

$(LIBA): $(CPP_OBJS)
	@echo Building library...
	@ar rs $(LIBA) $(CPP_OBJS)

train: $(CPP_TEST) 
	@echo Building train executable file ...
#@$(CC) $(CPP_TEST) -o $(BIN)train $(CPPFLAGS) $(LIBRARY) -ltrain $(CXXFLAGS) 
	@$(CC) $(CPP_TEST) -o $(BIN)train $(CPPFLAGS) $(CXXFLAGS) 
clean:
	rm $(OBJS)*.o
	rm $(LIBS)*.a
	rm $(BIN)train
