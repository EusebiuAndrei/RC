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
#include "DBService.h"
#include "business.h"

/* portul folosit */
#define PORT 2024

/* codul de eroare returnat de anumite apeluri */
extern int errno;

void loginUser(int client, struct User* user);
void registerUser(int client, struct User* user);
void validateUsername(int client, struct User* user);

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
					close (client);	/* inchidem conexiunea cu clientul */
					continue;		/* continuam sa ascultam */
				}

				printf ("[server]Mesajul a fost receptionat...%s\n", msg);

				char codeInt = msg[0] - '0';

				printf("CODE\n");
				printf("%s\n", msg);
				printf("codeInt: %d\n", codeInt);

				switch (codeInt)
				{
					case LOGIN:
						loginUser(client, &user);
						printf("Login\n");
						UserService_displayUser(&user);
						break;

					case REGISTER:
						registerUser(client, &user);
						printf("Register\n");
						UserService_displayUser(&user);
						break;

					case VALIDATE_USERNAME:
						printf("Validate username\n");
						validateUsername(client, &user);
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

void validateUsername(int client, struct User* user) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client

	/* s-a realizat conexiunea, se astepta mesajul */
	bzero (msg, 100);
	printf ("[server]Asteptam mesajul...\n");
	fflush (stdout);

	/* citirea mesajului */
	if (read (client, msg, 100) <= 0)
	{
		perror ("[server]Eroare la read() de la client.\n");
		close (client);	/* inchidem conexiunea cu clientul */
	}

	int userExists = DBService_userExists(msg);

	/*pregatim mesajul de raspuns */
	bzero(msgrasp,100);

	if(userExists) {
		strcat(msgrasp, "[Error]: This username has already been taken!");
	} else {
		strcat(msgrasp, "OK");
	}

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);

	/* returnam mesajul clientului */
	if (write (client, msgrasp, 100) <= 0)
	{
		perror ("[server]Eroare la write() catre client.\n");
	}
	else
		printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void loginUser(int client, struct User* user) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client

	/* s-a realizat conexiunea, se astepta mesajul */
	bzero (msg, 100);
	printf ("[server]Asteptam mesajul...\n");
	fflush (stdout);

	/* citirea mesajului */
	if (read (client, msg, 100) <= 0)
	{
		perror ("[server]Eroare la read() de la client.\n");
		close (client);	/* inchidem conexiunea cu clientul */
	}

	char username[20], password[20];
	char *sir = strtok(msg, ":");
	int i = 0;

	while(sir) {
		if(i == 0) 
			strcpy(username, sir);
		if(i == 1)
			strcpy(password, sir);

		i++;
		sir = strtok(NULL, ":");
	}

	printf("USERNAME: %s\n", username);
	printf("PASSWORD: %s\n", password);

	// Apelam baza de date
	int code = DBService_loginUser(username, password, user);
	printf("Apel DB: %d\n", code);

	/*pregatim mesajul de raspuns */
	bzero(msgrasp,100);
	//strcat(msgrasp,"Error: ");
	//strcat(msgrasp,msg);

	switch (code) {
		case 1:
			strcat(msgrasp, "[Error]: The username or password is wrong!");
			break;

		case -1:
			strcat(msgrasp, "[Error]: There was a problem with the DB");
			break;
		
		default:
			strcat(msgrasp, "OK");
			break;
	}

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);

	/* returnam mesajul clientului */
	if (write (client, msgrasp, 100) <= 0)
	{
		perror ("[server]Eroare la write() catre client.\n");
	}
	else
		printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void registerUser(int client, struct User* user) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client

	/* s-a realizat conexiunea, se astepta mesajul */
	bzero (msg, 100);
	printf ("[server]Asteptam mesajul...\n");
	fflush (stdout);

	/* citirea mesajului */
	if (read (client, msg, 100) <= 0)
	{
		perror ("[server]Eroare la read() de la client.\n");
		close (client);	/* inchidem conexiunea cu clientul */
	}

	char username[20], password[20], role[20];
	char *sir = strtok(msg, ":");
	int i = 0;

	while(sir) {
		if(i == 0) 
			strcpy(username, sir);
		if(i == 1)
			strcpy(password, sir);
		if(i == 2)
			strcpy(role, sir);

		i++;
		sir = strtok(NULL, ":");
	}

	printf("USERNAME: %s\n", username);
	printf("PASSWORD: %s\n", password);
	printf("ROLE: %s\n", role);

	// Apelam baza de date
	int code = DBService_registerUser(role, username, password, user);
	printf("Apel DB: %d\n", code);

	/*pregatim mesajul de raspuns */
	bzero(msgrasp,100);
	strcat(msgrasp,"Hello ");
	strcat(msgrasp,msg);

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);

	/* returnam mesajul clientului */
	if (write (client, msgrasp, 100) <= 0)
	{
		perror ("[server]Eroare la write() catre client.\n");
	}
	else
		printf ("[server]Mesajul a fost trasmis cu succes.\n");
}