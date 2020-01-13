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

#include "business.h"
#include "ProtocolService.h"

// ProtocolService_
/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

void loginUser(int sd);
void registerUser(int sd);
void addSong(int sd);
void closeApp(int sd);

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
        loginUser(sd);
        break;

      case REGISTER:
        printf("Register\n");
        registerUser(sd);
        break;

      case ADD_SONG:
        printf("Add song\n");
        addSong(sd);
        break;

      case EXIT:
        printf("Exit\n");
        isConnected = 0;
        closeApp(sd);
        break;
    }
  }

  /* inchidem conexiunea, am terminat */
  close (sd);
}

void loginUser(int sd) {
  char msg[100] = "";		// mesajul trimis
  char username[20], password[20];
  char code[10] = "0";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);

  ProtocolService_readField(username, "username");
  ProtocolService_readField(password, "password");

  ProtocolService_createMsg(msg, 0, ":", 2, username, password);
  printf("MESSAGE: %s\n", msg);
  
  ProtocolService_sendResponse(sd, msg, 100, WRITE_CLIENT);
  
  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);
  printf ("[client]Mesajul primit este: %s\n", msg);
}

void registerUser(int sd) {
  char msg[100] = "";		// mesajul trimis
  char username[20], password[20], role[20];
  char code[10] = "3";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);

  ProtocolService_readField(username, "username");

  ProtocolService_createMsg(msg, 0, ":", 1, username);
  printf("codul este %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 100, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);
  printf("%s\n", msg);

  // Username-ul exista deja - needs retry
  if(strcmp(msg, "OK")) {
    registerUser(sd);
    return;
  }

  strcpy(code, "1");
  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);

  ProtocolService_readField(password, "password");
  ProtocolService_readField(role, "role");

  ProtocolService_createMsg(msg, 0, ":", 3, username, password, role);
  printf("Mesajul este: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 100, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);
  printf ("[client]Mesajul primit este: %s\n", msg);
}

void addSong(int sd) {
  char msg[100] = "";		// mesajul trimis
  char title[20], description[20], genres[20], link[20];
  char code[10] = "4";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);

  // citirea date
  ProtocolService_readField(title, "title");
  ProtocolService_readField(description, "description");
  ProtocolService_readField(link, "link");
  ProtocolService_readField(genres, "genres");

  // creare mesaj
  ProtocolService_createMsg(msg, 0, ":", 4, title, description, genres, link);
  printf("Mesajul este: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 100, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);
  /* afisam mesajul primit */
  printf ("[client]Mesajul primit este: %s\n", msg);
}

void closeApp(int sd) {
  char code[10] = "2";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
}