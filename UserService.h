#pragma once
#include <stdio.h>
#include <string.h>

struct User {
	char username[20];
	char password[20];
	char role[20];
	int canVote;
};

void UserService_initializeUser(struct User* user);
void UserService_createUser(char username[20], char password[20], char role[20], int canVote, struct User* user);
void UserService_displayUser(struct User* user);
int UserService_isLoggedIn(struct User* user);
int USerService_userIsNormal(struct User* user);
int UserService_userIsAdmin(struct User* user);

void UserService_initializeUser(struct User* user) {
    strcpy(user->username, "");
    strcpy(user->password, "");
    strcpy(user->role, "");
    user->canVote = -1;
}

void UserService_createUser(char username[20], char password[20], char role[20], int canVote, struct User* user) {
    strcpy(user->username, username);
    strcpy(user->password, password);
    strcpy(user->role, role);
    user->canVote = canVote;
}

void UserService_displayUser(struct User* user) {
    printf("username: %s\n", user->username);
    printf("password: %s\n", user->password);
    printf("role: %s\n", user->role);
    printf("canVote: %d\n", user->canVote);
}

int UserService_isLoggedIn(struct User* user) {
    return user->canVote == -1 ? 0 : 1;
}

int USerService_userIsNormal(struct User* user) {
    int isNormal = strcmp(user->role, "normal");

    return isNormal == 0 ? 1: 0;
}

int UserService_userIsAdmin(struct User* user) {
    int isAdmin = strcmp(user->role, "admin");

    return isAdmin == 0 ? 1 : 0;
}