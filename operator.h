#include <fstream>
#include <iostream>
#include <sqlite3.h>
#include <string>
#ifndef _OPERATOR_
    #define _OPERATOR_

class Operator{

    private:
        sqlite3 *db;

    public:
        Operator();
        // Add destructor to close the Database
        void addTask(int, const char *);
        bool intPartExist(int) const;
        bool fracPartExist(int, int) const;
        void deleteTask(int, int);
        void taskNumOrganizer(int, int);
        void updateContent(int, int, const char*);
        void printAllList();
        void clearList();
        static int printCallback(void *, int, char **, char **);
        void checkError(const int, const std::string &) const;
      
};

#endif
