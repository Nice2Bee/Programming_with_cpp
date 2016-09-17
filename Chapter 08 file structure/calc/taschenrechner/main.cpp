/*
*			Calculator
*
*	Book: "Programming: Principles and Practice Using C++"
*
*	07.29.2016: fix userHelp(), fix ignore() (Jordan)
*	------------------
*	07.28.2016: added Variables (Jordan)
*	------------------
*	First version:
*	07.27.2016
*	by Jordan (github: Nice2Bee)
*
*	simple calculator
*	gets user in-/output by cin/cout
*
*	Commands:
*	Print:	';'
*	Quit:	'q'
*	Allowed mathematical operators: '+', '-', '*', '/', '(', ')', '{', '}', '%'
*	Declare Variable: let "Variable name" = "Variable value"; e.g.: let a = 4;
*
*	-------------------------------------------------------------------------
*	Grammar:
*	Terminals:		{v,z,+,-,*,/,!,%,(,),{,}}, (z � 0,1,2,3,4,5,6,7,8,9), (v � R)
*	Nonterminals:	{S,V,E,T,F,P,I}
*
*
*	*****************************************
*	* S => E | V							*
*	* V => V(v)								*
*	* E => T | E+T | E-T					*
*	* T => F | T*F | T/F | T%F				*
*	* F => P | P!							*
*	* P => z | +z | -z	| (E) | {E} | v		*
*	*****************************************
*	Funktionnames:	S = statement()
*					V = declaration()
*					E = expression()
*					T = term()
*					F = isFacto()
*					P = primary()
*	-------------------------------------------------------------------------
*
*
*	Known issues:
*	1. Wrong input like 5+*(3+3)+2+2; will return an error and the result =4;
*		-void TokenStream::ignore(char kind)
*		-cin.putback(';'); // to make sure the whole input has an end
*
*/

#include <stdlib.h>
#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctype.h>

#include "token.h"
#include "error.h"

using namespace std;

inline void keep_window_open();

void userHelp();

void cleanUpMess(TokenStream&);

double facto(double d);

double declaration(TokenStream&); //declerates a Variable
bool isDeclared(string var); // returns true if a variable is already used
double defineName(string var, double value, TokenStream& tokenStream); // pushbacks an new Variable on the variable_table
void varOverwrite(string var, double value); // User interaction if a Variable will be overwriten

double getValue(string s); // searches in variable_table for a variable and returns its value
void setvalue(string s, double v); // searches in variable_table for a variable and sets its value



double statement(TokenStream&);
double expression(TokenStream&);
double term(TokenStream&);
double isFacto(TokenStream&);
double primary(TokenStream&);

inline void keep_window_open() { char ch; cin >> ch; }



// Class Variable and Variable vector:
class Variable { // Stores a variable with name and value
public:
	string name;
	double value;
	Variable(string n, double v) :name(n), value(v) {} 
};

vector<Variable> variable_table; // Vector stores all variables defined by user or in main
							 // Funktions working with variables:
double getValue(string s) { // searches in variable_table for a variable an returns its value
	for (int i = 0; i < variable_table.size(); i++) {
		if (variable_table[i].name == s) {
			return variable_table[i].value;
		}
	}
	error("Variable::getValue", "no such Variable: " + s);
}

void setvalue(string s, double v) { // searches in variable_table for a variable and sets its value
	for (int i = 0; i < variable_table.size(); i++) {
		if (variable_table[i].name == s) {
			variable_table[i].value = v;
			return;
		}
	}
	error("Variable::getValue", "no such Variable: " + s);
}

double declaration(TokenStream& tokenStream) { //declerates a Variable
	Token t1 = tokenStream.get();
	if (t1.kind != name) error("::decleration()", "Decleration needs a name");
	string varName = t1.varName;

	Token t2 = tokenStream.get();
	if (t2.kind != '=') error("::decleration()", "No \"=\" in the decleration");

	double right = expression(tokenStream);
	defineName(varName, right, tokenStream);
	return right;
}

bool isDeclared(string var) {
	for (int i = 0; i < variable_table.size(); i++) {
		if (var == variable_table[i].name) return true;
	}
	return false;
}

void varOverwrite(string var, double value) { // User interaction if a Variable will be overwriten
	cout << "Variable: " + var + " is allready declared as:  ";
	cout << var + " = " << getValue(var);
	cout << "\noverwrite? y / n: ";
	char answer;
	cin >> answer;
	if (answer == 'y') {
		setvalue(var, value);
	}
}

double defineName(string var, double value, TokenStream& tokenStream) {
	if (isDeclared(var)) {
		varOverwrite(var, value);
	}

	variable_table.push_back(Variable(var, value));
	return value;
}


//

double facto(double d) //calculates factorial
{
	if (d == 0) return 1;
	return d * facto(d - 1);
}

// Gramma:
double statement(TokenStream& tokenStream) {
	Token t = tokenStream.get();
	switch (t.kind)
	{
	case let:
		return declaration(tokenStream);
	default:
		tokenStream.putback(t);
		return expression(tokenStream);
	}
}

double expression(TokenStream& tokenStream) {
	double left = term(tokenStream);
	Token t = tokenStream.get();
	while (true) {
		switch (t.kind)
		{
		case '+':
			left += term(tokenStream);
			t = tokenStream.get();
			break;
		case '-':
			left -= term(tokenStream);
			t = tokenStream.get();
			break;
		default:
			tokenStream.putback(t);
			return left;
		}
	}
}

double term(TokenStream& tokenStream) {
	double left = isFacto(tokenStream);
	Token t = tokenStream.get();
	double rigth;
	int moduloTest1;
	int moduloTest2;
	while (true)
	{
		switch (t.kind)
		{
		case '*':
			left *= isFacto(tokenStream);
			t = tokenStream.get();
			break;
		case '/':
			rigth = isFacto(tokenStream);
			if (rigth == 0) error("::term()", "No 0 division!");
			left /= rigth;
			t = tokenStream.get();
			break;
		case '%':
			moduloTest1 = int(left);
			rigth = isFacto(tokenStream);
			moduloTest2 = int(rigth);
			if (moduloTest1 != left || moduloTest2 != rigth)
				error("::term()", "Modulo not defined for decimal numbers");
			if (rigth == 0) error("::term()", "No 0 division!");
			return moduloTest1 % moduloTest2;
		default:
			tokenStream.putback(t);
			return left;
		}
	}
}

double isFacto(TokenStream& tokenStream) {
	double left = primary(tokenStream);
	Token t = tokenStream.get();
	while (true)
	{
		switch (t.kind)
		{
		case '!':
			left = facto(left);
			t = tokenStream.get();
			break;
		default:
			tokenStream.putback(t);
			return left;
		}
	}
}

double primary(TokenStream& tokenStream) {
	Token t = tokenStream.get();
	double d;

	switch (t.kind)
	{
	case number:
		return t.value;
	case '{':
		d = expression(tokenStream);
		t = tokenStream.get();
		if (t.kind != '}') error("::primary()", "no closing braces ");
		return d;
	case '(':
		d = expression(tokenStream);
		t = tokenStream.get();
		if (t.kind != ')') error("::primary()", "no closing braces ");
		return d;
	case '-':				// allows numbers to start with - or +
		return -primary(tokenStream);
	case '+':
		return primary(tokenStream);
	case name:
		return getValue(t.varName);
	default:
		error("::primary", "not a real function");
	}
}


// Calculator
void calculate(TokenStream& tokenStream) {

	double value = 0;

	while (true)
	{
		try
		{

			Token t = tokenStream.get(); //reads token from cin
			if (t.kind == quit) break;
			if (t.kind == help) { userHelp(); continue; }
			if (t.kind == print) {
				cout << '=' << value << '\n';
				t = tokenStream.get();
				while (t.kind == print) t = tokenStream.get(); // eats multiple ';' (print)
				tokenStream.putback(t);
				continue;
			}
			else {
				tokenStream.putback(t);
			}

			value = statement(tokenStream); // Starts gramma
		}
		catch (exception & e)
		{
			cerr << e.what();
			cleanUpMess(tokenStream);
		}
	}

}

int main()
{
	try
	{
		TokenStream tokenStream;
		defineName("pi", 3.14159265359, tokenStream);
		defineName("e", 2.71828182846, tokenStream);


		cout << "::::::: Calculator 1.1 :::::::\nhelp: h\nquit: q\n";
		calculate(tokenStream);
		cout << "See you!";
		keep_window_open();
		return 0;
	}
	catch (exception & e) {
		cerr << e.what();
		keep_window_open();
		return 1;
	}
	catch (...) {
		cerr << "Unknowen Error!";
		keep_window_open();
		return 2;
	}
}

void userHelp() {
	cout << "\nHelp:\n\n"
		"Commands:\n"
		"Print:\t;\n"
		"Quit:\tq\n\n"
		"Allowed mathematical operators : +, -, *, /, (, ), {, }, %, pi, e\n\n"
		"Declare Variable : let\n"
		"\"Variable name\" = \"Variable value\"\n"
		"Usage:\n"
		"e.g.:\t\ninput:\tlet a = 4 / 2\n"
		"input:\t2 + a;\n"
		"output:\t= 4;\n"
		"input:\t3 * (4 + {3 - 3}+2) / 2;\n"
		"output:\t= 9\n"
		"input:\tpi+e;\n"
		"output:\t=5.85987\n";
}