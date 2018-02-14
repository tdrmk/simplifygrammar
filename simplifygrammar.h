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
// NOTE: All these functions work only for valid grammar 
// see initgrammar.cpp for more information ..

// Type Declarations
typedef map<string,vector<vector<string> > > Grammar ; // this is used to store the grammar rules
typedef pair<const string,vector<vector<string > > > RuleSet ; // each non-terminal can derive may rules this for each non-terminal vector of rules
typedef vector<vector<string> > Rules ; // the vector of rules
typedef vector<string> Rule ; // rule is a array of symbol
typedef set<string> Symbols ; // this is a set of symbols
typedef string Symbol ; // symbol is a string , Those starting with a capital letter is a Nonterminal, rest are terminals 
typedef map<Symbol,Symbols> FFSet ; // used to store first and follow sets ,, 
extern Grammar grammar ; // all functions use this to do their manipulations
extern Symbol startsymbol ; // stores the start symbol ..

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
void removeunneccessaryderivingsymbols(); // removes all non-terminals that don't derive any rule
void removenullproductions();// removes null productions from grammar and adds new rules accordingly
void removeunitproductions(); // removes unit productions from grammar add adds new rules accordinglu
void removenonderivingsymbols(); // removes all rules that result in non-terminals that don't derive a terminal
void removenonreachablesymbols(); // removes all rules that cannot result from start symbol (sentential form)

// grammerpostprocessing.cpp
// these functions usually applied after grammar preprocessing (the above functions)
void removeleftrecursion(); // this helps to remove left recusion from grammar
void leftfactor(); // helps to remove common left prefix (some grammars don't appear LL1 because of these)

// firstfollow.cpp
// apply these after above functions ..
void printffset(FFSet ffset); // this is used for printing first and follow
FFSet getfirst() ; // this returns the first of all symbols in the grammar
FFSet getfollow(); // this returns the follow of all non-terminals in the grammar
void printfirstofrules() ; // this prints all the terminals that a rule can derive
bool checkforLL1Grammar(); // used to check if  resulting grammar is LL1 
#endif
