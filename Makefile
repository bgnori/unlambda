

CFALGS = -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include
LDFALGS = -lglib-2.0

DEBUG = -g


primitive.o: primitive.c primitive.h peer.h
	gcc $(CFALGS) -Wall $(DEBUG) -c primitive.c

peer.o: peer.c peer.h
	gcc $(CFALGS) -Wall $(DEBUG) -c peer.c

test: unlambda
	valgrind -v --error-limit=no --leak-check=yes --show-reachable=no ./unlambda 2>&1 | tee valgrind.log

clean:
	rm unlambda valgrind.log

unlambda: unlambda.o object.o
	gcc $(CFALGS) $(LDFALGS) $(DEBUG) -Wall  -o unlambda unlambda.o object.o

unlambda.o: unlambda.c object.h
	gcc $(CFALGS) -Wall $(DEBUG) -c unlambda.c

object.o: object.c object.h
	gcc $(CFALGS) -Wall $(DEBUG) -c object.c

