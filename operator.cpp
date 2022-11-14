#include "operator.h"

#include <limits.h>
#include <unistd.h>
#include <cstring>
Operator::Operator(){
 
    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    std::string path(result);
    path.append(".db");
    char p[path.length()+4];
    strcpy(p, path.c_str());

    int rc = sqlite3_open( p, &db);
    if(rc){
        std::cout << "Cannot open the dabase" << std::endl;
        exit(0);
    }  

    const char* SQL_TASKS = "CREATE TABLE IF NOT EXISTS Tasks(tasknum INTEGER AUTO INCREMENT PRIMARY KEY, task TEXT )";
    const char* SQL_SUB_TASKS = "CREATE TABLE IF NOT EXISTS SubTasks( subtasknum INTEGER AUTO INCREMENT, metadata INTEGER, subtask TEXT,FOREIGN KEY(metadata) REFERENCES Tasks(tasknum) ON UPDATE CASCADE ON DELETE CASCADE)";
    sqlite3_exec(db, "PRAGMA foreign_keys=ON", 0, 0, NULL);

    rc = sqlite3_exec(db, SQL_TASKS, 0, 0, NULL);
    int rc2 = sqlite3_exec(db, SQL_SUB_TASKS, 0, 0, NULL);

    if(rc != SQLITE_OK || rc2 != SQLITE_OK) {
        std::cout << "Database Error" << std::endl;
        exit(0);
    }
    char* sql = sqlite3_mprintf("INSERT INTO Tasks(tasknum, task) VALUES(0, 'dummy');");
    sqlite3_exec(db, sql, 0, 0, NULL);
}

void Operator::addTask(int intPart, const char *task) {

    char *sql;
    if (intPart == -1)
        sql = sqlite3_mprintf("INSERT INTO Tasks(tasknum, task) SELECT MAX(tasknum)+1, '%q' FROM Tasks;",task);
    else 
        sql = sqlite3_mprintf("INSERT INTO SubTasks(subtasknum, subtask, metadata) SELECT MAX(subtasknum)+1, '%q', %d FROM SubTasks WHERE metadata = %d;",task, intPart, intPart);
    
    int rc = sqlite3_exec(db, sql, 0, 0, NULL);
    sqlite3_free(sql);
    checkError(rc, "Something Went Wrong.");

    if (intPart == -1){
        sql = sqlite3_mprintf("INSERT INTO SubTasks( metadata, subtasknum, subtask) SELECT MAX(tasknum), 0, 'dummy' FROM Tasks;", intPart);
        rc = sqlite3_exec(db, sql, 0, 0, NULL);
        checkError(rc, "Something Went Wrong.");
    }

    return;
}

bool Operator::intPartExist(int intPart) const{
    char* sql = sqlite3_mprintf("SELECT * FROM Tasks WHERE tasknum = %d", intPart);
    sqlite3_stmt *stmt = NULL;
    bool flag = false;

    int rc = sqlite3_prepare_v2( db, sql, -1, &stmt, NULL );
    checkError(rc, "Something Went Wrong.");
    if (rc != SQLITE_OK) {
        sqlite3_free(sql);
        sqlite3_finalize( stmt );
        return false;
    }
    if (sqlite3_step( stmt ) == SQLITE_ROW )
        flag = 1;
    
    sqlite3_free(sql);
    sqlite3_finalize( stmt );

    return flag;
}

bool Operator::fracPartExist(int intPart, int fracPart) const{
    
    char* sql = sqlite3_mprintf("SELECT * FROM SubTasks WHERE metadata = %d AND subtasknum = %d", intPart, fracPart);
    sqlite3_stmt *stmt = NULL;
    bool flag = false;

    int rc = sqlite3_prepare_v2( db, sql, -1, &stmt, NULL );
    checkError(rc, "Something Went Wrong.");
    if (rc != SQLITE_OK) {
        sqlite3_free(sql);
        sqlite3_finalize( stmt );
        return false;
    }
    if (sqlite3_step( stmt ) == SQLITE_ROW )
        flag = true;
    
    sqlite3_free(sql);
    sqlite3_finalize( stmt );

    return flag;
     
}

void Operator::deleteTask(int intPart, int fracPart) {

    char *sql;
    if (fracPart < 0)
        sql = sqlite3_mprintf("DELETE FROM Tasks WHERE tasknum = %d", intPart);
    else
        sql = sqlite3_mprintf("DELETE FROM SubTasks WHERE subtasknum = %d AND metadata = %d" , fracPart, intPart);

    int rc = sqlite3_exec(db, sql, 0, 0, NULL);
    sqlite3_free(sql);
    checkError(rc, "Something Went Wrong.");

    taskNumOrganizer(intPart, fracPart);
    

}

void Operator::taskNumOrganizer(int intPart, int fracPart) {
    
    char *sql;

    if (fracPart < 0)
        sql = sqlite3_mprintf("UPDATE Tasks set tasknum = tasknum - 1 WHERE tasknum > %d", intPart);
    else
        sql = sqlite3_mprintf("UPDATE SubTasks set subtasknum = subtasknum - 1 WHERE subtasknum > %d AND metadata = %d", fracPart, intPart);

    
    int rc = sqlite3_exec(db, sql, 0, 0, NULL);
    sqlite3_free(sql);
    checkError(rc, "Something Went Wrong.");
}

void Operator::updateContent(int intPart, int fracPart, const char *task){
    
    char *sql;
    if (fracPart < 0)
        sql = sqlite3_mprintf("UPDATE Tasks set task = '%q' WHERE tasknum = %d", task, intPart);
    else
        sql = sqlite3_mprintf("UPDATE SubTasks set subtask = '%q' WHERE metadata = %d AND subtasknum = %d", task, intPart, fracPart);

    int rc = sqlite3_exec(db, sql, 0, 0, NULL);
    sqlite3_free(sql);
    checkError(rc, "Something Went Wrong.");
}

void Operator::printAllList(){
    char *sql = sqlite3_mprintf("SELECT Tasks.tasknum, Tasks.task, SubTasks.subtasknum, SubTasks.subtask FROM Tasks INNER JOIN SubTasks ON Tasks.tasknum=SubTasks.metadata ORDER BY SubTasks.metadata, SubTasks.subtasknum;");
    int rc = sqlite3_exec(db, sql, printCallback, 0, NULL);
    checkError(rc, "Something Went Wrong.");
    sqlite3_free(sql);
    return;
}

int Operator::printCallback(void *NotUsed, int argc, char **argv, char **azColName) {
   
    std::string subtask(argv[3]);
    
    if (subtask == "dummy")
        std::cout << argv[0] << ") " << argv[1] << std::endl;
    
    else 
        std::cout << "\t" << argv[2] << ") " << argv[3] << std::endl;
    
   return 0;
}

void Operator::clearList(){
    char* sql = sqlite3_mprintf("DROP TABLE IF EXISTS Tasks; DROP TABLE IF EXISTS SubTasks");
    int rc = sqlite3_exec(db, sql, 0, 0, NULL);
    checkError(rc, "Something Went Wrong.");
    sqlite3_free(sql);
}

void Operator::checkError(const int rc, const std::string &msg) const {
    if (rc != SQLITE_OK) {
        std::cout << msg << std::endl;
    }
}

