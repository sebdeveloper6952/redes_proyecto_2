# UVG | Redes - Proyecto 2
## Cliente de Chat con XMPP
- Sebastián Arriola 11463

## Instalación en Ubuntu
1. Se utilizó la librería libstrophe para la interacción de bajo nivel con el protocolo XMPP. Para instalar dicha librería en ubuntu, ejecutar `sudo apt update -y && sudo apt install -y libstrophe-dev`.
2. Debe contar con `make` y `gcc` instalados.

## Compilación
- ejecutar `make` en el directorio del proyecto.

## Ejecución
- ejecutar `./ncurses_client` en el directorio del proyecto.

## Descripción de Proyecto
- Se realizó un cliente de chat utilizando el protocolo XMPP, que se conecta al servidor del curso.
- La interfaz es en línea de comando, y se utiliza el programa a base de comandos. Los comandos tienen la forma: `/command`, a continuación se detallan todos los comandos y su explicación.

## Comandos
Notación utilizada `<option>` denota un argumento obligatorio, `[option]` denota un argumento opcional.
* `/help`
  - uso: `/help <command>`
  - descripción: muestra detalles sobre el comando especificado.
  - ejemplo `/help users`
* `/users`
  - uso: `/users`
  - descripción: muestra hasta 32 de los usuarios registrados en el servidor.
  - ejemplo `/users`
* `/roster`
  - uso: `/roster [add <jid>]`
  - descripción: cuando se utiliza `/roster`, se muestran los contactos del usuario. Cuando se utiliza `/roster add <jid>`, se envía una suscripción al usuario identificado con Jabber ID `<jid>`. Esto funciona como una petición de amistad.
  - ejemplo `/roster add sebdev@redes2020.xyz`
