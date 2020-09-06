deps = xmpp_register.c xmpp_login.c xmpp_utils.c
pkg_config = `pkg-config --cflags --libs libstrophe`
all:
	gcc -o client client.c $(deps) $(pkg_config)