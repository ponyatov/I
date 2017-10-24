#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cout<<YYERR; cerr<<YYERR; exit(-1); }
int main() { env_init(); return yyparse(); }

Sym::Sym(string T,string V) { tag=T; val=V; }
Sym::Sym(string V):Sym("sym",V){}
void Sym::push(Sym*o) { nest.push_back(o); }

string Sym::head() { ostringstream os;
	os << "<"+tag+":"+val+"> @"<<this; return os.str(); }
string pad(int n) { string S; for (int i=0;i<n;i++) S += '\t'; return S; }

set<Sym*> dump_used;
string Sym::dump(int depth) {
	// header
	string S = "\n"+pad(depth)+head();
	// break on infty dump recursion
	if (depth==0) dump_used.clear();
	if (dump_used.count(this)) return S+" ...";
	dump_used.insert(this);
	// attr{}ibutes
	for (auto it=attr.begin(),e=attr.end();it!=e;it++)
		S += "\n"+pad(depth+1)+it->first+" = "+it->second->dump(depth+2);
	// nest[]ed elements
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		S += (*it)->dump(depth+1);
	return S; }

Sym* Sym::eval(Sym*E) {
	if (E->attr.count(val)) return E->attr[val];
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		(*it) = (*it)->eval(E);
	return this; }

Sym* Sym::eq(Sym*o, Sym*E) {
	E->attr[val] = o;
	return o; }

Op::Op(string V):Sym("op",V){}

Sym* Op::eval(Sym*E) { Sym::eval(E);
	if (val=="=") return nest[0]->eq(nest[1],E);
	return this; }

Vector::Vector():Sym("vector","[]"){}
string Vector::head() { ostringstream os;
	os << "[] @"<<this; return os.str(); }

Env::Env(string V):Sym("env",V){}
Env* global = new Env("global");
void env_init() { global->attr["global"]=global;
	global->attr["MODULE"] = new Sym(MODULE);
	global->attr["TITLE"] = new Sym(TITLE);
	global->attr["AUTHOR"] = new Sym(AUTHOR);
	global->attr["GITHUB"] = new Sym(GITHUB);
}
