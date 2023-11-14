modem: modem.c
	gcc -o modem -std=c99 -O1 modem.c

clean:
	rm modem

