# paths
INCLUDE = ../include
MODULES = ../modules
MISC = ../misc
# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall  -g -I$(INCLUDE)
LDFLAGS = 

# Αρχεία .o
OBJS = mngstd.o $(MODULES)/UsingHashTable/ADTMap.o $(MODULES)/UsingADTBList/InvertedIndex.o $(MODULES)/UsingDoublyLL/ADTBList.o $(MODULES)/func.o

# Το εκτελέσιμο πρόγραμμα
EXEC = mngstd

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS = -i $(MISC)/inputFile/input300000.txt -c $(MISC)/configFile/config300000.txt

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

valgrind: $(EXEC)
	valgrind --leak-check=full ./$(EXEC) $(ARGS)