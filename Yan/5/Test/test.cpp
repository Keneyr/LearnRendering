
#include "fun.hpp"
#include <iostream>
#include <string>
#include <fstream>

int main()
{
    std::cout<<"vscode"<<std::endl;

    Fuck *f = new Fuck;
    f->id = 3;

    std::cout<<f->id<<std::endl;

    delete f;

    //check the file path
    //std::string file_path = "../model/a.txt";
    std::string file_path = "../model/a.txt";

    std::ifstream file(file_path);

    if(!file.is_open())
    {
        std::cout<<"failed"<<std::endl;
    }
    else
    {
         std::cout<<"success"<<std::endl;
    }
    

    
    
    return 0;
}