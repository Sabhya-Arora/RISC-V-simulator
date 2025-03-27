# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -O2 -std=c++17

# Executable names
FORWARD_EXEC = forward
NOFORWARD_EXEC = noforward

# Source files
FORWARD_SRC = forwarding.cpp
NOFORWARD_SRC = noforward.cpp

# Build rules
all: $(FORWARD_EXEC) $(NOFORWARD_EXEC)

$(FORWARD_EXEC): $(FORWARD_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(NOFORWARD_EXEC): $(NOFORWARD_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean rule
clean:
	rm -f $(FORWARD_EXEC) $(NOFORWARD_EXEC)
