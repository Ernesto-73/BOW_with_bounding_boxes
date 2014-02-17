#Author: Javier Zhang
# Date: 12-10-2013     

CXX         = g++
CC          = $(CXX)
CPPFLAGS    = -g -Wall -D DEBUG
CXXFLAGS    = $(INCLUDE) `pkg-config --libs --cflags libxml++-2.6 matio` -lopencv_highgui -lopencv_core -lopencv_features2d -lopencv_nonfree -lopencv_ml
SOURCES     = ./src/
HEADERS     = ./include/
OBJS        = ./obj/
LIBS        = ./lib/
BIN         = ./bin/
INCLUDE     = -I$(HEADERS)
LIBRARY     = -L$(LIBS)
CPP_OBJS    =  $(OBJS)cxml.o $(OBJS)readdata.o

CPP_TRAIN    = $(SOURCES)train.cpp
CPP_MKSMP    = $(SOURCES)mksmp.cpp

LIBA        = $(LIBS)libbow.a

$(OBJS)%.o : $(SOURCES)%.cpp $(HEADERS)%.h
	@echo Building $@...
	@$(CC) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

$(LIBA): $(CPP_OBJS)
	@echo Building library...
	@ar rs $(LIBA) $(CPP_OBJS)

train: $(CPP_TRAIN) $(LIBA)
	@echo Building train executable file ...	
	@$(CC) $(CPP_TRAIN) -o $(BIN)train $(CPPFLAGS) $(LIBRARY) -lbow $(CXXFLAGS) 
	
mksmp: $(CPP_MKSMP) $(LIBA)
	@echo Building mksmp executable file ...	
	@$(CC) $(CPP_MKSMP) -o $(BIN)mksmp $(CPPFLAGS) $(LIBRARY) -lbow $(CXXFLAGS) 

clean:
	@rm $(OBJS)*.o
	@rm $(LIBS)*.a
	@rm $(BIN)train
