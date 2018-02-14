#include<iostream>
#include<iomanip>
#include<fstream>

#include<cstdlib>
#include<cstdio>
#include <cctype>

#include<string>
#include<vector>
#include<set>
#include<map>
#include<algorithm>


using namespace std;

#ifndef _SIMPLIFY_GRAMMAR_
#define _SIMPLIFY_GRAMMAR_

// Type Declarations
typedef map<string,vector<vector<string> > > Grammar ;
typedef pair<const string,vector<vector<string > > > RuleSet ;
typedef vector<vector<string> > Rules ;
typedef vector<string> Rule ;
typedef set<string> Symbols ;
typedef string Symbol ;
typedef map<Symbol,Symbols> FFSet ; // used to store first and follow sets ,, 
extern Grammar grammar ;
extern Symbol startsymbol ;

// extern Symbols nonterminals, terminals ;

// Functions
// initgrammar.cpp
// This function initializes grammar from a file 
// Comments start with '#' and non-terminals with capital letter and rest are terminals
// '->' implies symbol 
void initgrammar(string filename);  // initializes grammar from given file
void printgrammar(); // This function prints the grammar
void addrule(Symbol nonterminal, Rule &rule); // this function adds a new rule to to grammar and clears the rule.
bool isterminal(Symbol symbol) ; // this function checks if given symbol is terminal or not
void getsymboldescription(Symbols &nonterminals, Symbols &terminals, Symbols &derivingsymbols); // this gets all the symbols and groups them to different types
void printsymbols(Symbols symbols) ; // prints the symbols
void printrule(Symbol nonterminal,Rule rule); // it prints a rule ...
vector<Symbol> vectorize(Symbols symbols); // vectorizes the given set

// grammarpreprocess.cpp
void removeduplicaterules(); // removes duplicate rules from grammar
void removeunneccessaryderivingsymbols();
void removenullproductions();
void removeunitproductions();
void removenonderivingsymbols();
void removenonreachablesymbols();

// grammerpostprocessing.cpp
void removeleftrecursion();
void leftfactor();

// firstfollow.cpp
void printffset(FFSet ffset);
FFSet getfirst() ;
FFSet getfollow();
void printfirstofrules() ;
 bool checkforLL1Grammar(); // checks if grammar is LL1 
#endif
