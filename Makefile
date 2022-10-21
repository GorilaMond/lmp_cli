obj = lmpcpp.o
lmp: lmp.cpp
	g++ $^ -o $@
	strip lmp
all: lmp
clean:
	rm -f lmp
install:
	sudo cp lmp /usr/bin
uninstall:
	sudo rm -f /usr/bin/lmp
