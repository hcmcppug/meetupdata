EXE := football_match_stats
CXX := clang++
CXXFLAGS := -std=c++1z -I /usr/local/include
LDFLAGS := -L /usr/local/lib
LIBS := -lboost_coroutine-mt -lboost_system

SOURCES := stl_demo.cpp
OBJECTS := $(SOURCES:.cpp=.o)

$(EXE): $(OBJECTS)
	$(CXX) $< -o $@ $(LDFLAGS) $(LIBS) 

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<
