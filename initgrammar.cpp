#include "simplifygrammar.h"

// Assumes Valid grammar
// Non-terminal is before '->'
Grammar grammar ;
Symbol startsymbol ;

// this function initializes the grammar
// gets all the rules from file and stores them in Grammar structure
// Rules must be of the form
// NON_TERMINAL -> NT1 NT2 t1 t2 
// non-terminals start with capital letter while rest are treated as terminals
// space, tab, new line treated as delimiters, and comments begin with '#' and a delimiter like space 
// comments inline or own-line ignored ..
// First Rule RHS treated as start symbol ...
// It does not accept invalid grammar will result is some error somewhere (like terminal deriving a rule )and also it does not work with 
// ambiguous grammar (not tested ... mostly will work) 
// null productions represented as 
// NT -> null
// multiple rules grouped together using |
// A -> B b | C c | null
// takes filename as argument and constructs a grammar 
// it must be called at start 
void initgrammar(string filename){
	// uncomment this line if you want to initialize a new grammar each time
	// currently you can call this on two files grammar rules get appended
	//grammar.clear() ; // to clear the grammar
	ifstream file (filename); // open file
	Symbol nextsymbol, nonterminalnew, nonterminalold ; // these are used to keep track of symbols
	Rule rule ; // this stores currently read non-terminals following a '->' or start of file or 'null' or '|'
	if (file.is_open()) { // if file successfully opened scan through it
		// NOTE: space, tab, new line are all natural delimiters 
		while (file >> nextsymbol) { // for each symbol
			if (nextsymbol == "#") { // if it is comment
				// Comment
				getline(file,nextsymbol); // ignore the line .. the remaining part of line not looked for any rule
			}
			else if (nextsymbol == "->") {
				// implies new rule
				nonterminalnew = rule.back(); // this implies last/prev symbol was non-terminal
				rule.pop_back();  // pop it out
				if ( ! rule.empty() ) {
					// if previously rule was read and not added then add it to grammar
					addrule(nonterminalold,rule);
				}
				if (grammar.empty()) {
					// no rule in grammar then it is the start symbol (the first symbol)
					startsymbol = nonterminalnew ;
					cout << "Note: Start Symbol (" + startsymbol + ")" << endl ;
				}
				nonterminalold = nonterminalnew ;
			}
			else if ( nextsymbol == "|") {
				// Add the previous rule to the grammar
				if ( ! rule.empty() ) // just a check may be previous rule was null
					addrule(nonterminalold,rule);
			}
			else if ( nextsymbol == "null" ) {
				// if null rule .. then rule must be empty just add it if valid grammar
				if ( ! rule.empty() )
					cout << "WARNING: symbols exist before a null production." << endl ;
				rule.clear() ; // for sake of completeness
				addrule(nonterminalold,rule);
			}
			else {
				// if just another symbol
				rule.push_back(nextsymbol);
			}
		}
		if ( !rule.empty() ) {
			// if any rule remaining to be added add it
			addrule(nonterminalold,rule);
		}
		// once all rule obtained clean them of duclicates
		removeduplicaterules();
		file.close();	
	}
	else {
		perror("Unable to open file.");
		exit(EXIT_FAILURE);
	}
} 

// this function checks if given symbol is terminal or not
bool isterminal(Symbol symbol) {
	if (isupper(symbol[0])) return false ;	// symbol is non-terminal if it begins with capital letter
	else return true ;
}

vector<Symbol> vectorize(Symbols symbols) {
	// this function converts from a set of Symbols to vector of symbols
	return vector<Symbol> (symbols.begin(), symbols.end() );
}

// gets terminals, non-terminals and deriving symbols (symbols on the RHS of rules, Need to be terminals)  
void getsymboldescription(Symbols &nonterminals, Symbols &terminals, Symbols &derivingsymbols)  {
	terminals.clear(); nonterminals.clear(); derivingsymbols.clear();
	for (RuleSet &ruleset : grammar) {
		// this cannot be modified ruleset.first
		Symbol derivingsymbol = ruleset.first ; // get the symbol on the RHS of rules
		derivingsymbols.insert(derivingsymbol);
		Rules &rules = ruleset.second ; // get the set of rules derived by the deriving symbol
		if ( isterminal(derivingsymbol) ) {
			// if deriving symbol is a terminal INVALID
			terminals.insert(derivingsymbol);
			cout << "WARNING: getterminals(): Deriving Symbol (" + derivingsymbol + ") is a terminal. " << endl ;  
		}
		else 
			nonterminals.insert(derivingsymbol); // Add it as terminal

		for ( Rule &rule : rules ) 
			for ( Symbol symbol : rule  ) // for each rule
				if ( isterminal(symbol) ) // check for terminals
					terminals.insert(symbol);
				else 
					nonterminals.insert(symbol);
	}
}
// prints a set of symbols
void printsymbols(Symbols symbols) {
	cout << "(" << symbols.size() << ")" ;
	for ( Symbol terminal : symbols ) 
		cout << " " << terminal << " " ;
	cout << endl ;
}

void printgrammar() {
	// get the symbols of the grammar
	Symbols nonterminals,terminals,derivingsymbols;
	getsymboldescription(nonterminals,terminals,derivingsymbols);
	
	cout << "Start Symbol : " << startsymbol << endl ;
	cout << "Terminals : " ;printsymbols(terminals);
	cout << "Non-Terminals : " ; printsymbols(nonterminals);
	cout << "Deriving Symbols : "; printsymbols(derivingsymbols);
	cout << "Rules : " << endl ;
	int count = 1 ;
	// get maz size of deriving symbol ..
	int maxsize = 0 ;
	for (RuleSet &ruleset : grammar) {
		Symbol derivingsymbol = ruleset.first ;
		if ( derivingsymbol.size() > maxsize  ) maxsize = derivingsymbol.size() ;
	}
	for (RuleSet &ruleset : grammar) {
		Symbol derivingsymbol = ruleset.first ; // get the symbol on the RHS of rules
		Rules &rules = ruleset.second ; // get the set of rules derived by the deriving symbol
		if (rules.size() == 0) {
			// if no rule
			cout << "WARNING: Unneccessary deriving symbol (" << derivingsymbol << ")" << endl ;
		}

		for ( Rule &rule : rules ) {
			cout << setw(4) << count << ". "  << setw(maxsize); // print count   
			printrule(derivingsymbol,rule) ;  // print rule 
			count ++ ; // increment count 
		}
	}
}

// this function adds a rule to the grammar and clears the rule
void addrule(Symbol nonterminal, Rule &rule) {
	grammar[nonterminal].push_back(rule); // grammar is a map while rule is vector
	rule.clear() ; // clear the rule
}
void printrule(Symbol nonterminal,Rule rule) {
	cout << nonterminal << " -> " ;
	if ( rule.empty() ) // null production
		cout << "--null--" ;
	for ( Symbol symbol : rule )
		cout << " " << symbol << " " ;
	cout << endl ;
}