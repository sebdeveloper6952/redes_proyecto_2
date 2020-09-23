# UVG | Redes - Proyecto 2
## Cliente de Chat con XMPP
- Sebastián Arriola 11463

## Utilizar Proyecto con Docker (Recomendado)
1. Se puede utilizar el proyecto fácilmente con Docker y sin instalar dependencias en su computadora, más que Docker (versión probada fue 19.03.12).
2. En el directorio del proyecto ejecutar `docker build --tag xmpp_chat .` para crear la imagen que contiene todas las dependencias necesarias.
3. Para utilizar el cliente de chat, ejecutar `docker run -it xmpp_chat`, lo cual corre el contenedor del proyecto.
4. Cuando se haya terminado de usar el chat y si se desea remover las imagenes creadas anteriormente, ejecutar `docker rmi xmpp_chat ubuntu:bionic`

## Utilizar Proyecto en Ubuntu
1. Se utilizó la librería libstrophe para la interacción de bajo nivel con el protocolo XMPP. La interfaz en línea de comando se realizó con ncurses.
2. Para instalar las dependencias del proyecto, ejecutar: `sudo apt update -y && sudo apt install -y build-essential pkg-config libssl-dev libexpat-dev libstrophe-dev libncurses5-dev libncursesw5-dev`
3. Debe contar con `make` y `gcc` instalados.

## Compilación
- ejecutar `make` en el directorio del proyecto.

## Ejecución
- ejecutar `./ncurses_client` en el directorio del proyecto.

## Funcionalidades Implementadas
- Registrar cuenta en servidor :heavy_check_mark:
- Eliminar cuenta en servidor :heavy_check_mark:
- Iniciar / cerrar sesión :heavy_check_mark:
- Mostrar todos los usuarios (se consultan todos pero solo se muestran 32) :heavy_check_mark:
- Mostrar contactos (roster)
- Mostrar detalles de contacto de un usuario (vCard) :heavy_check_mark:
- Comunicación 1 a 1 con cualquier usuario / contacto :heavy_check_mark:
- Participar en conversaciones grupales :heavy_check_mark:
- Definir mensaje de presencia :heavy_check_mark:
- Enviar / Recibir Notificaciones :heavy_check_mark:
- Enviar / Recibir Archivos :heavy_check_mark:

## Descripción de Proyecto
- Se realizó un cliente de chat utilizando el protocolo XMPP, que se conecta al servidor del curso.
- La interfaz es en línea de comando, y se utiliza el programa a base de comandos. Los comandos tienen la forma: `/command`, a continuación se detallan todos los comandos y su explicación.

## Comandos
Notación utilizada: `<option>` denota un argumento obligatorio, `[option]` denota un argumento opcional. **Todos los Jabber IDs utilizados en los comandos deben especificarse sin la parte del host (Excepto en el comando `/file`.**
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
  - descripción: Envía el archivo identificado por `<path>` al usuario identificado por su Jabber ID `<jid>`. **El Jabber ID debe especificarse completo. Se recomienda ejecutar el comando `/active` para ver el Jabber ID completo de un usuario, y luego enviar el archivo utilizando ese Jabber ID.**
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
