#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "DBService.h"

int main() {
    DBService_initializeDB();

    struct User myUser;

    int code = DBService_userExists("Elena");
    printf("%d\n", code);
    
    // UserService_initializeUser(&myUser);
    // UserService_displayUser(&myUser);
    // printf("isLoggedIn: %d\n", UserService_isLoggedIn(&myUser));
    // printf("isUserNormal: %d\n", USerService_userIsNormal(&myUser));
    // printf("isUserAdmin: %d\n", UserService_userIsAdmin(&myUser));

    // int code = DBService_loginUser("Tom", "abc", &myUser);
    // //int code = DBService_registerUser("admin", "Booo000m", "abc", &myUser);
    // printf("%d\n", code);

    // UserService_displayUser(&myUser);
    // printf("isLoggedIn: %d\n", UserService_isLoggedIn(&myUser));
    // printf("isUserNormal: %d\n", USerService_userIsNormal(&myUser));
    // printf("isUserAdmin: %d\n", UserService_userIsAdmin(&myUser));

    DBService_closeDB();

    return 0;
}