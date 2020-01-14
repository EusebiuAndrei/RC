 /* servTcpConc.c - Exemplu de server TCP concurent
    Asteapta un nume de la clienti; intoarce clientului sirul
    "Hello nume".
    */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sqlite3.h>
#include "../DBService.h"
#include "../business.h"
#include "../services/ProtocolService.h"
#include "../services/UserService.h"
#include "./ActionsHandler.h"

/* portul folosit */
#define PORT 2024

/* codul de eroare returnat de anumite apeluri */
extern int errno;

int main ()
{
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    char msg[100];		//mesajul primit de la client
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client
    int sd;			//descriptorul de socket

    /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
    	perror ("[server]Eroare la socket().\n");
    	return errno;
    }

    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

    /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
    	perror ("[server]Eroare la bind().\n");
    	return errno;
    }

	// Initializam baza de date
	DBService_initializeDB();

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 1) == -1)
    {
    	perror ("[server]Eroare la listen().\n");
    	return errno;
    }

    /* servim in mod concurent clientii... */
    while (1)
    {
    	int client;
    	int length = sizeof (from);
		
		struct User user;
		UserService_initializeUser(&user);

    	printf ("[server]Asteptam la portul %d...\n",PORT);
    	fflush (stdout);

    	/* acceptam un client (stare blocanta pina la realizarea conexiunii) */
    	client = accept (sd, (struct sockaddr *) &from, &length);

    	/* eroare la acceptarea conexiunii de la un client */
    	if (client < 0)
    	{
    		perror ("[server]Eroare la accept().\n");
    		continue;
    	}

    	int pid;
    	if ((pid = fork()) == -1) {
    		close(client);
    		continue;
    	} else if (pid > 0) {
    		// parinte
    		close(client);
    		while(waitpid(-1,NULL,WNOHANG));
    		continue;
    	} else if (pid == 0) {
    		// copil
    		close(sd);

    		/* s-a realizat conexiunea, se astepta mesajul */

			int isConnected = 1;

			while(isConnected) {

				bzero (msg, 100);
				printf ("[server]Asteptam mesajul...\n");
				fflush (stdout);

				/* citirea mesajului */
				if (read (client, msg, 100) <= 0)
				{
					perror ("[server]Eroare la read() de la client.\n");
					close (client);
					exit(2);	/* inchidem conexiunea cu clientul */
					continue;		/* continuam sa ascultam */
				}

				printf ("[server]Mesajul a fost receptionat...%s\n", msg);

				char codeInt = msg[0];

				printf("SIR: %s\n", msg);
				printf("CODE: %c\n", codeInt);

				switch (codeInt)
				{
					case LOGIN:
						ActionsHandler_loginUser(client, &user);
						printf("Login\n");
						UserService_displayUser(&user);
						break;

					case LOGOUT:
						ActionsHandler_logoutUser(client, &user);
						printf("Logout\n");
						UserService_displayUser(&user);
						break;

					case REGISTER:
						ActionsHandler_registerUser(client, &user);
						printf("Register\n");
						UserService_displayUser(&user);
						break;

					case VALIDATE_USERNAME:
						printf("Validate username\n");
						ActionsHandler_validateUsername(client, &user);
						break;

					case ADD_SONG:
						printf("Add song\n");
						ActionsHandler_addSong(client);
						break;

					case DELETE_SONG:
						printf("Delete song\n");
						ActionsHandler_deleteSong(client);
						break;

					case VOTE_SONG:
						printf("Vote song\n");
						ActionsHandler_voteSong(client);
						break;

					case DENY_VOTE:
						printf("Deny vote\n");
						ActionsHandler_denyVote(client);
						break;

					case DISPLAY_NORMAL:
						printf("Display normal\n");
						ActionsHandler_displayUserNormal(client);
						break;

					case DISPLAY_GENRES:
						printf("Display genres\n");
						ActionsHandler_displaySongsByGenres(client);
						break;

					case ADD_COMMENT:
						printf("Add comment\n");
						ActionsHandler_addComment(client, &user);
						break;
					
					case EXIT:
						printf("Exit\n");
						isConnected = 0;
						break;
				}
				
				printf("FINISHED\n");

			}

			printf("Job done with this client\n");
    		
    		/* am terminat cu acest client, inchidem conexiunea */
    		close (client);
    		exit(0);
    	}

    }				/* while */

	// Inchidem baza de date
	DBService_closeDB();
}				/* main */

