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

void ActionsHandler_loginUser(int client, struct User* user);
void ActionsHandler_logoutUser(int client, struct User* user);
void ActionsHandler_registerUser(int client, struct User* user);
void ActionsHandler_validateUsername(int client, struct User* user);
void ActionsHandler_displayUsers(int client);

void ActionsHandler_displayUserNormal(int client);
void ActionsHandler_displaySongsByGenres(int client);

void ActionsHandler_addSong(int client);
void ActionsHandler_deleteSong(int client);

void ActionsHandler_voteSong(int client);
void ActionsHandler_denyVote(int client);

void ActionsHandler_addComment(int client, struct User* user);
void ActionsHandler_displayComments(int client);

void ActionsHandler_validateUsername(int client, struct User* user) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[100]="";        //mesaj de raspuns pentru client

	ProtocolService_prepareToRead(msg, 100);
	ProtocolService_readResponse(client, msg, 100, READ_SERVER);
	printf("message for validate-user: %s\n", msg);

	int userExists = DBService_userExists(msg);

	/*pregatim mesajul de raspuns */
	if(userExists) {
		ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "[Error]: This username has already been taken!");
	} else {
		ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "OK");
	}

	printf("userExists: %s\n", msgrasp);

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);
	/* returnam mesajul clientului */
	ProtocolService_sendResponse(client, msgrasp, 100, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_loginUser(int client, struct User* user) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client

	ProtocolService_prepareToRead(msg, 100);
	ProtocolService_readResponse(client, msg, 100, READ_SERVER);

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
	char canVoteChar[10] = "";
	printf("Apel DB: %d\n", code);
	UserService_displayUser(user);

	switch (code) {
		case 1:
			ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "[Error]: The username or password is wrong!");
			break;
		case -1:
			ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "[Error]: There was a problem with the DB");
			break;
		default:
			strcpy(canVoteChar, user->canVote == 0 ? "0" : "1");
			printf("SENT BACK: %s %s", user->role, canVoteChar);
			ProtocolService_createMsg(msgrasp, 100, 1, ":", 2, user->role, canVoteChar);
			break;
	}

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 100, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_logoutUser(int client, struct User* user) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[100]="";        //mesaj de raspuns pentru client

	// Apelam baza de date
	UserService_initializeUser(user);

	ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "OK");

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 100, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_registerUser(int client, struct User* user) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client

	ProtocolService_prepareToRead(msg, 100);
	ProtocolService_readResponse(client, msg, 100, READ_SERVER);

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
	ProtocolService_createMsg(msgrasp, 100, 1, " ", 2, "Hello ", msg);

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 100, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_displayUsers(int client) {
	char msg[1000];		//mesajul primit de la client
    char msgrasp[1000];        //mesaj de raspuns pentru client


	// Apelam baza de date
	bzero(msgrasp, 1000);
	int code = DBService_getUsers(msgrasp);
	printf("Apel DB: %d\n", code);
	printf("Another one\n%s", msgrasp);

	switch (code) {
		case 0:
			ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "OK");
			break;
		
		case 1:
			ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Res]: There are no users");
			break;

		case -1:
			ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Error]: There was a problem with the DB");
			break;
	}

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 1000, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_addSong(int client) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client

	ProtocolService_prepareToRead(msg, 100);
	ProtocolService_readResponse(client, msg, 100, READ_SERVER);

	char title[20], description[20], genres[20], link[20];
	char *sir = strtok(msg, ":");
	int i = 0;

	while(sir) {
		if(i == 0) 
			strcpy(title, sir);
		if(i == 1)
			strcpy(description, sir);
		if(i == 2)
			strcpy(genres, sir);
		if(i == 3)
			strcpy(link, sir);

		i++;
		sir = strtok(NULL, ":");
	}

	printf("TITLE: %s\n", title);
	printf("DESCRIPTION: %s\n", description);
	printf("GENRES: %s\n", genres);
	printf("LINK: %s\n", link);

	// Apelam baza de date
	int code = DBService_addSong(title, description, link, genres);
	printf("Apel DB: %d\n", code);

	/*pregatim mesajul de raspuns */
	switch(code) {
		case 0:
			ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "OK");
			break;

		case 1:
			ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "[Error]: Something went wrong. Try another song!");
			break;

		case -1:
			ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "[Error]: There was an error with the DB");
			break;	
	}

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 100, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_deleteSong(int client) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[1000];        //mesaj de raspuns pentru client

	ProtocolService_prepareToRead(msg, 1000);
	ProtocolService_readResponse(client, msg, 1000, READ_SERVER);

	printf("ID_SONG: %s\n", msg);

	// Apelam baza de date
	bzero(msgrasp, 1000);
	int code = DBService_removeSong(msg);
	printf("Apel DB: %d\n", code);

	if(code) {
		ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Error]: There was a problem with the DB");
	} else {
		ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "OK");
	}

	printf("[server]Trimitem mesajul inapoi\n%s",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 1000, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_displayUserNormal(int client) {
	char msg[1000];		//mesajul primit de la client
    char msgrasp[1000];        //mesaj de raspuns pentru client


	// Apelam baza de date
	bzero(msgrasp, 1000);
	int code = DBService_displaySongs(msgrasp);
	printf("Apel DB: %d\n", code);
	printf("Another one\n%s", msgrasp);

	switch (code) {
		case 1:
			ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Res]: There are no songs");
			break;
		
		case -1:
			ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Error]: There was a problem with the DB");
			break;
	}

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 1000, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_displaySongsByGenres(int client) {
	char msg[1000];		//mesajul primit de la client
    char msgrasp[1000];        //mesaj de raspuns pentru client

	ProtocolService_prepareToRead(msg, 1000);
	ProtocolService_readResponse(client, msg, 1000, READ_SERVER);

	printf("GEN: %s\n", msg);

	// Apelam baza de date
	int code = DBService_displaySongsByGenres(msg, msgrasp);
	printf("Apel DB: %d\n", code);

	switch (code) {
		case 1:
			ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Res]: There are no songs with this genre");
			break;
		
		case -1:
			ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Error]: There was a problem with the DB");
			break;
	}

	printf("[server]Trimitem mesajul inapoi\n%s",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 1000, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_voteSong(int client) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[1000];        //mesaj de raspuns pentru client

	ProtocolService_prepareToRead(msg, 1000);
	ProtocolService_readResponse(client, msg, 1000, READ_SERVER);

	printf("ID_SONG: %s\n", msg);

	// Apelam baza de date
	bzero(msgrasp, 1000);
	int code = DBService_voteSong(msg);
	printf("Apel DB: %d\n", code);

	if(code) {
		ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Error]: There was a problem with the DB");
	} else {
		ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "OK");
	}

	printf("[server]Trimitem mesajul inapoi\n%s",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 1000, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_denyVote(int client) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[1000];        //mesaj de raspuns pentru client

	ProtocolService_prepareToRead(msg, 1000);
	ProtocolService_readResponse(client, msg, 1000, READ_SERVER);

	printf("USERNAME: %s\n", msg);

	// Apelam baza de date
	bzero(msgrasp, 1000);
	int code = DBService_denyVotes(msg);
	printf("Apel DB: %d\n", code);

	if(code) {
		ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Error]: There was a problem with the DB");
	} else {
		ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "OK");
	}

	printf("[server]Trimitem mesajul inapoi\n%s",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 1000, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_addComment(int client, struct User* user) {
	char msg[100];		//mesajul primit de la client
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client

	ProtocolService_prepareToRead(msg, 100);
	ProtocolService_readResponse(client, msg, 100, READ_SERVER);

	char id_song[20], text[20];
	char *sir = strtok(msg, ":");
	int i = 0;

	while(sir) {
		if(i == 0) 
			strcpy(id_song, sir);
		if(i == 1)
			strcpy(text, sir);
		i++;
		sir = strtok(NULL, ":");
	}

	printf("ID_SONG: %s\n", id_song);
	printf("TEXT: %s\n", text);
	printf("USERNAME: %s\n", user->username);

	// Apelam baza de date
	int code = DBService_addComment(user->username, id_song, text);
	printf("Apel DB: %d\n", code);

	/*pregatim mesajul de raspuns */
	switch(code) {
		case 0:
			ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "OK");
			break;

		case 1:
			ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "[Res]: There is not a comment with this id");
			break;

		case -1:
			ProtocolService_createMsg(msgrasp, 100, 1, " ", 1, "[Error]: There is a problem with the DB");
			break;
	}

	printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 100, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}

void ActionsHandler_displayComments(int client) {
	char msg[1000];		//mesajul primit de la client
    char msgrasp[1000];        //mesaj de raspuns pentru client

	ProtocolService_prepareToRead(msg, 1000);
	ProtocolService_readResponse(client, msg, 1000, READ_SERVER);

	printf("ID_SONG: %s\n", msg);

	// Apelam baza de date
	int code = DBService_getCommentsForSong(msg, msgrasp);
	printf("Apel DB: %d\n", code);

	switch (code) {
		case 1:
			ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Res]: There are no comments for this song\n");
			break;
		
		case -1:
			ProtocolService_createMsg(msgrasp, 1000, 1, " ", 1, "[Error]: There was a problem with the DB\n");
			break;
	}

	printf("[server]Trimitem mesajul inapoi\n%s",msgrasp);
	ProtocolService_sendResponse(client, msgrasp, 1000, WRITE_SERVER);
	printf ("[server]Mesajul a fost trasmis cu succes.\n");
}