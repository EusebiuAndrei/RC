#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "DBService.h"

int main() {
    DBService_initializeDB();

    int code = DBService_getCommentsForSong("@asa");
    printf("Commets for a specific song: %d\n", code);

    // int code = DBService_addComment("Bon", "@asa", "Hello boys");
    // printf("Comment added: %d\n", code);

    // int code = DBService_removeSong("fdsa");
    // printf("Song deleted: %d\n", code);

    // int code = DBService_voteSong("fdsa");
    // printf("Song voted: %d\n", code);

    // int code = DBService_displaySongsByGenres("fds");
    // printf("Display songs by genres: %d\n", code);

    // int code = DBService_displaySongs();
    // printf("Display songs: %d\n", code);

    // int code = DBService_denyVotes("Dan");
    // printf("Deny votes: %d\n", code);

    // struct User myUser;

    // // int code = DBService_userExists("Elena");
    // // printf("%d\n", code);
    
    // UserService_initializeUser(&myUser);
    // UserService_displayUser(&myUser);

    // int code = DBService_loginUser("Tom", "abc", &myUser);
    // //int code = DBService_registerUser("admin", "Booo000m", "abc", &myUser);
    // printf("%d\n", code);

    // UserService_displayUser(&myUser);

    // code = DBService_addSong("Boom", "description 1", "@ls", "folk,pop");
    // printf("Song added: %d\n", code);

    DBService_closeDB();

    return 0;
}