


unlambda: unlambda.c
	gcc -Wall -g -o unlambda unlambda.c

test: unlambda
	valgrind -v --error-limit=no --leak-check=yes --show-reachable=no ./unlambda 2>&1 | tee valgrind.log

clean:
	rm unlambda valgrind.log
