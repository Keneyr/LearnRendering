#pragma once
#include <iostream>

class Fuck
{
    public:
        Fuck(){std::cout<<"construct ..."<<std::endl;};
        ~Fuck(){std::cout<<"deconstruct..."<<std::endl;};
        int id = 4;
};