PREFIX?=/usr/X11R6
CFLAGS?=-pedantic -Wall -O3
DISPLAY=:1

all:
	g++ $(CFLAGS) -I$(PREFIX)/include *.cpp -L$(PREFIX)/lib -lX11 -o vswm 

clean:
	@clear
	@pkill vswm || true
	@pkill xterm || true
	@pkill xclock || true
	@rm -f vswm log.txt

run:
	@pkill vswm || true
	@pkill xterm || true
	@pkill xclock || true
	@pkill sxhkd || true
	@sxhkd &
	@./vswm &
	@xterm &
	@xclock -geometry 200x200+200+400 &
	@hsetroot -add "#000000" -add "#a0a0a0" -gradient 0


install: all
	sudo cp -f vswm /usr/games/vswm
