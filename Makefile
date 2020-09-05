all:
	gcc -o client client.c xmpp_register.c utils.c `pkg-config --cflags --libs libstrophe`