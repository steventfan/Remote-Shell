#include "base.h"

bool Command::execute(int input, int output)
{
    std::vector<std::string> data;
    char *arguments[1000];
    bool ret = true;
    
    arguments[0] = const_cast<char *>(command.c_str() );
    if(command == "echo")
    {
        arguments[1] = const_cast<char *>(parameters.c_str() );
        arguments[2] = nullptr;
    }
    else
    {
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer; //********** separate parameters when not outputting **********
        boost::char_separator<char> sep{" "};
        tokenizer tok{parameters, sep};
        
        for(tokenizer::iterator it = tok.begin(); it != tok.end(); it++)
        {
            data.push_back(*it);
        }
        
        unsigned int i = 0;
        
        while(i < data.size() )
        {
            arguments[i + 1] = const_cast<char *>( (data.at(i) ).c_str() );
            i++;
        }
        arguments[i + 1] = nullptr;
    }
    
    pid_t pid = fork();
    
    if(pid == -1) //********** fork() failed to split program into parent and child **********
    {
        perror("fork() has failed to execute successfully");
        
        ret = false;
    }
    else if(pid == 0) //********** child operation **********
    {
        if(dup2(input, 0) == -1)
        {
            perror("dup2(input) has failed to execute successfully");
        }
        if(dup2(output, 1) == -1)
        {
            perror("dup2(output) has failed to execute successfully");
        }
        if(execvp(arguments[0], arguments) == -1)
        {
            perror("execvp() has failed to execute successfully");
            
            exit(1);
        }
    }
    else //********** parent operation **********
    {
        int status;
        
        if(waitpid(pid, &status, 0) == -1) //********** stall parent operation **********
        {
            perror("waitpid() has failed to execute successfully");
            
            ret = false;
        }
        else if(WEXITSTATUS(status) != 0) //********** check child operation return status **********
        {
            ret = false;
        }
    }
    
    return ret;
}

bool Test::execute(int input, int output)
{
    std::string flag = "-e";
    std::string arguments = parameters;
    
    if(!arguments.empty() ) //********** separate flags and parameters **********
    {
        if(arguments.at(0) == '-')
        {
            if(arguments.find(" ") != std::string::npos)
            {
                flag = arguments.substr(0, arguments.find(" ") );
                arguments = arguments.substr(arguments.find(" ") + 1);
            }
            else
            {
                flag = arguments;
            }
        }
    }
    
    struct stat file;
    int result = stat(arguments.c_str(), &file);
    
    if(result != 0) //********** check stat() return status **********
    {
        std::cout << "(False)" << std::endl;
        
        return false;
    }
    else if(flag == "-e")
    {
        std::cout << "(True)" << std::endl;
        
        return true;
    }
    else if(flag == "-f") //********** file flag **********
    {
        if(S_ISREG(file.st_mode) )
        {
            std::cout << "(True)" << std::endl;
            
            return true;
        }
        std::cout << "(False)" << std::endl;
        
        return false;
    }
    else if(flag == "-d") //********** directory flag **********
    {
        if(S_ISDIR(file.st_mode) )
        {
            std::cout << "(True)" << std::endl;
            
            return true;
        }
        std::cout << "(False)" << std::endl;
        
        return false;
    }
    std::cout << "(False)" << std::endl;
    
    return false;
}

bool Exit::execute(int input, int output)
{
    throw false;
}

bool Semicolon::execute(int input, int output)
{
    left->execute(input, output);
    
    return right->execute(input, output);
}

bool OR::execute(int input, int output) 
{
    if(!left->execute(input, output) )
    {
        return right->execute(input, output);
    }
    
    return true;
}

bool AND::execute(int input, int output)
{
    if(left->execute(input, output) )
    {
        return right->execute(input, output);
    }
    
    return false;
}

bool Pipe::execute(int input, int output)
{
    int file[2];
    
    if(pipe(file) == -1)
    {
        perror("pipe() has failed to execute successfully");
        
        return false;
    }
    if(!left->execute(input, file[1]) )
    {
        return false;
    }
    close(file[1]);
    if(!right->execute(file[0], output) )
    {
        return false;
    }
    close(file[0]);
    
    return true;
}

bool Input::execute(int input, int output)
{
    std::string file = dynamic_cast<Executable *>(right)->data();
    bool value;
    
    input = open(file.c_str(), O_RDONLY);
    if(input < 0)
    {
        std::cout << file << ": No such file or directory" << std::endl;
        value = false;
    }
    else
    {
        value = left->execute(input, output);
    }
    
    close(input);
    
    return value;
}

bool Single::execute(int input, int output)
{
    std::string file = dynamic_cast<Executable *>(right)->data();
    
    output = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    
    bool value = left->execute(input, output);
    
    close(output);
    
    return value;
}

bool Double::execute(int input, int output)
{
    std::string file = dynamic_cast<Executable *>(right)->data();
    
    output = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
    
    bool value = left->execute(input, output);
    
    close(output);
    
    return value;
}

std::string Command::data()
{
    if(parameters.empty() )
    {
        return command;
    }
    
    return command + " " + parameters;
}

std::string Test::data()
{
    if(parameters.empty() )
    {
        return "test";
    }
    
    return "test " + parameters;
}

std::string Exit::data()
{
    if(parameters.empty() )
    {
        return "exit";
    }
    
    return "exit " + parameters;
}

void Command::print()
{
    std::cout << command << " @ " << parameters << std::endl;
}

void Test::print()
{
    std::cout << "TEST" << " @ " << parameters << std::endl;
}

void Exit::print()
{
    std::cout << "EXIT" << " @ " << parameters << std::endl;
}

void Semicolon::print()
{
    std::cout << ";" << std::endl;
}

void OR::print()
{
    std::cout << "||" << std::endl;
}

void AND::print()
{
    std::cout << "&&" << std::endl;
}

void Pipe::print()
{
    std::cout << "|" << std::endl;
}

void Input::print()
{
    std::cout << "<" << std::endl;
}

void Single::print()
{
    std::cout << ">" << std::endl;
}

void Double::print()
{
    std::cout << ">>" << std::endl;
}