#ifndef CONTAINER_H
#define CONTAINER_H

#include "base.h"
#include <vector>

class Container
{
    public:
        Base *root;
        
        Container() : root(0) {};
        Container(Base *pointer) : root(pointer) {};
        void destructor(Base *);
        void print(Base *);
};

#endif