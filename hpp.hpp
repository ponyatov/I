#ifndef _H_HPP
#define _H_HPP

//#define MODULE
#define TITLE "Interpreter"
#define AUTHOR "Dmitry Ponyatov <dponyatov@gmail.com>"
#define GITHUB "https://github.com/ponyatov/I"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
using namespace std;

struct Sym {
	string tag,val;							// <T:V> pair
	Sym(string,string); Sym(string);		// constructors
	vector<Sym*> nest; void push(Sym*);		// nest[]ed elements
	map<string,Sym*> attr; Sym* next;		// env{}ironment/attributes
	virtual string dump(int=0);				// \ dump
	virtual string head();					// /
	virtual Sym* eval(Sym*);				// eval()uate/compute element
	virtual Sym* eq(Sym*B,Sym*E);			// A = B
};

struct Env:Sym { Env(string); };			// environment (symbol table)
extern Env* global;
extern void env_init();

struct Op:Sym { Op(string); 				// operator
	Sym* eval(Sym*); };

struct Vector:Sym { Vector(); 				// [vector]
	string head(); };

extern int yylex();							// lexer/parser interface
extern char* yytext;
extern int yylineno;
#define TOC(C,X) { yylval.o = new C(yytext); return X; } /* token */
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_HPP
