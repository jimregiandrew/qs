# http://www.puxan.com/web/blog/HowTo-Write-Generic-Makefiles
# See also http://stackoverflow.com/questions/2481269/how-to-make-simple-c-makefile
# Declaration of variables
CC = g++
CC_FLAGS = -Wall -Wextra --std=c++0x -g -D_GLIBCXX_DEBUG

# File names
EXEC = test
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
