file = main

codigo: $(file).cpp
	g++ $(file).cpp -o $(file) -Wall
	./$(file)

clean:
	rm -f $(file)