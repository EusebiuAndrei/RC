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

#define READ_CLIENT "[client]Eroare la read() de la server.\n"
#define READ_SERVER "[server]Eroare la read() de la client.\n"
#define WRITE_CLIENT "[client]Eroare la write() spre server.\n"
#define WRITE_SERVER "[server]Eroare la write() catre client.\n"

void ProtocolService_readField(char *field, char *numeField);
void ProtocolService_readResponse(int sd, char* sir, int length, char *errorMsg);
void ProtocolService_sendResponse(int sd, char* sir, int length, char *errorMsg);
void ProtocolService_prepareToRead(char *msg, int length);
void ProtocolService_createMsg(char *msg, int length, int isServer, char *linker, int num, ...);

void ProtocolService_readField(char *field, char *numeField) {
  bzero (field, 20);
  
  printf ("Enter %s", numeField);
  if(!strcmp(numeField, "role")) {
    printf("[normal/admin]");
  }
  printf (": ");
  fflush (stdout);
  
  read (0, field, 20);
  // printf("%s: %s", numeField, field);
}

void ProtocolService_readResponse(int sd, char* sir, int length, char *errorMsg) {
  /* citirea raspunsului dat de server 
     (apel blocant pina cind serverul raspunde) */
  if (read (sd, sir, length) < 0) {
    perror (errorMsg);
    // return errno;
  }
}

void ProtocolService_sendResponse(int sd, char* sir, int length, char *errorMsg) {
  /* trimiterea mesajului la server */
  if (write (sd, sir, length) <= 0) {
    perror (errorMsg);
    // return errno;
  }
}

void ProtocolService_prepareToRead(char *msg, int length) {
    /* s-a realizat conexiunea, se astepta mesajul */
	bzero (msg, length);
	printf ("[server]Asteptam mesajul...\n");
	fflush (stdout);
}

void ProtocolService_createMsg(char *msg, int length, int isServer, char *linker, int num, ...) {
  va_list valist;
  char sir[100];
  int i;

  bzero(msg, length);
  /* initialize valist for num number of arguments */
  va_start(valist, num);

  bzero(sir, 100);
  strcpy(sir, va_arg(valist, char*));

  if(!isServer) {
    strncat(msg, sir, strlen(sir) - 1);
  } else {
    strncat(msg, sir, strlen(sir));
  }

//  printf("AAAAAAA: %s\n", msg);

  /* access all the arguments assigned to valist */
  for (i = 1; i < num; i++) {
    bzero(sir, 100);
    strcpy(sir, va_arg(valist, char*));
    strcat(msg, linker);

    if(!isServer) {
        strncat(msg, sir, strlen(sir) - 1);
    } else {
        strncat(msg, sir, strlen(sir));
    }
  }
	
  /* clean memory reserved for valist */
  va_end(valist);
}

void ProtocolService_destructureMsg(char msg[100], int num, ...) {
  va_list valist;
  int i;

  bzero(msg, 100);
  /* initialize valist for num number of arguments */
  va_start(valist, num);

  /* access all the arguments assigned to valist */
  char *sir = strtok(msg, ":"), exemplu[20], *copyArg;
  printf("%s\n", msg);
  printf("%s\n", sir);
  while(sir) {
      printf("%s\n\n", sir);
    copyArg = va_arg(valist, char*);
    
    copyArg = sir;

    sir = strtok(NULL, ":");
  }

  /* clean memory reserved for valist */
  va_end(valist);
}