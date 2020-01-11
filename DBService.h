#pragma once
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#define QUERY_LENGTH 500

#define USERS_ROW_LENGTH 5
#define SONGS_ROW_LENGTH 6
#define COMMENTS_ROW_LENGTH 4

#define USERS "users"
#define SONGS "songs"
#define COMMENTS "comments"

sqlite3 *DB;
char *errorMessage;
int command;
sqlite3_stmt* capat;

void getRowInfo(char *tabel);
void getUsers();

void _bindParemter(char query[QUERY_LENGTH], char *value);
int DBService_initializeDB();
int DBService_loginUser(char *username, char *password);
int DBService_registerUser(char *role, char* username, char* password);

int DBService_initializeDB() {
    command = sqlite3_open("./top-music", &DB);

    if(command) {
        fprintf(stderr, "The db could not be open: %s.\n", sqlite3_errmsg(DB));
        return 1;
    } else {
        fprintf(stderr, "I've opened the DB.\n");
        return 0;
    }
};

void _bindParemter(char query[QUERY_LENGTH], char *value) {
    char *firstParam = strchr(query, '?');
    
    int diff = firstParam - query;
    char finalQuery[QUERY_LENGTH] = "";

    strncpy(finalQuery, query, diff);
    strcat(finalQuery, value);
    strcat(finalQuery, query + diff + 1);

    strcpy(query, finalQuery);
}

int DBService_registerUser(char *role, char* username, char* password) {
    char query[500] = "INSERT INTO users (role, username, password) VALUES ('?', '?', '?');";
    _bindParemter(query, role);
    _bindParemter(query, username);
    _bindParemter(query, password);

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);
    command = sqlite3_finalize(capat);

    if(queryExecResponse == SQLITE_DONE) {
        DBService_loginUser(username, password);
        return 0;
    }

    if(queryExecResponse == SQLITE_CONSTRAINT) {
        return 1;
    }

    return -1;
}

int DBService_loginUser(char *username, char *password) {
    char query[QUERY_LENGTH] = "SELECT * FROM users WHERE username = '?' AND password = '?';";
    _bindParemter(query, username);
    _bindParemter(query, password);

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);

    if(queryExecResponse == SQLITE_ROW) {
        getRowInfo(USERS);
        command = sqlite3_finalize(capat);
        return 0;
    }

    command = sqlite3_finalize(capat);

    if(queryExecResponse == SQLITE_CONSTRAINT) {
        return 1;
    }

    return -1;
}

// ==============================

void getUsers() {
    char query[QUERY_LENGTH] = "SELECT * FROM users;";

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
   
    while(sqlite3_step(capat) == SQLITE_ROW) {
        printf("Good query\n");
        getRowInfo(USERS);
    }
}

void getRowInfo(char *tabel) {
    int n;

    if(!strcmp(tabel, USERS)) n = USERS_ROW_LENGTH;
    if(!strcmp(tabel, SONGS)) n = SONGS_ROW_LENGTH;
    if(!strcmp(tabel, COMMENTS)) n = COMMENTS_ROW_LENGTH;

    for(int i = 0; i < n; ++i) {
        printf("%s ", sqlite3_column_text(capat, i));
    }
    printf("\n");
}