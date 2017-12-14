#include "base.h"
#include "container.h"
#include <boost/tokenizer.hpp>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    bool run = true;
    
    while(run)
    {
        Container *container = new Container;
        std::string userInput;
        
        std::cout << "$ ";
        
        std::getline(std::cin, userInput);
        
        for(unsigned int i = 0; i < userInput.size(); i++) //********** remove comments **********
        {
            if(userInput.at(i) == '#')
            {
                userInput.erase(i);
                
                break;
            }
        }
        try
        {
            typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
            boost::char_separator<char> sep{" ", ";()|<>"};
            tokenizer tok{userInput, sep}; //********** parse user input **********
            bool commandStatus = true;
            bool testStatus = false;
            bool expectConnector = false;
            std::string commandLine;
            std::vector<std::string> tokens;
            
            if(userInput.find("<<") != std::string::npos || userInput.find("> >") != std::string::npos || userInput.find(">  >") != std::string::npos || userInput.find("| |") != std::string::npos || userInput.find("|  |") != std::string::npos)
            {
                    throw 'c'; //********** invalid connector syntax **********
            }
            for(tokenizer::iterator it = tok.begin(); it != tok.end(); it++)
            {
                if(*it == "(")
                {
                    if(!commandStatus)
                    {
                        throw 'p'; //********** invalid parentheses syntax **********
                    }
                    tokens.push_back(*it);
                }
                else if(*it == ")")
                {
                    if(commandStatus)
                    {
                        throw 'p'; //********** invalid parentheses syntax **********
                    }
                    expectConnector = true;
                    if(!commandLine.empty() )
                    {
                        tokens.push_back(commandLine);
                        commandLine.clear();
                    }
                    tokens.push_back(*it);
                }
                else if(*it == ";" || *it == "&&" || *it == "|" || *it == "<" || *it == ">")
                {
                    if(commandStatus || testStatus)
                    {
                        throw 'c'; //********** invalid connector syntax **********
                    }
                    commandStatus = true;
                    expectConnector = false;
                    if(!commandLine.empty() )
                    {
                        tokens.push_back(commandLine);
                        commandLine.clear();
                    }
                    
                    tokenizer::iterator its = it;
                        
                    its++;
                    if(its != tok.end() && (*it == "|" || *it == ">") )
                    {
                        if(*it == "|" && *its == "|")
                        {
                            it++;
                            tokens.push_back("||");
                        }
                        else if(*it == ">" && *its == ">")
                        {
                            it++;
                            tokens.push_back(">>");
                        }
                        else
                        {
                            tokens.push_back(*it);
                        }
                    }
                    else if(*it == "|" || *it == ">")
                    {
                        throw 'c'; //********** invalid connector syntax **********
                    }
                    else
                    {
                        tokens.push_back(*it);
                    }
                }
                else
                {
                    if(expectConnector)
                    {
                        throw 'a'; //********** expected connector after brackets **********
                    }
                    else if(commandStatus)
                    {
                        commandStatus = false;
                        if(*it == "[")
                        {
                            testStatus = true;
                            commandLine = "test";
                        }
                        else if(*it == "]")
                        {
                            throw 't'; //********** invalid test syntax **********
                        }
                        else
                        {
                            commandLine = *it;
                        }
                    }
                    else
                    {
                        if(*it == "[" || *it == "]")
                        {
                            if(*it == "[" || !testStatus)
                            {
                                throw 't'; //********** invalid test syntax **********
                            }
                            testStatus = false;
                            expectConnector = true;
                        }
                        else
                        {
                            commandLine += " " + *it;
                        }
                    }
                }
            }
            if(testStatus)
            {
                throw 't'; //********** invalid test syntax **********
            }
            else if(!commandLine.empty() )
            {
                tokens.push_back(commandLine);
            }
            else if(!tokens.empty() )
            {
                if(tokens.at(tokens.size() - 1) == ";" || tokens.at(tokens.size() - 1) == "&&" || tokens.at(tokens.size() - 1) == "|" || tokens.at(tokens.size() - 1) == "<" || tokens.at(tokens.size() - 1) == ">")
                {
                    throw 'c'; //********** invalid connector syntax **********
                }
            }
            else
            {
                throw "empty"; //********** empty command line **********
            }
            
            std::vector<std::string> stack;
            std::vector<Base *> postFix;
            Base *pointer;
            
            for(unsigned int i = 0; i < tokens.size(); i++) //********** create postfix objects **********
            {
                if(tokens.at(i) == ";" || tokens.at(i) == "||" || tokens.at(i) == "&&" || tokens.at(i) == "|" || tokens.at(i) == "<" || tokens.at(i) == ">" || tokens.at(i) == ">>")
                {
                    if(!stack.empty() && stack.at(stack.size() - 1) != "(")
                    {
                        if(stack.at(stack.size() - 1) == ";")
                        {
                            pointer = new Semicolon;
                        }
                        else if(stack.at(stack.size() - 1) == "||")
                        {
                            pointer = new OR;
                        }
                        else if(stack.at(stack.size() - 1) == "&&")
                        {
                            pointer = new AND;
                        }
                        else if(stack.at(stack.size() - 1) == "<")
                        {
                            pointer = new Input;
                        }
                        else if(stack.at(stack.size() - 1) == ">")
                        {
                            pointer = new Single;
                        }
                        else if(stack.at(stack.size() - 1) == ">>")
                        {
                            pointer = new Double;
                        }
                        else
                        {
                            pointer = new Pipe;
                        }
                        stack.pop_back();
                        postFix.push_back(pointer);
                    }
                    stack.push_back(tokens.at(i) );
                }
                else if(tokens.at(i) == "(")
                {
                    stack.push_back(tokens.at(i) );
                }
                else if(tokens.at(i) == ")")
                {
                    if(stack.empty() )
                    {
                        while(!postFix.empty() ) //********** deallocate unused memory **********
                        {
                            delete postFix.at(0);
                            postFix.erase(postFix.begin() );
                        }
                        
                        throw 'p'; //********** invalid parentheses syntax **********
                    }
                    while(stack.at(stack.size() - 1) != "(")
                    {
                        if(stack.at(stack.size() - 1) == ";")
                        {
                            pointer = new Semicolon;
                        }
                        else if(stack.at(stack.size() - 1) == "||")
                        {
                            pointer = new OR;
                        }
                        else if(stack.at(stack.size() - 1) == "&&")
                        {
                            pointer = new AND;
                        }
                        else if(stack.at(stack.size() - 1) == "<")
                        {
                            pointer = new Input;
                        }
                        else if(stack.at(stack.size() - 1) == ">")
                        {
                            pointer = new Single;
                        }
                        else if(stack.at(stack.size() - 1) == ">>")
                        {
                            pointer = new Double;
                        }
                        else
                        {
                            pointer = new Pipe;
                        }
                        stack.pop_back();
                        postFix.push_back(pointer);
                        if(stack.empty() )
                        {
                            while(!postFix.empty() ) //********** deallocate unused memory **********
                            {
                                delete postFix.at(0);
                                postFix.erase(postFix.begin() );
                            }
                            
                            throw 'p'; //********** invalid parentheses syntax **********
                        }
                    }
                    stack.pop_back();
                }
                else
                {
                    if(tokens.at(i).find(" ") != std::string::npos)
                    {
                        std::string executables = tokens.at(i).substr(0, tokens.at(i).find(" ") );
                        std::string parameters = tokens.at(i).substr(tokens.at(i).find(" ") + 1);
                        
                        if(executables == "exit")
                        {
                            pointer = new Exit(parameters);
                        }
                        else if(executables == "test")
                        {
                            pointer = new Test(parameters);
                        }
                        else
                        {
                            pointer = new Command(executables, parameters);
                        }
                    }
                    else
                    {
                        if(tokens.at(i) == "exit")
                        {
                            pointer = new Exit;
                        }
                        else if(tokens.at(i) == "test")
                        {
                            pointer = new Test;
                        }
                        else
                        {
                            pointer = new Command(tokens.at(i) );
                        }
                    }
                    postFix.push_back(pointer);
                }
            }
            if(stack.size() > 1)
            {
                while(!postFix.empty() ) //********** deallocate unused memory **********
                {
                    delete postFix.at(0);
                    postFix.erase(postFix.begin() );
                }
                
                throw 'p'; //********** invalid parentheses syntax **********
            }
            if(stack.size() == 1)
            {
                if(stack.at(0) == ";")
                {
                    pointer = new Semicolon;
                }
                else if(stack.at(0) == "||")
                {
                    pointer = new OR;
                }
                else if(stack.at(0) == "&&")
                {
                    pointer = new AND;
                }
                else if(stack.at(0) == "<")
                {
                    pointer = new Input;
                }
                else if(stack.at(0) == ">")
                {
                    pointer = new Single;
                }
                else if(stack.at(0) == ">>")
                {
                    pointer = new Double;
                }
                else if(stack.at(0) == "|")
                {
                    pointer = new Pipe;
                }
                else
                {
                    while(!postFix.empty() ) //********** deallocate unused memory **********
                    {
                        delete postFix.at(0);
                        postFix.erase(postFix.begin() );
                    }
                    
                    throw 'p'; //********** invalid parentheses syntax **********
                }
                postFix.push_back(pointer);
            }
            
            std::vector<Base *> objects;
            
            while(!postFix.empty() ) //********** build tree **********
            {
                if(dynamic_cast<Connector *>(postFix.at(0) ) == NULL)
                {
                    objects.push_back(postFix.at(0) );
                    postFix.erase(postFix.begin() );
                }
                else
                {
                    pointer = postFix.at(0);
                    postFix.erase(postFix.begin() );
                    dynamic_cast<Connector *>(pointer)->right = objects.at(objects.size() - 1);
                    objects.pop_back();
                    dynamic_cast<Connector *>(pointer)->left = objects.at(objects.size() - 1);
                    objects.pop_back();
                    objects.push_back(pointer);
                }
            }
            container->root = objects.at(0);
            container->root->execute(0, 1); //********** execute commands **********
            //container->print(container->root); //********** print container tree (for testing purposes) **********
            container->destructor(container->root); //********** deallocate unused memory **********
            delete container;
        }
        catch(char error)
        {
            if(error == 'c')
            {
                std::cout << "Error: Invalid Connector Syntax" << std::endl;
            }
            else if(error == 'p')
            {
                std::cout << "Error: Invalid Parentheses Syntax" << std::endl;
            }
            else if(error == 't')
            {
                std::cout << "Error: Invalid Test Syntax" << std::endl;
            }
            else
            {
                std::cout << "Error: Expected Connector After Brackets" << std::endl;
            }
            delete container; //********** deallocate unused memory **********
        }
        catch(bool error)
        {
            run = false;
            
            container->destructor(container->root); //********** deallocate unused memory **********
            delete container;
        }
        catch(...)
        {
            delete container; //********** deallocate unused memory **********
        }
    }
    
    return 0;
}