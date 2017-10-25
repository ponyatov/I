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
	for (auto it=attr.begin(),e=attr.end();it!=e;it++)
		attr[it->first] = it->second->eval(E);
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		(*it) = (*it)->eval(E);							// MEMORY LEAK !
	return this; }

Sym* Sym::pfxadd(Sym*op) { return op; }
Sym* Sym::pfxsub(Sym*op) { return op; }

Sym* Sym::add(Sym*op,Sym*o) { return op; }
Sym* Sym::mul(Sym*op,Sym*o) { return op; }

Sym* Sym::eq(Sym*op,Sym*o, Sym*E) {
	E->attr[val] = o;
	return o; }

Sym* Sym::at(Sym*op,Sym*o) { return eval(o); }

Error::Error(string V):Sym("error",V) { yyerror(V); }

Hex::Hex(string V):Sym("hex",V) { val=strtoul(V.substr(2).c_str(),NULL,0x10); }
string Hex::head() { ostringstream os;
	os <<"0x"<< hex << uppercase << val << " @"<<this; return os.str(); }

Int::Int(string V):Sym("int",V) { val=atoi(V.c_str()); }
Int::Int(long D):Sym("int","") { val=D; }
string Int::head() { ostringstream os;
	os << "<" << tag <<":"<< val << "> @"<<this; return os.str(); }
Sym* Int::pfxadd(Sym*op) { return new Int(+val); }
Sym* Int::pfxsub(Sym*op) { return new Int(-val); }

Num::Num(string V):Sym("num",V) { val=atof(V.c_str()); }
Num::Num(double D):Sym("num","") { val=D; }
string Num::head() { ostringstream os;
	os << "<" << tag <<":"<< val << "> @"<<this; return os.str(); }
Sym* Num::pfxadd(Sym*op) { return new Num(+val); }
Sym* Num::pfxsub(Sym*op) { return new Num(-val); }

Str::Str(string V):Sym("str",V){}
string Str::head() { ostringstream os;
	os << "'" << val << "' @"<<this; return os.str(); }

Op::Op(string V):Sym("op",V){}

Sym* Op::eval(Sym*E) { Sym::eval(E);
	switch (nest.size()) {
		case 1:
			if (val=="+") return nest[0]->pfxadd(this);
			if (val=="-") return nest[0]->pfxsub(this);
		case 2:
			if (val=="+") return nest[0]->add(this,nest[1]);
			if (val=="*") return nest[0]->mul(this,nest[1]);
			if (val=="=") return nest[0]->eq(this,nest[1],E);
			if (val=="@") return nest[0]->at(this,nest[1]);
	}
	return this; }

Vector::Vector():Sym("vector","[]"){}
string Vector::head() { ostringstream os;
	os << "[] @"<<this; return os.str(); }

Dict::Dict():Sym("dict","{}"){}
string Dict::head() { ostringstream os;
	os << "{} @"<<this; return os.str(); }
Sym* Dict::eval(Sym*E) { Sym::eval(this); Sym::eval(E); return this; }

	Env::Env(string V):Sym("env",V){}
Env* global = new Env("global");
void env_init() {
	global->attr["global"] = global;
	global->attr["doc"] = new Str("global environment");
	// metainfo
	global->attr["MODULE"] = new Str(MODULE);
	global->attr["TITLE"] = new Str(TITLE);
	global->attr["AUTHOR"] = new Str(AUTHOR);
	global->attr["GITHUB"] = new Str(GITHUB);
}
