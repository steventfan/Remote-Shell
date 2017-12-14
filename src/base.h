#ifndef BASE_H
#define BASE_H

#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <boost/tokenizer.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Base
{
    public:
        Base() {};
        virtual ~Base() {};
        virtual bool execute(int, int) = 0;
        virtual void print() = 0; //********** print function for testing purposes **********
};

class Executable : public Base
{
    public:
        Executable() {};
        virtual ~Executable() {};
        virtual bool execute(int, int) = 0;
        virtual std::string data() = 0;
        virtual void print() = 0;
};

class Connector : public Base 
{
    public:
        Base *left;
        Base *right;
        
        Connector() : left(0), right(0) {};
        virtual ~Connector() {};
        virtual bool execute(int, int) = 0;
        virtual void print() = 0;
};

class Command : public Executable
{
    private:
        std::string command;
        std::string parameters;
    public:
        Command() : command(""), parameters("") {};
        Command(std::string element) : command(element), parameters("") {};
        Command(std::string element, std::string elements) : command(element), parameters(elements) {};
        ~Command() {};
        bool execute(int, int);
        std::string data();
        void print();
};

class Test : public Executable
{
    private:
        std::string parameters;
    public:
        Test() : parameters("") {};
        Test(std::string elements) : parameters(elements) {};
        ~Test() {};
        bool execute(int, int);
        std::string data();
        void print();
};

class Exit : public Executable 
{
    private:
        std::string parameters;
    public:
        Exit() : parameters("") {};
        Exit(std::string elements) : parameters(elements) {};
        ~Exit() {};
        bool execute(int, int);
        std::string data();
        void print();
};

class Semicolon : public Connector
{
    public:
        Semicolon() {};
        ~Semicolon() {};
        bool execute(int, int);
        void print();
};

class OR : public Connector
{
    public:
        OR() {};
        ~OR() {};
        bool execute(int, int);
        void print();
};

class AND : public Connector
{
    public:
        AND() {};
        ~AND() {};
        bool execute(int, int);
        void print();
};

class Pipe : public Connector
{
    public:
        Pipe() {};
        ~Pipe() {};
        bool execute(int, int);
        void print();
};

class Input : public Connector
{
    public:
        Input() {};
        ~Input() {};
        bool execute(int, int);
        void print();
};

class Single : public Connector
{
    public:
        Single() {};
        ~Single() {};
        bool execute(int, int);
        void print();
};

class Double : public Connector
{
    public:
        Double() {};
        ~Double() {};
        bool execute(int, int);
        void print();
};

#endif