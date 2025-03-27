# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -O2 -std=c++17

# Executable names
FORWARD_EXEC = src/forward
NOFORWARD_EXEC = src/noforward

# Source files
FORWARD_SRC = src/forwarding.cpp
NOFORWARD_SRC = src/noforward.cpp

# Build rules
all: $(FORWARD_EXEC) $(NOFORWARD_EXEC)

$(FORWARD_EXEC): $(FORWARD_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(NOFORWARD_EXEC): $(NOFORWARD_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean rule
clean:
	rm -f $(FORWARD_EXEC) $(NOFORWARD_EXEC)
