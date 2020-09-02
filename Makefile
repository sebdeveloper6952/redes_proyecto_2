all:
	gcc -o client client.c `pkg-config --cflags --libs libstrophe`