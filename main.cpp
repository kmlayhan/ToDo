#include "operator.cpp"
#include <math.h>   

void isFloat(const char*, int &, int &);
bool doesIndexExist(const Operator &, int, int);

int main(int args, char *argv[]){
    
    int intPart, fracPart = -1;
    
    Operator op;

    // Print all todo list 
    // After print, terminate
    if (args == 1) {
        op.printAllList();
        return 0;
    } 

    std::string command(argv[1]);

    if (args == 2) {
        if (command == "-clr") {
            char response;
            std::cout << "Clear the list? (Y/N) "; 
            while(response != 'y' && response != 'Y' && response != 'n' && response != 'N'){
                    std::cin.clear();
                    std::cout << "Please select Y/N ";
                    std::cin >> response;
                }

                if( !(response == 'y' || response == 'Y') ){
                    exit(0);
                }
            op.clearList();
        }
        exit(0);
    }

    // 3 argument is entered
    if (args == 3){

        // If command is -add
        if (command == "-add") {
            // Append argv[2] as a new task
            op.addTask(-1, argv[2]);
            //std::cout << "Task appended. " << std::endl;
            return 0;
        }


        else if (command=="-del") {

            // Check the following argument (argv[2]), if it is integer or float
            isFloat(argv[2], intPart, fracPart);
            

            // Check if the given index exits on database.
            if(!doesIndexExist(op, intPart, fracPart)){
                std::cout << "Given task does not exits" << std::endl;
                exit(0);
            }
            else if(intPart == 0 || fracPart == 0) {
                exit(0);
            }

            // Index is integer
            else if(fracPart < 0) {
                char response;
                std::cout << "Are You Sure? If selected task has subtasks, they will be deleted as well. (Y/N) ";
                std::cin >> response;
                while(response != 'y' && response != 'Y' && response != 'n' && response != 'N'){
                    std::cin.clear();
                    std::cout << "Please select Y/N ";
                    std::cin >> response;
                }

                if( !(response == 'y' || response == 'Y') ){
                    exit(0);
                }

            }    

            op.deleteTask(intPart, fracPart);

        }

        else {
            std::cout << "Unknown Command" << std::endl;
        }
    } 
    
    else if (args == 4) {
        
        isFloat(argv[2], intPart, fracPart);

        // Append given sub-task 
        if(command == "-add") {
            // Append a new sub-task
            if (fracPart == -1 && doesIndexExist(op, intPart, fracPart)){
                op.addTask(intPart, argv[3]);
            }     

            else {
            std::cout << "You cannot add sub-sub task" << std::endl;
            }
        }

        else if(command == "-ch") {
            // If index does not exits
            if(!doesIndexExist(op, intPart, fracPart) && (intPart == 0 || fracPart == 0) ){
            std::cout << "Given task does not exits" << std::endl;
            exit(1);
            }

            op.updateContent(intPart, fracPart, argv[3]);

        }
        
        else {
            std::cout << "Unknown Command" << std::endl;
        }

    }

    else {
        std::cout << "Unknown Command" << std::endl;
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

bool doesIndexExist(const Operator &op, int intPart, int fracPart){
    
    // If intPart does not exits on database
    if(!op.intPartExist(intPart)) {
        return 0;
    }
    
    // If the given index is a index of sub task
    else if (fracPart > -1) {
        // Check if frac part exist on database
        if(!op.fracPartExist(intPart, fracPart)) {
            return 0; 
        }
    }
    return 1;
}
