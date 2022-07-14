/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include "command.hh"
#include "shell.hh"


Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands) {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();

    if ( _outFile ) {
        delete _outFile;
    }
    _outFile = NULL;

    if ( _inFile ) {
        delete _inFile;
    }
    _inFile = NULL;

    if ( _errFile ) {
        delete _errFile;
    }
    _errFile = NULL;

    _background = false;
}

void Command::print() {
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simpleCommand : _simpleCommands ) {
        printf("  %-3d ", i++ );
        simpleCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO");
    printf( "\n\n" );
}

void Command::execute() {
    // Don't do anything if there are no simple commands
    if ( _simpleCommands.size() == 0 ) {
        Shell::prompt();
        return;
    }
    
    // Print contents of Command data structure
    print();
    
    // save in and out
    int tmpin = dup(0);
    int tmpout = dup(1);

    // set initial input
    int fdin;
    if (_inFile) {
      fdin = open(_inFile.c_str(), O_RDONLY);
    } else {
      // use default input
      fdin = dup(tmpin);
    }

    int ret;
    int fdout;

    for ( size_t i = 0; i < _simpleCommands.size(); i++ ) {
      dup2(fdin, 0);
      close(fdin);

      if(i == _simpleCommands.size() - 1){
        if(_outFile){
	  fdout = open(_outFile, ......);
	} else {
	  fdout = dup(tmpout);
	}
      }
      ret = fork();
      if (ret == 0) {
	size_t num = _simpleCommands[i]->_arguments.size();
        char** myargv = new char*[num + 1];
	for ( size_t j = 0; j < num; j++ ) {
	  myargv[j] = new char[_simpleCommands[i]->_arguments[j]->size() + 1];
          strcpy(myargv[j], _simpleCommands[i]->_arguments[j]->c_str());
	}
	myargv[num + 1] = NULL;
        execvp(_simpleCommands[i]->_arguments[0]->c_str(), myargv);
        
	for( size_t j = 0; j < num; j++ ) {
	  delete [] myargv[j];
	}
	delete [] myargv;
	
	perror("execvp");
	exit(1);
      }
      else if (ret < 0) {
        perror("fork");
	return;
      }
    }
    if (!_background) {
      waitpid(ret, NULL, 0);
    }
    // Clear to prepare for next command
    clear();

    // Print new prompt
    Shell::prompt();
}

SimpleCommand * Command::_currentSimpleCommand;
