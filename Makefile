test: bcmap.o
bcmap.o: bcmap.h
clean:
	rm -f test bcmap.o
dist: clean
	cd .. ; tar czvf bcmap.tar.gz bcmap/
