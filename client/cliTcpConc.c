/* cliTcpConc.c - Exemplu de client TCP
   Trimite un nume la server; primeste de la server "Hello nume".
         
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#include "../business.h"
#include "../services/ProtocolService.h"
#include "./ActionsHandler.h"

// ProtocolService_
/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  char msg[100] = "";		// mesajul trimis
  char username[20], password[20], role[20];

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

  char code[10];
  int isConnected = 1, codeInt;

  while(isConnected) {
    // Select the command
    bzero (code, 10);
    printf ("[client]Alegeti o comanda: ");
    fflush (stdout);
    read (0, code, 10);

    codeInt = code[0] - '0';
    
    switch (codeInt)
    {
      case LOGIN: 
        printf("Login");
        ActionsHandler_loginUser(sd);
        break;

      case REGISTER:
        printf("Register\n");
        ActionsHandler_registerUser(sd);
        break;

      case ADD_SONG:
        printf("Add song\n");
        ActionsHandler_addSong(sd);
        break;

      case DELETE_SONG:
        printf("Delete song\n");
        ActionsHandler_deleteSong(sd);
        break;

      case VOTE_SONG:
        printf("Vote song\n");
        ActionsHandler_voteSong(sd);
        break;

      case DENY_VOTE:
        printf("Deny vote\n");
        ActionsHandler_denyVote(sd);
        break;

      case DISPLAY_NORMAL:
        printf("Display normal\n");
        ActionsHandler_displaySongsNormal(sd);
        break;

      case DISPLAY_GENRES:
        printf("Display genres\n");
        ActionsHandler_displaySongsByGenres(sd);
        break;

      case ADD_COMMENT:
        printf("Add comment");
        ActionsHandler_addComment(sd);
        break;

      case EXIT:
        printf("Exit\n");
        isConnected = 0;
        ActionsHandler_closeApp(sd);
        break;
    }
  }

  /* inchidem conexiunea, am terminat */
  close (sd);
}
