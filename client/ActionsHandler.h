#pragma once

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

void ActionsHandler_loginUser(int sd, struct User* user);
void AcionsHandler_logoutUser(int sd, struct User* user);
void ActionsHandler_registerUser(int sd, struct User* user);
int ActionsHandler_displayUsers(int sd);

void ActionsHandler_addSong(int sd);
void ActionsHandler_deleteSong(int sd);

void ActionsHandler_voteSong(int sd);
void ActionsHandler_denyVote(int sd);

int ActionsHandler_displaySongsNormal(int sd);
void ActionsHandler_displaySongsByGenres(int sd);

void ActionsHandler_addComment(int sd);
void ActionsHandler_displayComments(int sd);

void ActionsHandler_closeApp(int sd);


void ActionsHandler_loginUser(int sd, struct User* user) {
  char msg[100] = "";		// mesajul trimis
  char username[20], password[20];
  char code[10] = "0";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);

  ProtocolService_readField(username, "username");
  ProtocolService_readField(password, "password");

  ProtocolService_createMsg(msg, 100, 0, ":", 2, username, password);
  printf("MESSAGE: %s\n", msg);
  
  ProtocolService_sendResponse(sd, msg, 100, WRITE_CLIENT);
  
  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);

  if(strstr(msg, "[Error]")) {
    ActionsHandler_loginUser(sd, user);
    printf ("[client]: %s\n", msg);
  }

  char role[10], canVote[10];
  char *sir = strtok(msg, ":");
	int i = 0;

	while(sir) {
		if(i == 0) 
			strcpy(role, sir);
		if(i == 1)
			strcpy(canVote, sir);

		i++;
		sir = strtok(NULL, ":");
	}

  char usernameUser[20]="", passwordUser[20]="";
  int canVoteInt = canVote[0] - '0';
  strncat(usernameUser, username, strlen(username) - 1);
  strncat(passwordUser, password, strlen(password) - 1);

  UserService_createUser(usernameUser, passwordUser, role, canVoteInt, user);
  printf("Login suceeded\n");
}

void AcionsHandler_logoutUser(int sd, struct User* user) {
  char msg[100] = "";		// mesajul trimis
  char code[10] = "#";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
  
  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);
  printf ("[client]Mesajul primit este: %s\n", msg);

  UserService_initializeUser(user);

  printf("Logout succeeded\n");
}

void ActionsHandler_registerUser(int sd, struct User* user) {
  char msg[100] = "";		// mesajul trimis
  char username[20], password[20], role[20];
  char code[10] = "3";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);

  ProtocolService_readField(username, "username");

  ProtocolService_createMsg(msg, 100, 0, ":", 1, username);
  printf("codul este %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 100, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);
  printf("%s\n", msg);

  // Username-ul exista deja - needs retry
  if(strcmp(msg, "OK")) {
    ActionsHandler_registerUser(sd, user);
    return;
  }

  strcpy(code, "1");
  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);

  ProtocolService_readField(password, "password");
  ProtocolService_readField(role, "role");

  ProtocolService_createMsg(msg, 100, 0, ":", 3, username, password, role);
  printf("Mesajul este: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 100, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);

  char usernameUser[20]="", passwordUser[20]="", roleUser[20]="";
  strncat(usernameUser, username, strlen(username) - 1);
  strncat(passwordUser, password, strlen(password) - 1);
  strncat(roleUser, role, strlen(role) - 1);

  UserService_createUser(usernameUser, passwordUser, roleUser, 1, user);
  printf("Register succeded\n");

  printf ("[client]Mesajul primit este: %s\n", msg);
}

int ActionsHandler_displayUsers(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char code[10] = "$";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
  
  ProtocolService_readResponse(sd, msg, 1000, READ_CLIENT);
  printf ("%s\n", msg);

  if(strstr(msg, "[Error]") || strstr(msg, "[Res]")) {
    return 1;
  }
  return 0;
}

void ActionsHandler_addSong(int sd) {
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
  ProtocolService_createMsg(msg, 100, 0, ":", 4, title, description, genres, link);
  // printf("Mesajul este: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 100, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);
  
  if(!strcmp(msg, "OK")) {
    printf("[Result]: Song added\n");
    return;
  }

  printf("%s\n", msg);
}

void ActionsHandler_deleteSong(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char code[10] = "-";
  char id_song[10];

  if(ActionsHandler_displaySongsNormal(sd)) {
    return;
  }

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
  
  ProtocolService_readField(id_song, "id_song");

  ProtocolService_createMsg(msg, 1000, 0, ":", 1, id_song);
  printf("MESSAGE: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 1000, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 1000, READ_CLIENT);
  printf ("%s", msg);
}

void ActionsHandler_closeApp(int sd) {
  char code[10] = "2";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
}

int ActionsHandler_displaySongsNormal(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char code[10] = "5";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
  
  ProtocolService_readResponse(sd, msg, 1000, READ_CLIENT);
  printf ("%s\n", msg);

  if(strstr(msg, "[Error]") || strstr(msg, "[Res]")) {
    return 1;
  }
  return 0;
}

void ActionsHandler_displaySongsByGenres(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char genre[20];
  char code[10] = "6";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);

  ProtocolService_readField(genre, "genre");

  ProtocolService_createMsg(msg, 1000, 0, ":", 1, genre);
  printf("MESSAGE: %s\n", msg);
  
  ProtocolService_sendResponse(sd, msg, 1000, WRITE_CLIENT);
  
  ProtocolService_readResponse(sd, msg, 1000, READ_CLIENT);
  printf ("%s\n", msg);
}

void ActionsHandler_voteSong(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char code[10] = "7";
  char id_song[10];

  if(ActionsHandler_displaySongsNormal(sd)) {
    return;
  }

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
  
  ProtocolService_readField(id_song, "id_song");

  ProtocolService_createMsg(msg, 1000, 0, ":", 1, id_song);
  printf("MESSAGE: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 1000, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 1000, READ_CLIENT);

  if(!strcmp(msg, "OK")) {
    printf("[Result]: Voted\n");
    return;
  }

  printf ("%s\n", msg);
}

void ActionsHandler_denyVote(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char code[10] = "9";
  char username[20];

  if(ActionsHandler_displayUsers(sd)) {
    return;
  }

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
  
  ProtocolService_readField(username, "username");

  ProtocolService_createMsg(msg, 1000, 0, ":", 1, username);
  printf("MESSAGE: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 1000, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 1000, READ_CLIENT);
  printf ("%s\n", msg);
}

void ActionsHandler_addComment(int sd) {
  char msg[100] = "";		// mesajul trimis
  char id_song[20], username[20], text[20];
  char code[10] = "8";

  if(ActionsHandler_displaySongsNormal(sd)) {
    return;
  }

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);

  // citirea date
  ProtocolService_readField(id_song, "id_song");
  ProtocolService_readField(text, "text");

  // creare mesaj
  ProtocolService_createMsg(msg, 100, 0, ":", 2, id_song, text);
  printf("Mesajul este: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 100, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);
  /* afisam mesajul primit */

  if(!strcmp(msg, "OK")) {
    printf("[Result]: Comment added");
    return;
  }

  printf ("%s\n", msg);
}

void ActionsHandler_displayComments(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char id_song[20];
  char code[10] = "*";

  if(ActionsHandler_displaySongsNormal(sd)) {
    return;
  }

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);

  ProtocolService_readField(id_song, "id_song");

  ProtocolService_createMsg(msg, 1000, 0, ":", 1, id_song);
  printf("MESSAGE: %s\n", msg);
  
  ProtocolService_sendResponse(sd, msg, 1000, WRITE_CLIENT);
  
  ProtocolService_readResponse(sd, msg, 1000, READ_CLIENT);
  printf ("%s", msg);
}