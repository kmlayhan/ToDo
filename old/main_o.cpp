#include "operator.h"
#include <math.h>   

void isFloat(const char*, int &, int &);
bool doesIndexExist(int, bool, bool);

int main(int args, char *argv[]){
    
    int intPart, fracPart = -1;
    bool intPartExistOnDatabase = true, fracPartExistOnDatabase = false;
    
    std::string command(argv[1]);

    Operator op = Operator();
    
    // Print all todo list 
    // After print, terminate
    if (args == 1) {
        std::cout << "Print all todo list" << std::endl;
        return 0;
    } 

    // Append argv[2] as a new task
    if (args == 3 && command == "append"){
        std::cout << "Task appended. " << std::endl;
        exit(0);
    }

    // Check the following argument (argv[2]), if it is integer or float
    isFloat(argv[2], intPart, fracPart);

    // Integer existance check to Database
    
    // If float and Integer exist
        // Float existance check to Database
    

    // Append given sub-task 
    if (command == "append"){

        // Append a new sub-task
        if (fracPart == -1 && intPartExistOnDatabase && args == 4){
            std::cout << "Task appended. " << std::endl;
        }
        else{
            std::cout << "You cannot add sub-sub task" << std::endl;
        }
    } 
    
    else if (command == "delete" && args == 3){

        
        // If index does not exits
        if(!doesIndexExist(fracPart, fracPartExistOnDatabase, intPartExistOnDatabase)){
            std::cout << "Given task does not exits" << std::endl;
        }

        // Index is float 
        else if(fracPart > -1){
            // Delete and reorganizie the numbers

        }    

        // Index is integer
        else{
            // If it does not have sub-tasks
                // Delete
            
            // Else
                // Ask if s/he is sure
                // If Sure
                    // Delete
                // Else
                    // Terminate
        }
        exit(0);
    } 
    
    else if(command == "change"){
        
        // If index does not exits
        if(!doesIndexExist(fracPart, fracPartExistOnDatabase, intPartExistOnDatabase)){
            std::cout << "Given task does not exits" << std::endl;
        }

        // Else
            // Change context of index with argv[3]
    } 


    else {
        // Raise error, command cannot found 
        std::cout << "Command does not exits." << std::endl;
    }
    

    return 0;
}


void isFloat(const char* arg, int &integer, int &frac){
    std::string strArg(arg);
    try {
        int decimalIndex = strArg.find('.');
        integer = std::stoi(strArg.substr(0,decimalIndex));
        if (decimalIndex != std::string::npos){
            frac = std::stoi(strArg.substr(decimalIndex+1,strArg.length()));
        }
    } 
    
    catch (std::invalid_argument)
    {   
        std::cout << "Invalid Argument." << std::endl;
        exit(0);
    }
}

bool doesIndexExist(int fracPart, bool fracPartExistOnDatabase, bool intPartExistOnDatabase){
    if( (fracPart > -1 && fracPartExistOnDatabase) || (fracPart == -1 && intPartExistOnDatabase)){
        return 1;
    }
    return 0;
}
