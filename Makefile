TARGET = out.exe
CC = gcc
RM = rm -f


all :
	@echo "make listeChainee"
	@echo "make listeZ"
	@echo "make permutation"
	@echo "make quadTree"

listechainee :
	@$(CC) -o $(TARGET) listeChainee.c
	@./$(TARGET)

listeZ :
	@$(CC) -o $(TARGET) listeZ.c
	@./$(TARGET)

permutation :
	@$(CC) -o $(TARGET) permutation.c
	@./$(TARGET)

quadtree :
	@$(CC) -o $(TARGET) quadtree.c
	@./$(TARGET)


clean:
	$(RM) *.exe *.o

.PHONY: clean


