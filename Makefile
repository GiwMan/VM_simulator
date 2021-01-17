objects = ./src/main2.o ./src/HashedPageTable.o ./src/PageInfo.o ./src/Queue.o ./src/SCinfo.o

mem_sim : $(objects)
		cc -Wall -g -o ./build/mem_sim $(objects)

main2.o: ./src/main2.c ./include/HashedPageTable.h ./include/Queue.h ./include/PageInfo.h ./include/SCinfo.h
		cc -Wall -g -c  ./src/main2.c ./src/HashedPageTable.c ./src/Queue.c ./src/PageInfo.c ./src/SCinfo.c

HashedPageTable.o: ./src/HashedPageTable.c ./include/HashedPageTable.h ./include/Queue.h ./include/PageInfo.h
		cc -Wall -g -c ./src/HashedPageTable.c ./src/Queue.c ./src/PageInfo.c

Queue.o: ./src/Queue.c ./include/Queue.h ./include/HashedPageTable.h
		cc -Wall -g -c ./src/Queue.c ./src/HashedPageTable.c

PageInfo.c: ./src/PageInfo.c ./include/PageInfo.h
		cc -Wall -g -c ./src/PageInfo.c

SCinfo.c: ./src/SCinfo.c ./include/SCinfo.h ./include/PageInfo
		cc -Wall -g -c ./src/SCinfo.c ./src/PageInfo.c


# mem_sim: 
# 	gcc -Wall -g ./src/main2.c ./src/HashedPageTable.c ./src/PageInfo.c ./src/Queue.c -o ./build/mem_sim

# Queue.o: 

clean:
	rm -rf ./build/mem_sim ./src/*.o


