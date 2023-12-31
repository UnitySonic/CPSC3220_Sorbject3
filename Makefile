libmyalloc.so: allocator.c
	clang -O2 -DNDEBUG -Wall -fPIC -shared allocator.c -o libmyalloc.so -lm

clean:
	rm *.so

cleanTar:
	rm *.tgz
tar:
	tar cvzf project3.tgz README Makefile allocator.c

test:
	clang -g -Wall -o test allocator.c test3.c -lm
testFast:
	clang -g -O2 -Wall -o test allocator.c test3.c -lm

test1: test1.c
	gcc -g -Wall -o test1 test1.c
test2: test2.c
	gcc -g -Wall -o test2 test2.c
test3: test3.c
	gcc -g -Wall -o test3 test3.c
test4: test4.c
	gcc -g -Wall -o test4 test4.c
