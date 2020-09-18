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
Notación utilizada `<option>` denota un argumento obligatorio, `[option]` denota un argumento opcional. **Todos los Jabber IDs utilizados en los comandos deben especificarse sin la parte del host.**
* `/help`
  - uso: `/help <command>`
  - descripción: muestra detalles sobre el comando especificado.
  - ejemplo: `/help users`
* `/users`
  - uso: `/users`
  - descripción: muestra hasta 32 de los usuarios registrados en el servidor.
  - ejemplo: `/users`
* `/roster`
  - uso: `/roster [add <jid>]`
  - descripción: cuando se utiliza `/roster`, se muestran los contactos del usuario. Cuando se utiliza `/roster add <jid>`, se envía una suscripción al usuario identificado con Jabber ID `<jid>`. Esto funciona como una petición de amistad. El Jabber ID **debe** especificarse sin la parte del host, ver ejemplo.
  - ejemplo: `/roster add sebdev`
* `/active`
  - uso: `/active`
  - descripción: muestra todos los estados que se han recibido de los contactos del usuario. Cuando un contacto del usuario inicia sesión, envía un stanza de presencia al servidor, y el servidor lo reenvía a todos los contados de dicho usuario. Esa stanza de presencia es la que se muestra en este comando.
  - ejemplo: `/active`
* `/presence`
  - uso: `/presence <show> [status]`
  - descripción: cambia el estado actual del usuario. `<show>` debe ser uno de los siguientes valores: `available`, `away`, `xa`, `dnd`, `invisible`. `<status>` es opcional y es una cadena de caracteres que se muestra como el estado del usuario.
  - ejemplo: `/presence away en clases`
* `/priv`
  - uso: `/priv <jid>`
  - descripción: inicia un chat privado con el usuario identificado con Jabber ID `<jid>`. El Jabber ID **debe** especificarse sin la parte del host, ver ejemplo.
  - ejemplo: `/priv sebdev`
* `/group`
  - uso: `/group <room_jid> <nick>`
  - descripción: se une al grupo con Jabber ID `<jid>`. Si el grupo no existe y el servidor está configurado correctamente, el grupo es creado. El Jabber ID **debe** especificarse sin la parte del host, ver ejemplo. `<nick>` es el apodo que el usuario desea tener dentro del grupo.
  - ejemplo: `/group mi_grupo_favorito juan`
* `/vcard`
  - uso: `/vcard <jid>`
  - descripción: muestra la vCard del usuario identificado por Jabber ID `<jid>`. El Jabber ID **debe** especificarse sin la parte del host, ver ejemplo.
  - ejemplo: `/vcard sebdev`
* `/file`
  - uso: `/file <path> <jid>`
  - descripción: Envía el archivo identificado por `<path>` al usuario identificado por su Jabber ID `<jid>`. El Jabber ID **debe** especificarse sin la parte del host, ver ejemplo.
  - ejemplo: `/file test.png sebdev`
* `/menu`
  - uso: `/menu`
  - descripción: limpia la pantalla. Si se está dentro de un chat privado o grupal, se sale del chat.
  - ejemplo: `/menu`
* `/delete`
  - uso: `/delete`
  - descripción: elimina la cuenta del usuario del servidor.
  - ejemplo `/delete`
* `/quit`
  - uso: `/quit`
  - descripción: sale del programa :(
  - ejemplo: `/quit`
