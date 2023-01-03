# Copyright (c) 2022-2023
# Date: 2023-01-03


CC = g++

CXXFLAGS = -g -Wall -O2 -fPIC

LD_FLAGS = -fPIE

INCLUDE = -I./

OBJS = expr_eval.o

TARGET = expr_eval

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) $(LD_FLAGS) $^ -o $@

$(OBJS):%.o:%.cpp
	$(CC) -c $(CXXFLAGS) $(INCLUDE) $< -o $@

.PHONY:clean

clean:
	rm -f $(OBJS) $(TARGET)
      
