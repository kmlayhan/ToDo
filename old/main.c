#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

void openDB(sqlite3 **db);
void createTable(sqlite3 *db);
void insertDB(sqlite3 *db, char *f, char *s);
void getAll(sqlite3 *db);
void clear(sqlite3 *db);
void checkError(int rc, sqlite3 *db, char *err_msg);
void deleteRow(sqlite3 *db, int rowNum);
int callback(void *, int, char **, char **);


int main(void) {
    
    sqlite3 *db;
    char *err_msg;

    openDB(&db);
    createTable(db);
    //insertDB(db, "CS 300", "Lecture");
    getAll(db);
    //clear(db);
    deleteRow(db, 1);
    getAll(db);
    sqlite3_close(db);
    
    return 0;
}

void openDB(sqlite3 **db) {

    int rc = sqlite3_open("ToDo.db", db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n",sqlite3_errmsg(*db));
        sqlite3_close(*db);
        exit(1);
    }

    return;
}

void createTable(sqlite3 *db) {
    
    char *sql = "CREATE TABLE IF NOT EXISTS TODO (action TEXT, comment Text);";
    char *err_msg;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    checkError(rc, db, err_msg);
    
    return;
}

void insertDB(sqlite3 *db, char *f, char *s) {
    char *sql = sqlite3_mprintf("INSERT INTO TODO(action, comment) Values('%q', '%q');",f, s);
    char *err_msg;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    sqlite3_free(sql);
    checkError(rc, db, err_msg);
    return;
}

void getAll(sqlite3 *db) {
    
    char *sql2 = "SELECT rowid, action, comment FROM TODO";
    char *err_msg;
    int rc = sqlite3_exec(db, sql2, callback, 0, &err_msg);
    checkError(rc, db, err_msg);

    return;
}

void clear(sqlite3 *db) {
    
    char *sql = "DROP TABLE TODO";
    char *err_msg;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    checkError(rc, db, err_msg);
    createTable(db);
    return;
}

void checkError(int rc, sqlite3 *db, char *err_msg) {

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        exit(1);
    }
    return;
}

void deleteRow(sqlite3 *db, int rowNum) {
    char *sql = sqlite3_mprintf("DELETE FROM TODO WHERE rowid = %p;", rowNum);
    char *err_msg;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    sqlite3_free(sql);
    checkError(rc, db, err_msg);
    return;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    
    printf("%s) %s\n",argv[0], argv[1]);
    if (argv[2] != NULL)
    {
        printf("\t-%s\n", argv[2]);
    }
    
    /*
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    */
    printf("\n");
    
    return 0;
}
