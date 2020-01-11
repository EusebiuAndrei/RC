#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "DBService.h"

int main() {
    DBService_initializeDB();

    int code = DBService_loginUser("Tom", "abc");
    //int code = DBService_registerUser("admin", "Donovan", "abc");
    printf("%d\n", code);

    return 0;
}