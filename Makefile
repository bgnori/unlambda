

CFALGS = -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include
LDFALGS = -lglib-2.0

DEBUG = -g

unlambda: unlambda.o object.o
	gcc $(CFALGS) $(LDFALGS) $(DEBUG) -Wall  -o unlambda unlambda.o object.o

unlambda.o: unlambda.c object.h
	gcc $(CFALGS) -Wall $(DEBUG) -c unlambda.c

object.o: object.c object.h
	gcc $(CFALGS) -Wall $(DEBUG) -c object.c

test: unlambda
	valgrind -v --error-limit=no --leak-check=yes --show-reachable=no ./unlambda 2>&1 | tee valgrind.log

clean:
	rm unlambda valgrind.log
