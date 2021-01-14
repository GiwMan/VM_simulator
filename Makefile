mem_sim: 
	gcc -Wall -g ./src/main.c ./src/PageTable.c ./src/Queue.c -o ./build/mem_sim

clean:
	rm -rf ./build/mem_sim ./src/*.o