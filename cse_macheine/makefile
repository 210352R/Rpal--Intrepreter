# Define the compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# List of source files without file extensions
SOURCES = rpal-interpreter lexicon parser asttost flattenst cse


# List of object files generated from source files
OBJECTS = $(SOURCES:=.o)

# List of header files
HEADERS = $(wildcard *.hpp)

# Name of the executable
EXECUTABLE = myprogram

# Default target (executable)
all: $(EXECUTABLE)

# Rule to build the executable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Rule to build object files from source files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Phony target to clean object files and executable
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

# Phony target to run the program
run: $(EXECUTABLE)
	./$(EXECUTABLE)
