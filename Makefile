# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++23 -Wall -Wextra -Werror -g

# Source files
SRCS = main.cpp \
	token/token.cpp \
	lexer/defs/lexdefs.cpp \
	lexer/lexer.cpp \
	ASTree/defs/astdefs.cpp \
	ASTree/ASTree.cpp \
	parser/parser.cpp

# Object files (derived from the source files)
OBJS = $(SRCS:.cpp=.o)

# Output executable
EXEC = minicpp

# Default target
all: $(EXEC)

# Rule to link object files into the executable
$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC)

# Rule to compile each source file into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: $(EXEC)
	./$(EXEC)

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(EXEC)

# Additional rule to remove dependencies (optional)
distclean: clean
	rm -f *.d

# Include dependencies (optional, but useful for large projects)
-include $(OBJS:.o=.d)

