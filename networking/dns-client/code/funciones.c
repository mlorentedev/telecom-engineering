/*
**	Fichero: funciones.c	
**	Autor:Manuel Lorente Almán	
**	Fecha: 26-12-2013
**	
**	
**	Descripcion: Fichero con funciones utilizadas en todo el programa
**      y cuya breve descripcion encontramos en el fichero de libreria
**      "tcp.h".
*/

/*Includes del sistema*/

#include <stdio.h>       //Contiene funciones de biblioteca y prototipos de C
#include <stdlib.h>      //Libreria para la reserva dinámica de memoria
#include <string.h>      //Libreria para el manejo de cadena de caracteres
#include <unistd.h>      // Librerias para la utilizacion de sockets
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <netdb.h>

/*Includes de la aplicación*/
#include "tcp.h"

/* 
   Bloquea el programa hasta que no haya algo que leer del descriptor 
   pasado como parametro descriptor (puede ser un socket o un fichero)
   o hasta que transcurran un plazo (segundos) sin recibir nada.
   Parametros de entrada:
   - descriptor - descriptor del fichero o socket
   - segundos - tiempo maximo de espera en segundos
   Devuelve:
   - 1 si se puede leer, 0 si ha vencido el plazo.
*/
int recibido_en_plazo(int descriptor, int segundos)
{
  struct timeval plazo = { segundos, 0 }; /* plazo de recepcion */
  fd_set readfds;	        //Conjunto de descriptores a monitorizar
  FD_ZERO(&readfds);            //Limpiamos fds
  FD_SET(descriptor, &readfds); //Insertamos descriptor en el conjunto
  return select(descriptor + 1,
                &readfds, (fd_set *) NULL, (fd_set *) NULL, &plazo) != 0;
}
/*Función que segun el error recibido imprime un mensaje de error o no*/

void Gestiona_Errores(int error, char *servidor)
{
  switch(error)
    {
    case 1:
      fprintf(stderr,ERROR1,servidor);
      break;
    case 2:
      fprintf(stderr,ERROR2,servidor);
      break;
    case 3:
      fprintf(stderr,ERROR3,servidor);
      break;
    case 4:
      fprintf(stderr,ERROR4);
      break;
    case 5:
      fprintf(stderr,ERROR5);
      break;
    case 6:
      fprintf(stderr,ERROR6);
      break;
    case 7:
      fprintf(stderr,ERROR7,servidor);
      break;
    case 8:
      fprintf(stderr,ERROR8);
    break;
      case 9:
      fprintf(stderr,ERROR9);
      break;
    default:
      break;
    }
}
/*Se encarga de elegir la opcion de transferencia de archivo
o consulta DNS en otro caso retorna 0*/
int Elige_opcion(char *opcion)
{
  int a = 0;
  if (strcmp(opcion,"-f") == 0)
    a = 1;
  if (strcmp(opcion,"-s") == 0)
    a = 2;
  
  return a;
}
/*
  Realiza el proceso de inicializacion estandar de un socket TCP activo.
  En caso de error provoca el fin del programa.
  Parametros de entrada:
  - maquina - cadena de texto con el nombre o direccion IP de la maquina.
  x  - puerto - cadena de texto con el puerto a usar. Puede ser un numero
  o un nombre de servicio existente en el fichero services
  Devuelve:
  - Nuevo socket.
*/
int inicia_socket_cliente(const char *maquina, const char *puerto)
{
  int s;
  struct addrinfo infoserv;     /* informacion extremo local */
  struct sockaddr dirserv;      /* direccion internet socket servidor */
  
  /* Ponemos a 0 la estructura infoserv */
  memset(&infoserv, 0, sizeof(struct addrinfo));
  infoserv.ai_addr = &dirserv;  //aqui se guardara la direccion
  /* Obtenemos datos del extremo al que queremos llamar.
     Si tiene exito, utilizaremos los datos almacenados en infoserv 
     para crear el socket. */
  if (!traduce_a_direccion(maquina, puerto, SOCKET_TCP, &infoserv))
    {
      exit(EXIT_ERROR);
      Gestiona_Errores(8,NULL);}
  
  /* Crea socket de TCP */
  if ((s = socket(infoserv.ai_family, infoserv.ai_socktype,
                  infoserv.ai_protocol)) < 0)
    error_fatal("socket");
  
  /* Conectamos el socket */
  if (connect(s, &dirserv, sizeof(dirserv)) < 0)
    {
      close(s);
      error_fatal("");
	Gestiona_Errores(6,NULL);
    }
  return s;
}

/* 
   Muestra por la salida de errores informacion del ultimo error
   producido y sale del programa.
   Parametros de entrada:
   - mens - cadena de texto que se agrega al mensaje de error
   Devuelve:
   - Nada
*/
void error_fatal(char *mens)
{
  perror(mens);
  exit(EXIT_ERROR);
}

/* 
   Bloquea el programa hasta que no haya algo que leer del descriptor 
   pasado como parametro s (puede ser un socket o un fichero) o hasta que
   transcurran un plazo (segundos) sin recibir nada.
   Parametros de entrada:
   - descriptor - descriptor del fichero o socket
   - segundos - tiempo maximo de espera en segundos
   Devuelve:
   - 1 si se puede leer, 0 si ha vencido el plazo.
*/
int espera_recepcion(int descriptor, int segundos)
{
  
  struct timeval plazo = { segundos, 0L };      /* plazo de recepcion */
  
  fd_set fds;                   //Conjunto de descriptores a monitorizar
  FD_ZERO(&fds);                //Limpiamos fds
  FD_SET(descriptor, &fds);     //Insertamos descriptor en el conjunto
  return (select
          (descriptor + 1, &fds, (fd_set *) NULL, (fd_set *) NULL, &plazo));
  
}


/* 
   Bloquea el programa hasta que no se pueda escribir en el descriptor 
   pasado como parametro s (puede ser un socket o un fichero) o hasta que
   transcurran un plazo (segundos) sin poder escribir.
   Parametros de entrada:
   - descriptor - descriptor del fichero o socket
   - segundos - tiempo maximo de espera en segundos
   Devuelve:
   - 1 si se puede escribir, 0 si ha vencido el plazo.
*/
int espera_envio(int descriptor, int segundos)
{
  
  struct timeval plazo = { segundos, 0L };      /* plazo para poder escribir */

  fd_set fds;                   //Conjunto de descriptores a monitorizar
  FD_ZERO(&fds);                //Limpiamos fds
  FD_SET(descriptor, &fds);     //Insertamos descriptor en el conjunto
  return (select
          (descriptor + 1, (fd_set *) NULL, &fds, (fd_set *) NULL, &plazo));

}

/* 
   Funcion que lee del descriptor (socket o fichero) el numero de bytes indicados
   por parametro y los almacena en el buffer pasado.
   Tiene en cuenta errores de fin de fichero o cierre de socket y que en una
   lectura puede que no se lean todos los bytes pedidos. Tambien deja de leer
   si entre lecturas pasa mas de un determinado tiempo.
   Parametros de entrada:
   - s - descriptor del fichero o socket
   - buffer - buffer donde se guardaran los bytes leidos
   - longitud - numero de bytes que se quieren leer
   - segundos - tiempo maximo de espera en segundos entre lecturas
   Devuelve:
   - Verdadero si ha conseguido leer todos los bytes pedidos
*/
int lee(int s, char *buffer, int longitud, int segundos)
{
  int leidos_total = 0;         //Datos leidos hasta el momento
  int leidos_actual = 0;        //Datos leidos en la ultima peticion de lectura
  int error = FALSE;
  while (leidos_total < longitud && !error)
    {
      //Esperamos a que haya datos disponibles
      if (espera_recepcion(s, segundos) == 1)
        {
	 
          leidos_actual =
            read(s, buffer + leidos_total, longitud - leidos_total);
          if (leidos_actual == 0)       //Indica fin de fichero o cierre del socket
            error = TRUE;
          else if (leidos_actual < 0)   //Ha habido un error
            {
              perror("read");
              error = TRUE;
            }
          else
            leidos_total += leidos_actual;
        }
      else
        {
          fprintf(stderr, "La lectura tarda demasiado\n");
          error = TRUE;
        }
    }

  return (leidos_total == longitud);
}

/* 
   Funcion que escribe en el descriptor (socket o fichero) el numero de bytes 
   indicados por parametro del buffer tambien pasado.
   Tiene en cuenta errores de fin de fichero o cierre de socket y que en una
   escritura puede que no se escriban todos los bytes pedidos. Tambien deja de
   escribir si entre escrituras pasa mas de un determinado tiempo.
   Parametros de entrada:
   - s - descriptor del fichero o socket
   - buffer - buffer que contiene los bytes a escribir
   - longitud - numero de bytes que se quieren escribir
   - segundos - tiempo maximo de espera en segundos entre escrituras
   Devuelve:
   - Verdadero si ha conseguido escribir todos los bytes pedidos
*/
int escribe(int s, const char *buffer, int longitud, int segundos)
{
  int escritos_total = 0;       //Datos escritos hasta el momento
  int escritos_actual = 0;      //Datos escritos en la ultima peticion
  int error = FALSE;
  while (escritos_total < longitud && !error)
    {
      //Esperamos a que podamos enviar datos
      if (espera_envio(s, segundos) == 1)
        {
          escritos_actual =
            write(s, buffer + escritos_total, longitud - escritos_total);
          if (escritos_actual == 0)     //Indica fin de fichero o cierre del socket
            error = TRUE;
          else if (escritos_actual < 0) //Ha habido un error
            {
              perror("write");
              error = TRUE;
            }
          else
            escritos_total += escritos_actual;
        }
      else
        {
          fprintf(stderr, "La escritura tarda demasiado\n");
          error = TRUE;
        }
    }
  return (escritos_total == longitud);
}

/* 
   Lee un mensaje de un socket cliente. Espera recibir un entero (4 bytes)
   codificado como esta en memoria con la longitud de los datos, seguido de 
   los datos. Tiene en cuenta el tiempo maximo de espera. 
   Devuelve los datos leidos (memoria dinamica que
   hay que liberar tras su uso) y la longitud de estos (out_longitud)
   Parametros de entrada:
   - s_cliente - socket del cliente
   - segundos - tiempo maximo de espera en segundos entre lecturas
   Parametros de salida
   - out_longitud - longitud de los datos leidos
   Devuelve:
   - Datos leidos. Hay que liberar la memoria despues de su uso.
   NULL en caso de error.
*/
char *lee_mensaje(int s_cliente, int segundos, int *out_longitud)
{
  int longitud = 0;             //Longitud de los datos que vamos a recibir
  char *buf = NULL;             //Buffer para almacenar los datos leidos

  /* Leemos longitud: pasamos a la funcion lee la direccion de memoria de la
     variable longitud y su tamanio para que los bytes se guarden ahi */
  if (lee(s_cliente, (char *) &longitud, sizeof(longitud), segundos)
      && longitud > 0)
    {                           //Longitud recibida y mayor que cero
      //Creamos buffer con memoria dinamica de ese tamanio
      *out_longitud = longitud;
      buf = (char *) malloc(longitud);
      if (NULL != buf)          //Hay memoria
        {
          //Leemos datos
          if (!lee(s_cliente, buf, longitud, segundos))
            {
              fprintf(stderr, "Error recibiendo datos.\n");
              //Liberamos memoria
              free(buf);
              buf = NULL;
            }
        }
      else
        fprintf(stderr, "No hay memoria.\n");
    }

  return buf;
}

/* 
   Envia un mensaje. Envia un entero (4 bytes)
   codificado como esta en memoria con la longitud de los datos, seguido de 
   los datos. Tiene en cuenta el tiempo maximo de espera. 
   Parametros de entrada:
   - s_cliente - socket del cliente
   - datos - datos a enviar
   - len - longitud de los datos a enviar
   - segundos - tiempo maximo de espera en segundos entre escrituras
   Devuelve:
   - Verdadero si se han enviado todos los bytes.
*/
int envia_mensaje(int s_cliente, char *datos, int len, int segundos)
{
  int correcto = TRUE;

  //Enviamos longitud
  if (escribe(s_cliente, (char *) &len, sizeof(len), segundos))
    {
      //Enviamos datos
      if (!escribe(s_cliente, datos, len, segundos))
        {
          fprintf(stderr, "Error enviando datos.\n");
          correcto = FALSE;
        }
    }
  else
    {
      fprintf(stderr, "Error enviando longitud.\n");
      correcto = FALSE;
    }

  return correcto;
}
/*
  Convierte una direccion de Internet y un puerto de servicio
  (ambos cadena de caracteres) a valores numericos para poder
  ser utilizados en otras funciones, como bind y connect.
  La informacion tambien se imprimira por pantalla.
  Parametros de entrada:
  - maquina - cadena de caracteres con la direccion de Internet
  - puerto - cadena de caracteres con el puerto de servicio
  - tipo - SOCKET_UDP, SOCKET_TCP o SOCKET_TCP_PASIVO (escucha)
  Parametros de salida:
  - info - estructura addrinfo con el primer valor encontrado
  Devuelve:
  - Verdadero, si ha tenido exito.
*/
int traduce_a_direccion(const char *maquina, const char *puerto,
                        int tipo, struct addrinfo *info)
{
  struct addrinfo hints;        /* Estructura utilizada para afinar la
                                   busqueda */
  struct addrinfo *result, *rp; /*rp, variable usada para recorrer
                                   la lista de direcciones 
                                   encontradas */
  int error = 0;

  /* Obtiene las direcciones que coincidan con maquina/puerto */

  /* Ponemos a 0 la estructura hints */
  memset(&hints, 0, sizeof(struct addrinfo));
  /*Inicializamos la estructura */
  hints.ai_family = AF_INET;    /* AF_UNSPEC Permite IPv4 o IPv6
                                   AF_INET solo IPv4 */
  if (SOCKET_UDP == tipo)
    hints.ai_socktype = SOCK_DGRAM;     /* Socket de datagramas */
  else
    hints.ai_socktype = SOCK_STREAM;    /* Socket de flujo */
  hints.ai_protocol = 0;        /* Cualquier protocolo */
  if (SOCKET_TCP_PASIVO == tipo)
    hints.ai_flags |= AI_PASSIVE;       /* Cualquier direccion IP */


  /*Llamamos a la funcion de busqueda de nombres */
  error = getaddrinfo(maquina, puerto, &hints, &result);
  if (error != 0)
    {
      //Mostramos informacion del error usando la funcion gai_strerror
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
    }
  else
    {

      /* getaddrinfo() devuelve una lista de estructuras addrinfo.
         Vamos a imprimirlas todas, aunque solo devolveremos la primera */
      printf("Resultado de la resolucion de nombre:\n");
      for (rp = result; rp != NULL; rp = rp->ai_next)
        {
          printf("-> ");
          imprime_extremo_conexion(rp->ai_addr, rp->ai_addrlen, tipo);
          printf("\n");
        }

      if (result == NULL)
        {                       /* No se ha devuelto ninguna direccion */
          fprintf(stderr, "No se han encontrado direcciones.\n");
          error = 1;            //Hay error
        }
      else
        {
          //Copiamos solo los campos del primer resultado que interesan.
          info->ai_family = result->ai_family;
          info->ai_socktype = result->ai_socktype;
          info->ai_protocol = result->ai_protocol;
          *info->ai_addr = *result->ai_addr;    //Copiamos contenido del puntero
          info->ai_addrlen = result->ai_addrlen;
        }

      freeaddrinfo(result);     /* Liberamos los datos */

    }
  return !error;
}

/*
  Funcion que imprime el nombre de la maquina asociada a una
  direccion de internet y el puerto de una conexion.
  Hace uso de la funcion getnameinfo.
  Parametros de entrada:
  - direccion - estructura sockaddr con informacion de un extremo del socket.
  - len - longitud de la estructura direccion
  - tipo - SOCKET_UDP o SOCKET_TCP
  Devuelve:
  - Nada
*/
void imprime_extremo_conexion(const struct sockaddr *direccion, socklen_t len,
                              int tipo)
{
  char hbufnum[NI_MAXHOST];     //cadena de la maquina (numerico)
  char hbufnombre[NI_MAXHOST];  //cadena de la maquina (nombre)
  char sbuf[NI_MAXSERV];        //cadena del servicio
  int opciones = NI_NUMERICHOST | NI_NUMERICSERV;       //Opciones para getnameinfo
  int error = 0;

  if (tipo == SOCKET_UDP)
    {
      opciones |= NI_DGRAM;
    }
  //Convertimos a cadena de caracteres
  error = getnameinfo(direccion, len, hbufnum, sizeof(hbufnum), sbuf,
                      sizeof(sbuf), opciones);
  if (error == 0)
    {
      //Obtenemos tambien el nombre asociado a esa direccion IP
      if (tipo == SOCKET_TCP_PASIVO)
        printf("Escuchando en ");
      else if (getnameinfo(direccion, len, hbufnombre, sizeof(hbufnombre),
                           NULL, 0, NI_NAMEREQD))
        //Error obteniendo el nombre
        printf("Maquina=(desconocida) ");
      else
        printf("Maquina=%s ", hbufnombre);

      //Imprimimos valores numericos.
      printf("(%s), Puerto=%s", hbufnum, sbuf);
      if (tipo == SOCKET_UDP)
        printf(" UDP");
      else
        printf(" TCP");

    }
  else
    //Mostramos informacion del error usando la funcion gai_strerror
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
}


