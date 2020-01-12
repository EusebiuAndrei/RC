#pragma once
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "UserService.h"

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

int DBService_initializeDB();
int DBService_closeDB();

int DBService_loginUser(char *username, char *password, struct User* user);
int DBService_registerUser(char *role, char* username, char* password, struct User* user);
int DBService_userExists(char username[20]);
int DBService_denyVotes(char username[20]);

int DBService_addSong(char title[20], char description[20], char link[50], char genres[50]);
int DBService_removeSong(char link[20]);
int DBService_voteSong(char link[20]);
int DBService_displaySongsByGenres(char genres[20]);
int DBService_displaySongs();

void _populateUser(struct User* user);
void _bindParemter(char query[QUERY_LENGTH], char *value);

void getRowInfo(char *tabel);
void getUsers();

// DB connection related

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

int DBService_closeDB() {
    command = sqlite3_close(DB);

    if(command) {
        fprintf(stderr, "The db could not be closed: %s.\n", sqlite3_errmsg(DB));
        return 1;
    } else {
        fprintf(stderr, "I've closed the DB.\n");
        return 0;
    }
}

// User registration/login process

int DBService_registerUser(char *role, char* username, char* password, struct User* user) {
    char query[500] = "INSERT INTO users (role, username, password) VALUES ('?', '?', '?');";
    _bindParemter(query, role);
    _bindParemter(query, username);
    _bindParemter(query, password);

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);
    command = sqlite3_finalize(capat);

    if(queryExecResponse == SQLITE_DONE) {
        DBService_loginUser(username, password, user);
        return 0;
    }

    if(queryExecResponse == SQLITE_CONSTRAINT) {
        return 1;
    }

    return -1;
}

int DBService_loginUser(char *username, char *password, struct User* user) {
    char query[QUERY_LENGTH] = "SELECT * FROM users WHERE username = '?' AND password = '?';";
    _bindParemter(query, username);
    _bindParemter(query, password);

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);

    printf("AM RULAT LOGIN\n");
    printf("%d\n", queryExecResponse);

    if(queryExecResponse == SQLITE_ROW) {
        getRowInfo(USERS);
        _populateUser(user);
        command = sqlite3_finalize(capat);
        return 0;
    }

    command = sqlite3_finalize(capat);

    if(queryExecResponse == SQLITE_DONE) {
        return 1;
    }

    return -1;
}

int DBService_userExists(char username[20]) {
    char query[QUERY_LENGTH] = "SELECT * FROM users WHERE username = '?';";
    _bindParemter(query, username);

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);
    command = sqlite3_finalize(capat);

    return queryExecResponse == SQLITE_ROW ? 1 : 0;
}

int DBService_denyVotes(char username[20]) {
    char query[QUERY_LENGTH] = "UPDATE users SET canVote = 0 WHERE username = '?';";
    _bindParemter(query, username);

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);
    command = sqlite3_finalize(capat);

    if(queryExecResponse == SQLITE_DONE) {
        return 0;
    }

    if(queryExecResponse == SQLITE_CONSTRAINT) {
        return 1;
    }

    return -1;
}

// Songs flow

int DBService_addSong(char title[20], char description[20], char link[50], char genres[50]) {
    char query[QUERY_LENGTH] = "INSERT INTO songs (title, description, link, genres) VALUES('?', '?', '?', '?');";
    _bindParemter(query, title);
    _bindParemter(query, description);
    _bindParemter(query, link);
    _bindParemter(query, genres);

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);
    command = sqlite3_finalize(capat);

    if(queryExecResponse == SQLITE_DONE) {
        return 0;
    }

    if(queryExecResponse == SQLITE_CONSTRAINT) {
        return 1;
    }

    return -1;
}

int DBService_removeSong(char link[20]) {
    char query[QUERY_LENGTH] = "DELETE FROM songs WHERE link = '?';";
    _bindParemter(query, link);

    printf("fsdfsdsd\n");

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);
    command = sqlite3_finalize(capat);

    if(queryExecResponse == SQLITE_DONE) {
        return 0;
    }

    if(queryExecResponse == SQLITE_CONSTRAINT) {
        return 1;
    }

    return -1;
}

int DBService_voteSong(char link[20]) {
    char query[QUERY_LENGTH] = "UPDATE songs SET votes = votes + 1 WHERE link = '?';";
    _bindParemter(query, link);

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);
    command = sqlite3_finalize(capat);

    if(queryExecResponse == SQLITE_DONE) {
        return 0;
    }

    if(queryExecResponse == SQLITE_CONSTRAINT) {
        return 1;
    }

    return -1;
}

int DBService_displaySongsByGenres(char genres[20]) {
    char query[QUERY_LENGTH] = "SELECT * FROM songs WHERE genres LIKE '%?%' ORDER BY votes;";
    _bindParemter(query, genres);

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);

    printf("%d\n", queryExecResponse);

    if(queryExecResponse == SQLITE_ROW) {
        getRowInfo(SONGS);
        command = sqlite3_finalize(capat);
        return 0;
    }

    command = sqlite3_finalize(capat);

    if(queryExecResponse == SQLITE_DONE) {
        return 1;
    }

    return -1;
}

int DBService_displaySongs() {
    char query[QUERY_LENGTH] = "SELECT * FROM songs ORDER BY votes;";

    command = sqlite3_prepare_v2(DB, query, -1, &capat, 0);
    printf("%s\n", sqlite3_sql(capat));

    int queryExecResponse = sqlite3_step(capat);

    printf("%d\n", queryExecResponse);

    if(queryExecResponse == SQLITE_ROW) {
        getRowInfo(SONGS);
        command = sqlite3_finalize(capat);
        return 0;
    }

    command = sqlite3_finalize(capat);

    if(queryExecResponse == SQLITE_DONE) {
        return 1;
    }

    return -1;
}

// Utility functions

void _bindParemter(char query[QUERY_LENGTH], char *value) {
    char *firstParam = strchr(query, '?');
    
    int diff = firstParam - query;
    char finalQuery[QUERY_LENGTH] = "";

    strncpy(finalQuery, query, diff);
    strcat(finalQuery, value);
    strcat(finalQuery, query + diff + 1);

    strcpy(query, finalQuery);
}

void _populateUser(struct User* user) {
    char username[20], password[20], role[20], canVote[20];
    int canVoteInt;

    strcpy(username, sqlite3_column_text(capat, 2));
    strcpy(password, sqlite3_column_text(capat, 3));
    strcpy(role, sqlite3_column_text(capat, 1));
    strcpy(canVote, sqlite3_column_text(capat, 4));
    canVoteInt = canVote[0] - '0';

    //printf("%s %s %s %d\n", username, password, role, canVoteInt);

    UserService_createUser(username, password, role, canVoteInt, user);
}

// ==============================================================================================

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