LIBS = $(shell pkg-config --libs opencv)

CXX = mpic++
CXX_FLAGS = -Wall -Wextra -pedantic -std=c++03 -O2 -flto

NAME := color_blur

all:  
	$(CXX) $(CXX_FLAGS) $(NAME).cpp $(LIBS) -o $(NAME) 


