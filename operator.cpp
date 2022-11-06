#include "operator.h"

Operator::Operator(){
    int rc;
    rc = sqlite3_open("todo.db", &db);
    if(rc){
        std::cout << "Cannot open the dabase" << std::endl;
        exit(0);
    }  
    char* errMsg;
    
    const char* SQL_TASKS = "CREATE TABLE IF NOT EXISTS Tasks(tasknum INTEGER AUTO INCREMENT PRIMARY KEY, task TEXT )";
    const char* SQL_SUB_TASKS = "CREATE TABLE IF NOT EXISTS SubTasks( subtasknum INTEGER AUTO INCREMENT, metadata INTEGER, subtask TEXT,FOREIGN KEY(metadata) REFERENCES Tasks(tasknum) ON UPDATE CASCADE ON DELETE CASCADE)";
    sqlite3_exec(db, "PRAGMA foreign_keys=ON", 0, 0, &errMsg);

    rc = sqlite3_exec(db, SQL_TASKS, 0, 0, &errMsg);
    int rc2 = sqlite3_exec(db, SQL_SUB_TASKS, 0, 0, &errMsg);

    if(rc != SQLITE_OK || rc2 != SQLITE_OK) {
        std::cout << "Database Error" << std::endl;
        sqlite3_free(errMsg);
        exit(0);
    }
    char* sql = sqlite3_mprintf("INSERT INTO Tasks(tasknum, task) VALUES(0, 'dummy');");
    sqlite3_exec(db, sql, 0, 0, &errMsg);
    sqlite3_free(errMsg);
}

void Operator::addTask(int intPart, const char *task) {

    char *sql;
    char *err_msg;
    if (intPart == -1)
        sql = sqlite3_mprintf("INSERT INTO Tasks(tasknum, task) SELECT MAX(tasknum)+1, '%q' FROM Tasks;",task);
    else 
        sql = sqlite3_mprintf("INSERT INTO SubTasks(subtasknum, subtask, metadata) SELECT MAX(subtasknum)+1, '%q', %d FROM SubTasks WHERE metadata = %d;",task, intPart, intPart);
    
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    sqlite3_free(sql);
    if(rc != SQLITE_OK) {
        std::cout << "Failed To Append The Task" << std::endl;
        std::cout << err_msg << std::endl;
        sqlite3_free(err_msg);
        exit(0);
    }

    if (intPart == -1){
        sql = sqlite3_mprintf("INSERT INTO SubTasks( metadata, subtasknum, subtask) SELECT MAX(tasknum), 0, 'dummy' FROM Tasks;", intPart);
        sqlite3_exec(db, sql, 0, 0, &err_msg);
        std::cout << err_msg << std::endl;
        sqlite3_free(err_msg);
    }

    return;
}

bool Operator::intPartExist(int intPart) const{
    char* sql = sqlite3_mprintf("SELECT * FROM Tasks WHERE tasknum = %d", intPart);
    char *err_msg;
    sqlite3_stmt *stmt = NULL;
    bool flag = false;

    int rc = sqlite3_prepare_v2( db, sql, -1, &stmt, NULL );
    if ( rc != SQLITE_OK){
        sqlite3_free(sql);
        std::cout << "Query Error" << std::endl;
        exit(0);
    }
    
    else if (sqlite3_step( stmt ) == SQLITE_ROW )
        flag = 1;
    
    sqlite3_free(sql);
    sqlite3_finalize( stmt );

    return flag;
}

bool Operator::fracPartExist(int intPart, int fracPart) const{
    
    char* sql = sqlite3_mprintf("SELECT * FROM SubTasks WHERE metadata = %d AND subtasknum = %d", intPart, fracPart);
    char *err_msg;
    sqlite3_stmt *stmt = NULL;
    bool flag = false;

    int rc = sqlite3_prepare_v2( db, sql, -1, &stmt, NULL );
    if ( rc != SQLITE_OK){
        sqlite3_free(sql);
        std::cout << "Query Error" << std::endl;
        exit(0);
    }
    
    else if (sqlite3_step( stmt ) == SQLITE_ROW )
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
    char *err_msg;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    sqlite3_free(sql);
    if(rc != SQLITE_OK) {
        std::cout << "Failed To Delete" << std::endl;
        std::cout << err_msg << std::endl;
        sqlite3_free(err_msg);
        exit(0);
    }

    taskNumOrganizer(intPart, fracPart);
    

}

void Operator::taskNumOrganizer(int intPart, int fracPart) {
    
    char *sql;
    char*err_msg;

    if (fracPart < 0)
        sql = sqlite3_mprintf("UPDATE Tasks set tasknum = tasknum - 1 WHERE tasknum > %d", intPart);
    else
        sql = sqlite3_mprintf("UPDATE SubTasks set subtasknum = subtasknum - 1 WHERE subtasknum > %d AND metadata = %d", fracPart, intPart);

    
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    sqlite3_free(sql);
    if(rc != SQLITE_OK) {
        std::cout << "Failed To Delete" << std::endl;
        std::cout << err_msg << std::endl;
        sqlite3_free(err_msg);
        exit(0);
    }
}

void Operator::updateContent(int intPart, int fracPart, const char *task){


    char *sql;
    char*err_msg;

    if (fracPart < 0)
        sql = sqlite3_mprintf("UPDATE Tasks set task = '%q' WHERE tasknum = %d", task, intPart);
    else
        sql = sqlite3_mprintf("UPDATE SubTasks set subtask = '%q' WHERE metadata = %d AND subtasknum = %d", task, intPart, fracPart);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    sqlite3_free(sql);
    if(rc != SQLITE_OK) {
        std::cout << "Failed To Update" << std::endl;
        std::cout << err_msg << std::endl;
        sqlite3_free(err_msg);
        exit(0);
    }

}

void Operator::printAllList(){
    char *sql = sqlite3_mprintf("SELECT Tasks.tasknum, Tasks.task, SubTasks.subtasknum, SubTasks.subtask FROM Tasks INNER JOIN SubTasks ON Tasks.tasknum=SubTasks.metadata ORDER BY SubTasks.metadata, SubTasks.subtasknum;");
    char *err_msg;
    int rc = sqlite3_exec(db, sql, printCallback, 0, &err_msg);
    sqlite3_free(err_msg);
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
    char *err_msg;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        std::cout << err_msg << std::endl;
    }
    sqlite3_free(err_msg);
    sqlite3_free(sql);
}