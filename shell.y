
/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%code requires 
{
#include <string>

#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}

%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}

%token <cpp_string> WORD
%token NOTOKEN GREAT NEWLINE GREATGREAT PIPE AMPERSAND GREATGREATAMPERSAND GREATAMPERSAND LESS

%{
//#define yylex yylex
#include <cstdio>
#include "shell.hh"

void yyerror(const char * s);
int yylex();

%}

%%
//I added below

goal: command_list;

arg_list: 
  arg_list WORD {
    printf(" Yacc: insert argument \"%s\"\n", $2->c_str());
    Command::_currentSimpleCommand->insertArgument( $2 );
  }
  | /*empty*/ 
  ;

cmd_and_args: 
  WORD {
    printf(" Yacc: insert command \"%s\"\n", $1->c_str());
    Command::_currentSimpleCommand = new SimpleCommand();
    Command::_currentSimpleCommand->insertArgument( $1 );
  }
  arg_list {
    Shell::_currentCommand.
    insertSimpleCommand( Command::_currentSimpleCommand );
  }
  ; 

pipe_list: 
  pipe_list PIPE cmd_and_args 
  | cmd_and_args 
  ;

io_modifier: 
  GREATGREAT Word 
  | GREAT Word {
    printf(" Yacc: insert output \"%s\"\n", $2->c_str());
    Shell::_currentCommand._outFile = $2;
  }
  | GREATGREATAMPERSAND Word
  | GREATAMPERSAND Word 
  | LESS Word 
  ; 

io_modifier_list: 
  io_modifier_list io_modifier 
  | /*empty*/ 
  ; 

background_optional:  
  AMPERSAND 
  | /*empty*/ 
  ; 

command_line: 
  pipe_list io_modifier_list background_opt NEWLINE 
  | NEWLINE /*accept empty cmd line*/ 
  | error NEWLINE{yyerrok;} 
             /*error recovery*/ 

command_list :  
  command_list command_line 
  ;/* command loop*/


%%

void
yyerror(const char * s)
{
  fprintf(stderr,"%s", s);
}

#if 0
main()
{
  yyparse();
}
#endif
