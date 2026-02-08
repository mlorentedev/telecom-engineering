/*
**	Fichero: clientedns.3	
**	Autor:Manuel Lorente Almán	
**	Fecha: 29-12-2013
**	
**	
**	Descripcion: Aplicacion distribuida encargada de la comunicacion
**      Con un servidor DNS. Posee dos tareas en funcion de la opcion elegida;
**      transferir base de datos que utilizara servirdor DNS mediante una
**      conexion TCP (-f) o realizar una consulta a un servidor DNS sobre
**      registros de su base de datos a traves de una conexion UDP(-s). Todo
**      ello con sus correspondientes tratamiento de errores.
*/

/*Librerias del sistema*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <netdb.h>

/*Librerias de la aplicacion*/
#include "tcp.h"

/*
  Funcion principal
*/
int main(int argc, char **argv)
{
  FILE * fent = NULL;           /* Ficheros a utilizar para leer y escribir*/
  int s_cliente;                /* socket del cliente */
  int error = 0;                /* Variable de control que indica si ha habido error */
  char linea[TAM_MAX_LINEA];    /* Linea leida de la entrada estandar */
  unsigned int len = 0;         /* Longitud de la linea a enviar*/
  int len_recibido = 0;         /* Longitud del mensaje recibido */
  char * respuesta = NULL;      /* Respuesta recibida del servidor*/
  struct addrinfo infoserv;     /* Informacion extremo remoto*/
  struct sockaddr dirserv;      /* Direccion internet remota*/
  struct sockaddr from;         /* Direccion internet remota auxiliar*/
  PDU_RR mensaje;               /* Estructuras para el intercambio de mensajes*/
  int cc;                       /* Variable de control intercambio mensajes por UDP*/
  unsigned int i = 0;           /* Variable de control del numero de secuencia*/

  /* Ignoramos la senial SIGPIPE que se produce cuando se intenta escribir
     en un socket cerrado por el otro extremo cuando ambos extremos están
     en la misma maquina. Si no se ignorara el programa
     se cerraría inmediatamente al ocurrir esto. Al ignorarlo, write devuelve
     un error que podemos tratar en el codigo */ 

  signal (SIGPIPE, SIG_IGN);

  /* Comprueba numero de argumentos */
  if (argc != 5)
    {
      Gestiona_Errores(4,argv[3]);
      error=1;
    }
  else
    {
      if (Elige_opcion(argv[1]) == 0)
	Gestiona_Errores(5,NULL);
      else
	{
	  /*Filtro para la transferencia de la base de datos al servidor*/
	  if ( 1 == Elige_opcion(argv[1]))
	    {
	      /*Creamos el socket del cliente para conexion TCP*/
	      s_cliente = inicia_socket_cliente(argv[3], argv[4]);

	      if (s_cliente == 0)
		Gestiona_Errores(6,NULL);
	      else
		{ 
		  /*Abrimos archivo con base de datos a transferir*/
		  fent =fopen (argv [2], "r");
		  
		  /*Si no se puede abrir o no se encuentra se trata el error*/
		  if (fent == NULL)
		    Gestiona_Errores(7,argv[2]);
		  else
		    {
		      /*Vamos leyendo el fichero linea a linea y enviandolo al servidor*/
		      while (fgets(linea, TAM_MAX_LINEA, fent) != NULL)
			{
			  len = strlen(linea);
			  if (len >0)
			    if (envia_mensaje(s_cliente, linea, len, PLAZO) != TRUE)
			      {
				/*A continuacion el tratamiento en el caso de error en el envio*/
				exit(EXIT_ERROR);
				Gestiona_Errores(6,NULL);
			      }
			}
		      /*Enviamos mensaje de longitud 0 para cerrar la conexion TCP*/
		      strcpy(linea,"");
		      len = envia_mensaje(s_cliente,linea,0,PLAZO);
		      respuesta = lee_mensaje(s_cliente, PLAZO, &len_recibido);
		      
		      /*Esperamos ok del servidor para dar por concluida la transferencia*/
		      if (strncmp(respuesta,"OK",2) == 0)
			{
			  /*Cerramos el socket*/
			  if (close(s_cliente) < 0) 
			    Gestiona_Errores(6,NULL);
			  /*Si todo es correcto imprimimos mensaje de transferencia completada*/
			  fprintf(stdout,RES1, argv[2]);
			}
		      else
		        {
			Gestiona_Errores(3,NULL);
			  Gestiona_Errores(9,NULL);
		     } 
		    }
		}
	    }
	  /*Filtro para la opcion de consulta DNS*/
	  else if (Elige_opcion(argv[1]) == 2)
	    {
	      /*Gestionamos la direccion y la preparamos para utilizarla*/
	      memset(&infoserv, 0, sizeof(struct addrinfo));
	      infoserv.ai_addr = &dirserv;
	      if ( !traduce_a_direccion(argv[2], PUERTOUDP, SOCKET_UDP, &infoserv))
		{
		  error =1;
		  Gestiona_Errores(6,NULL);
		}
	      /*Si la direccion es correcta creamos el socket UDP para la consulta*/
	      else if ((s_cliente = socket(infoserv.ai_family, infoserv.ai_socktype,
					   infoserv.ai_protocol)) <0)
		{
		  fprintf(stderr,"No se pudo crear el socket\n");
		  error = 1;
		}
	      else
		{
		  /*Rellenamos la PDU con los datos a enviar: consulta, campo request
		   y numero de secuencia*/
		  strcpy(mensaje.rr.name, argv[3]);
		  strcpy(mensaje.rr.value, "");	
		  mensaje.rr.type =*argv[4];
		  mensaje.sn=1;
		  mensaje.req = 0;
		  /*Variable de control cc para el envio de la estructura con datos*/
		  cc = sendto(s_cliente, &mensaje, sizeof(mensaje), 0,
			      infoserv.ai_addr, infoserv.ai_addrlen);
		  
		  if ( cc< 0)
		    error_fatal("sendto");
		  len = 0;
		  /*Temporizador de recepcion de mensajes*/
		  if (recibido_en_plazo(s_cliente,10))
		    {
		      len = sizeof(from);
		      i = mensaje.sn;
		      cc = recvfrom(s_cliente, &mensaje, sizeof(mensaje), 0
				    ,&from, &len);
		      /*En caso de ausencia de recepcion se produce un error fatal*/
		      if ( cc < 0)
			{
			  Gestiona_Errores(6,argv[2]);
			  error_fatal("");
			}
		      else
			{
			  /*Comprobamos que se ha encontrado el registro*/
			  if ((mensaje.req == 1) && (i == mensaje.sn))
			    {
			      /*Comprobamos que la respuesta es correcta*/
			      if (0 != strcmp(mensaje.rr.value,""))
				fprintf(stdout,RES2,argv[2],
					mensaje.rr.name, mensaje.rr.value, mensaje.rr.type);
			      else
				Gestiona_Errores(1,argv[2]);
			    }
			  
			  else
			    Gestiona_Errores(2,argv[2]);
			}
		    }
		  else
		    Gestiona_Errores(3,argv[2]);
		}
	      /*Cerramos el socket*/
	      if (close(s_cliente)<0)
		error_fatal("close");
	    }
	}
    }
  /*En caso de haber abierto el archivo, lo cerramos*/
  if (fent != NULL)
    fclose(fent);
  return error;
}
