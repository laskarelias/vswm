
PREFIX?=/usr/X11R6
CFLAGS?=-pedantic -Wall -O3
DISPLAY=:1

all:
	g++ $(CFLAGS) -I$(PREFIX)/include *.cpp -L$(PREFIX)/lib -lX11 -o vswm 

clean:
	clear
	pkill vswm || true
	pkill xterm || true
	pkill xclock || true
	rm -f vswm log.txt

run:
	pkill vswm || true
	pkill xterm || true
	pkill xclock || true
	./vswm &
	xterm &
	xclock -geometry 200x200+200+400 &

install: all
	sudo cp vswm /usr/games/vswm
