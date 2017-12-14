#include "container.h"

void Container::destructor(Base *current)
{
    if(dynamic_cast<Executable *>(current) == NULL)
    {
        destructor(dynamic_cast<Connector *>(current)->left);
        destructor(dynamic_cast<Connector *>(current)->right);
    }
    delete current;
}

void Container::print(Base *current)
{
    if(dynamic_cast<Executable *>(current) == NULL)
    {
        if(dynamic_cast<Connector *>(current)->left != 0)
        {
            print(dynamic_cast<Connector *>(current)->left);
        }
        current->print();
        if(dynamic_cast<Connector *>(current)->right != 0)
        {
            print(dynamic_cast<Connector *>(current)->right);
        }
    }
    else
    {
        current->print();
    }
}