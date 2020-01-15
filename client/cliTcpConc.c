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
#include "../services/UserService.h"
#include "./ActionsHandler.h"

// ProtocolService_
/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

void Utils_displayMenu(struct User* user);

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

  struct User user;
  UserService_initializeUser(&user);

  Utils_displayMenu(&user);

  while(isConnected) {
    // Select the command
    bzero (code, 10);
    printf ("[client]Alegeti o comanda: ");
    fflush (stdout);
    read (0, code, 10);

    char codeInt = code[0];
    printf("COD-CLIENT: %c\n", codeInt);
    
    switch (codeInt)
    {
      case LOGIN: 
        if(UserService_isLoggedIn(&user)) 
          break;
        printf("Login\n");
        ActionsHandler_loginUser(sd, &user);
        if(UserService_isLoggedIn(&user)) {
          UserService_displayUser(&user);
          Utils_displayMenu(&user);
        }
        break;

      case LOGOUT:
        if(!UserService_isLoggedIn(&user)) 
          break;
        printf("Logout\n");
        AcionsHandler_logoutUser(sd, &user);
        UserService_displayUser(&user);
        Utils_displayMenu(&user);
        break;

      case REGISTER:
        if(UserService_isLoggedIn(&user)) 
          break;
        printf("Register\n");
        ActionsHandler_registerUser(sd, &user);
        UserService_displayUser(&user);
        Utils_displayMenu(&user);
        break;

      case DISPLAY_USERS:
        if(!UserService_isLoggedIn(&user)) 
          break;
        if(!UserService_userIsAdmin(&user)) 
          break;
        printf("Display users\n");
        ActionsHandler_displayUsers(sd);
        break;

      case ADD_SONG:
        if(!UserService_isLoggedIn(&user)) 
          break;
        printf("Add song\n");
        ActionsHandler_addSong(sd);
        break;

      case DELETE_SONG:
        if(!UserService_isLoggedIn(&user)) 
          break;
        if(!UserService_userIsAdmin(&user)) 
          break;
        printf("Delete song\n");
        ActionsHandler_deleteSong(sd);
        break;

      case VOTE_SONG:
        if(!UserService_isLoggedIn(&user)) 
          break;
        printf("Vote song\n");
        ActionsHandler_voteSong(sd);
        break;

      case DENY_VOTE:
        if(!UserService_isLoggedIn(&user)) 
          break;
        if(!UserService_userIsAdmin(&user)) 
          break;
        printf("Deny vote\n");
        ActionsHandler_denyVote(sd);
        break;

      case DISPLAY_NORMAL:
        if(!UserService_isLoggedIn(&user)) 
          break;
        printf("Display normal\n");
        ActionsHandler_displaySongsNormal(sd);
        break;

      case DISPLAY_GENRES:
        if(!UserService_isLoggedIn(&user)) 
          break;
        printf("Display genres\n");
        ActionsHandler_displaySongsByGenres(sd);
        break;

      case ADD_COMMENT:
        if(!UserService_isLoggedIn(&user)) 
          break;
        printf("Add comment\n");
        ActionsHandler_addComment(sd);
        break;

      case DISPLAY_COMMENTS:
        if(!UserService_isLoggedIn(&user)) 
          break;
        printf("Display comments\n");
        ActionsHandler_displayComments(sd);
        break;

      case EXIT:
        printf("Exit\n");
        isConnected = 0;
        printf("La revedere!\n");
        ActionsHandler_closeApp(sd);
        break;
    }
  }

  /* inchidem conexiunea, am terminat */
  close (sd);
}

void Utils_displayMenu(struct User* user) {
  int isLoggedIn = UserService_isLoggedIn(user);

  printf("\n== MENU ==\n");

  if(!isLoggedIn) {
    printf("Login: %c\n", LOGIN);
    printf("Register: %c\n", REGISTER);
  } else {
    printf("Logout: %c\n", LOGOUT);
  }

  if(isLoggedIn) {
    printf("Display songs: %c\n", DISPLAY_NORMAL);
    printf("Display songs by genres: %c\n", DISPLAY_GENRES);

    printf("Add a song: %c\n", ADD_SONG);
    printf("Vote a song: %c\n", VOTE_SONG);

    printf("Add a comment to a song: %c\n", ADD_COMMENT);
    printf("Display comments to a song: %c\n", DISPLAY_COMMENTS);

    if(UserService_userIsAdmin(user)) {
      printf("Modify the ability to vote: %c\n", DENY_VOTE);
      printf("Delete a song: %c\n", DELETE_SONG);
      printf("Display users: %c\n", DISPLAY_USERS);
    }
  }

  printf("Exit: %c\n", EXIT);

  printf("== END ==\n\n");
}