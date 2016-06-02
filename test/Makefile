BIN=xmlparse
OBJECTS=main.o
LIBS=-L.. -lxml
FLAGS=-O2 -I.. -Wall -Wextra

.c.o: $(OBJECTS)
	$(CC) -c $< -o $@ $(FLAGS)

$(BIN): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f *.o $(BIN)
