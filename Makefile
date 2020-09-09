deps = xmpp_register.c xmpp_login.c xmpp_utils.c
deps += xmpp_roster.c xmpp_users.c xmpp_service_discovery.c
deps += xmpp_presence.c xmpp_account.c xmpp_vcard.c
deps += xmpp_im.c xmpp_gm.c
pkg_config = `pkg-config --cflags --libs libstrophe`
all:
	gcc -o client client.c $(deps) $(pkg_config)