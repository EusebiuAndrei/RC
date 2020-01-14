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

void ActionsHandler_loginUser(int sd);
void ActionsHandler_registerUser(int sd);

void ActionsHandler_addSong(int sd);
void ActionsHandler_deleteSong(int sd);

void ActionsHandler_voteSong(int sd);
void ActionsHandler_denyVote(int sd);

void ActionsHandler_displaySongsNormal(int sd);
void ActionsHandler_displaySongsByGenres(int sd);

void ActionsHandler_addComment(int sd);

void ActionsHandler_closeApp(int sd);


void ActionsHandler_loginUser(int sd) {
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
  printf ("[client]Mesajul primit este: %s\n", msg);
}

void ActionsHandler_registerUser(int sd) {
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
    ActionsHandler_registerUser(sd);
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
  printf ("[client]Mesajul primit este: %s\n", msg);
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
  printf("Mesajul este: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 100, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 100, READ_CLIENT);
  /* afisam mesajul primit */
  printf ("[client]Mesajul primit este: %s\n", msg);
}

void ActionsHandler_deleteSong(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char code[10] = "-";
  char id_song[10];

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

void ActionsHandler_displaySongsNormal(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char code[10] = "5";

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
  
  ProtocolService_readResponse(sd, msg, 1000, READ_CLIENT);
  printf ("%s", msg);
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
  printf ("%s", msg);
}

void ActionsHandler_voteSong(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char code[10] = "7";
  char id_song[10];

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
  
  ProtocolService_readField(id_song, "id_song");

  ProtocolService_createMsg(msg, 1000, 0, ":", 1, id_song);
  printf("MESSAGE: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 1000, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 1000, READ_CLIENT);
  printf ("%s", msg);
}

void ActionsHandler_denyVote(int sd) {
  char msg[1000] = "";		// mesajul trimis
  char code[10] = "9";
  char username[20];

  ProtocolService_sendResponse(sd, code, 10, WRITE_CLIENT);
  
  ProtocolService_readField(username, "username");

  ProtocolService_createMsg(msg, 1000, 0, ":", 1, username);
  printf("MESSAGE: %s\n", msg);

  ProtocolService_sendResponse(sd, msg, 1000, WRITE_CLIENT);

  ProtocolService_readResponse(sd, msg, 1000, READ_CLIENT);
  printf ("%s", msg);
}

void ActionsHandler_addComment(int sd) {
  char msg[100] = "";		// mesajul trimis
  char id_song[20], username[20], text[20];
  char code[10] = "8";

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
  printf ("[client]Mesajul primit este: %s\n", msg);
}