# http://www.puxan.com/web/blog/HowTo-Write-Generic-Makefiles
# See also http://stackoverflow.com/questions/2481269/how-to-make-simple-c-makefile
# Declaration of variables
CC = g++
CC_FLAGS = -Wall -Wextra --std=c++0x -g -D_GLIBCXX_DEBUG

all: test qsc

# File names
TEST = test
SOURCES_TEST = BitSink.cpp  Coders.cpp    HuffmanCoder.cpp    HuffmanTable.cpp  qs_BitSource.cpp  qs_Quantity.cpp   test_Coders.cpp catch.cpp    Decoders.cpp  HuffmanDecoder.cpp  Modeller.cpp test_BitSink.cpp  test_Huffman.cpp
OBJECTS_TEST = $(SOURCES_TEST:.cpp=.o)
# Main target
$(TEST): $(OBJECTS_TEST)
	$(CC) $(OBJECTS_TEST) -o $(TEST)

QSC = qsc
SOURCES_QSC = qsc.cpp
OBJECTS_QSC = $(SOURCES_QSC:.cpp=.o)
$(QSC): $(OBJECTS_QSC)
	$(CC) $(OBJECTS_QSC) -o $(QSC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(TEST) $(OBJECTS_TEST) $(QSC) $(OBJECTS_QSC)
