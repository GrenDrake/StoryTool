# uncomment these lines if using a compiler that doesn't support C++11's regex
# header
#LFLAGS=-lboost_regex
#CXXFLAGS= --std=c++11 -g -Wall -DBOOST

# uncomment these instead if the compiler does support C++11's regex header
LFLAGS=
CXXFLAGS= --std=c++11 -g -Wall

OBJS=src/storytools.o src/story.o src/replace.o src/exporterBare.o \
	src/exporterHTML.o src/exporterRTF.o src/exporterMarkdown.o

.PHONY: clean

all: $(OBJS)
	g++ $(OBJS) $(LFLAGS) -o storytools

clean:
	$(RM) src/*.o storytools
