ifeq ($(OS),Windows_NT)
    # Windows-specific settings
    RM = del
    ECHO = echo Windows

	# Source files
	SRCS_RAW = main.cpp \
		token/defs/tokendefs.cpp \
		token/token.cpp \
		lexer/defs/lexdefs.cpp \
		lexer/lexer.cpp \
		ASTree/defs/astdefs.cpp \
		ASTree/ASTree.cpp \
		parser/parser.cpp \
		SchemaCatalog/defs/schemadefs.cpp \
		SchemaCatalog/TableSchema/TableSchema.cpp \
		SchemaCatalog/SchemaCatalog/SchemaCatalog.cpp \
		analyzer/analyzer.cpp \
		storage/BufferManager/BufferManager.cpp \
		storage/BTree/BTree.cpp \
		QueryExecutor/QueryExecutor.cpp

	SRCS = $(subst /,\\,$(SRCS_RAW))
	LIBS = -lWs2_32
else
    # Assume Unix/Linux
    RM = rm -f
    ECHO = echo Linux

	# Source files
	SRCS = main.cpp \
		token/defs/tokendefs.cpp \
		token/token.cpp \
		lexer/defs/lexdefs.cpp \
		lexer/lexer.cpp \
		ASTree/defs/astdefs.cpp \
		ASTree/ASTree.cpp \
		parser/parser.cpp \
		SchemaCatalog/defs/schemadefs.cpp \
		SchemaCatalog/TableSchema/TableSchema.cpp \
		SchemaCatalog/SchemaCatalog/SchemaCatalog.cpp \
		analyzer/analyzer.cpp \
		storage/BufferManager/BufferManager.cpp \
		storage/BTree/BTree.cpp \
		QueryExecutor/QueryExecutor.cpp
	LIBS = 
endif

# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++23 -Wall -Wextra -Werror -g

# Object files (derived from the source files)
OBJS = $(SRCS:.cpp=.o)

# Output executable
EXEC = minidbms

# Default target
all: $(EXEC)

# Rule to link object files into the executable
$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC) $(LIBS)

# Rule to compile each source file into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: $(EXEC)
	./$(EXEC)

# Clean up object files and executable
clean:
	$(RM) $(OBJS) $(EXEC)

# Additional rule to remove dependencies (optional)
distclean: clean
	$(RM) *.d

# Include dependencies (optional, but useful for large projects)
-include $(OBJS:.o=.d)

