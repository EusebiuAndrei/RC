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
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#include "business.h"

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

void registerUser(int sd);
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
        break;

      case REGISTER:
        printf("Register\n");
        registerUser(sd);
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

void registerUser(int sd) {
  char msg[100] = "";		// mesajul trimis
  char username[20], password[20], role[20];
  char code[10] = "1";

  /* trimiterea mesajului la server */
  if (write (sd, code, 10) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      // return errno;
    }

  // citirea date
  bzero (username, 20);
  printf ("[client]Introduceti un nume: ");
  fflush (stdout);
  read (0, username, 20);
  printf("USERNAME: %s", username);

  bzero (password, 20);
  printf ("[client]Introduceti o parola: ");
  fflush (stdout);
  read (0, password, 20);
  printf("PASSWORD: %s", password);

  bzero (role, 20);
  printf ("[client]Alegeti tipul contului: ");
  fflush (stdout);
  read (0, role, 20);
  printf("ROLE: %s", role);

  // creare mesaj
  strncat(msg, username, strlen(username) - 1);
  strcat(msg, ":");
  strncat(msg, password, strlen(password) - 1);
  strcat(msg, ":");
  strncat(msg, role, strlen(role) - 1);

  printf("Mesajul este: %s\n", msg);

  /* trimiterea mesajului la server */
  if (write (sd, msg, 100) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      // return errno;
    }

  /* citirea raspunsului dat de server 
     (apel blocant pina cind serverul raspunde) */
  if (read (sd, msg, 100) < 0)
    {
      perror ("[client]Eroare la read() de la server.\n");
      // return errno;
    }
  /* afisam mesajul primit */
  printf ("[client]Mesajul primit este: %s\n", msg);
}

void closeApp(int sd) {
  char code[10] = "2";

  /* trimiterea mesajului la server */
  if (write (sd, code, 10) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      // return errno;
    }
}