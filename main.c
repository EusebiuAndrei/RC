#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "DBService.h"

int main() {
    DBService_initializeDB();

    struct User myUser;

    // int code = DBService_userExists("Elena");
    // printf("%d\n", code);
    
    UserService_initializeUser(&myUser);
    UserService_displayUser(&myUser);

    int code = DBService_loginUser("Tom", "abc", &myUser);
    //int code = DBService_registerUser("admin", "Booo000m", "abc", &myUser);
    printf("%d\n", code);

    UserService_displayUser(&myUser);

    code = DBService_addSong("Boom", "description 1", "@ls", "folk,pop");
    printf("Song added: %d\n", code);

    DBService_closeDB();

    return 0;
}