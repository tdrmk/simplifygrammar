#include "simplifygrammar.h"

// this function removes duplicate rules  from grammar
void removeduplicaterules() {
	for ( RuleSet &ruleset : grammar ) {
		Rules &rules = ruleset.second ;
		set<Rule>  setofrules (rules.begin(),rules.end()); // store in set 
		rules = vector<Rule>(setofrules.begin(),setofrules.end()); // convert back to vector form 
	}
}
// this function removes deriving symbols not dering anything 
void removeunneccessaryderivingsymbols() {
	Symbols nonterminals,terminals,derivingsymbols;
	getsymboldescription(nonterminals,terminals,derivingsymbols);
	for ( Symbol derivingsymbol : derivingsymbols ) {
		// for each deriving symbol check if any rule it derives
		if ( grammar.count(derivingsymbol) > 0 && grammar[derivingsymbol].size() == 0 ) {
			grammar.erase(derivingsymbol); // if symbol deriving no rule remove it ..
		}
	}

}
// checks if given rule (list of Symbols) all symbols are in nullables set 
bool contained_in(Rule rule, Symbols nullables){
	for ( Symbol symbol : rule ) 
		// for each symbol check if it not belongs to the nullables
		if ( nullables.count(symbol) == 0 ) return 	false ;

	return true ;
}

// This function removes all null productions ( A -> null )
void removenullproductions() {
	Symbols nullables ;
	Symbols nonterminals,terminals,derivingsymbols;
	getsymboldescription(nonterminals,terminals,derivingsymbols);

	// Find Nullable Non-terminals directly from rules
	for ( Symbol nonterminal : nonterminals ) {
		if (grammar.count(nonterminal) > 0 ){
			Rules & rules= grammar[nonterminal];
			for (Rule & rule : rules) {
				if (rule.size() == 0 )
					nullables.insert(nonterminal) ;// cout << "Nullable " << nonterminal << endl ;
			} 
		}
	}
	cout << "Initial Set of Nullables: " ; printsymbols(nullables);
	Symbols newnullables = nullables ; ;
	// Find all nullable non-terminals
	do {
		nullables = newnullables ;
		for ( Symbol nonterminal : nonterminals ) {
			if (grammar.count(nonterminal) > 0 ){
				Rules &rules= grammar[nonterminal];
				for (Rule & rule : rules) {
					if ( rule.size() != 0 && contained_in(rule,newnullables) && newnullables.count(nonterminal) == 0 ) 
						cout << "New Nullable Non-terminal: " << nonterminal << endl, newnullables.insert(nonterminal);
				} 
			}
		}
	} while ( newnullables.size() > nullables.size() ); // do it till no more new nullables
	cout << "Final Set of Nullables: " ; printsymbols(nullables);

	// Now generate new rules using nullables
	for ( Symbol nullable : nullables) {
		for ( RuleSet &ruleset : grammar ) {
			Symbol derivingsymbol = ruleset.first ;
			Rules &rules = ruleset.second ;
			for (int i = 0 ; i < rules.size() ; i ++) {
				for (int j = 0 ; j < rules[i].size() ; j ++) {
					if ( rules[i][j] == nullable ) {
						Rule newrule = rules[i];
						newrule.erase(newrule.begin()+j);
						addrule(derivingsymbol,newrule);
					}
				}
			}

		}
	}
	//printgrammar();
	// Now remove all NULL rules 
	for ( RuleSet &ruleset : grammar ) {
		Rules &rules = ruleset.second ;
		for (int i = 0 ; i < rules.size() ; i ++) {
			if (rules[i].size() == 0 ) // if null production remove it
				rules.erase(rules.begin()+i), i-- ;// decrement count to check next rule
		}
	}
	// since we have deleted rules there might exist some deriving symbols
	// deriving no rules so remove them
	removeunneccessaryderivingsymbols();
	//printgrammar();
	// Now remove all duplicate rules if any exist 
	removeduplicaterules();
	//printgrammar();
	if ( nullables.count(startsymbol) > 0 ) {
		// if start symbol is nullable then just warn
		cout << "WARNING: Start Symbol "<< startsymbol << " is NULLABLE and is ignored." << endl ;
	}
}
void removeunitproductions() {
	map<Symbol,Symbols> unitproductions ;
	Symbols nonterminals,terminals,derivingsymbols;
	getsymboldescription(nonterminals,terminals,derivingsymbols);

	// set the initial set of unit productions
	for (Symbol nonterminal: nonterminals) 
		unitproductions[nonterminal].insert(nonterminal);

	// Find all unit productions
	map<Symbol,Symbols> newunitproductions = unitproductions ;
	do {
		unitproductions = newunitproductions ;
		for ( RuleSet &ruleset : grammar ) {
			Symbol derivingsymbol = ruleset.first ;
			Rules &rules = ruleset.second;
			for (Rule rule: rules) {
				if ( rule.size() == 1  &&  rule[0] != derivingsymbol && !isterminal(rule[0]) ) {
					// if unit production found 
					// Note: Terminals and Rules like A->A not included as unit productions 
					Symbol symbol = rule[0] ; // derivingsymbol -> symbol
					for ( Symbol unitproduction : unitproductions[symbol]) {
						newunitproductions[derivingsymbol].insert(unitproduction) ;
					}
				}
			}
		}
	}
	while ( newunitproductions != unitproductions ) ;
	// Print the Unit Productions
	for ( pair<const Symbol,Symbols> unitproductionlist : unitproductions ) {
		cout << unitproductionlist.first << " : " ; 
		printsymbols(unitproductionlist.second);
	}

	// Now remove all unit productions from grammar
	for (RuleSet &ruleset : grammar) {
		Rules &rules = ruleset.second ;
		for (int i = 0 ; i < rules.size() ; i ++ ) {
			if (rules[i].size() == 1 && ! isterminal(rules[i][0]) ) {
				// if not a terminal then it is unit production remove it
				rules.erase(rules.begin()+i) ;
				i-- ; // decrement to check the next rule ..
			}
		}
	}
	//printgrammar();
	// since we have erased rules 
	// there may be some non-deriving terminals remove them
	removeunneccessaryderivingsymbols();
	//printgrammar();
	// Now add new rules 
	for ( pair<const Symbol,Symbols> unitproductionlist : unitproductions ) {
		Symbol derivingsymbol = unitproductionlist.first ; 
		Symbols unitproductionset = unitproductionlist.second ;
		for ( Symbol symbol : unitproductionset ) {
			if ( symbol != derivingsymbol && 
				grammar.count(symbol) > 0 && 
				grammar[symbol].size() > 0 ) {
				// check if not the same symbol ..
				// then add all rules of symbol to deriving symbol
				for (Rule rule: grammar[symbol]) {
					addrule(derivingsymbol,rule);
				}
				// Now remove all duplicate rules if any exist 
				removeduplicaterules();
			}
		}
	}
	//printgrammar();
	// Now remove all duplicate rules if any exist 
	// Just in case ensuring not necessay here
	removeduplicaterules();
	//printgrammar();
}

// this function removes all rules not deriving terminals 
// Note: NULL productions must be eliminated for it to work as here 'null' is not considered at all
void removenonderivingsymbols(){
	Symbols nonterminals,terminals,derivingsymbols;
	getsymboldescription(nonterminals,terminals,derivingsymbols);
	
	Symbols derivingterminals  = terminals; // symbols that derive terminals
	// Initialize with set of terminals
	cout << "Initial Deriving Symbols: " ; printsymbols(derivingterminals);
	Symbols newderivingterminals = derivingterminals ;
	do {
		derivingterminals = newderivingterminals ;
		for (RuleSet &ruleset: grammar) {
			Symbol derivingsymbol = ruleset.first ; // LHS 
			Rules &rules = ruleset.second ; // corresponding rules
			for (Rule &rule : rules) {
				if ( newderivingterminals.count(derivingsymbol) == 0 && // if not  existing  derivable symbol
					contained_in(rule,newderivingterminals)  ){
					cout << "New Deriving Symbol :" << derivingsymbol << endl;
					newderivingterminals.insert(derivingsymbol) ;
				}
			}
		}

	} while (newderivingterminals != derivingterminals );
	cout << "All Deriving Symbols: " ; printsymbols(derivingterminals);

	// eliminate rules not containing derivingterminals
	for ( RuleSet &ruleset : grammar ) {
		Symbol derivingsymbol = ruleset.first ; // LHS 
		Rules &rules = ruleset.second ; // corresponding rules
		for ( int i = 0 ; i < rules.size() ; i ++ ) {
			// for each rule check if deriving ...
			if ( ! contained_in(rules[i],derivingterminals) ) {
				// if non deriving rule remove it
				cout << "Removing Rule: " ; printrule(derivingsymbol, rules[i]) ;  	
				rules.erase(rules.begin() + i); 
				i-- ; // to check the next rule 
			}
		}
	}

	// now remove symbols not deriving at all
	removeunneccessaryderivingsymbols();
}
// removes all rules not reachable from start symbol
void removenonreachablesymbols() {

	// Start from startsymbol and obtain all reachable symbols
	Symbols reachablesymbols ;
	// Initialize with start symbols
	reachablesymbols.insert(startsymbol);
	//get all reachable symbols
	Symbols newreachablesymbols = reachablesymbols;
	do {
		reachablesymbols = newreachablesymbols ;
		for ( Symbol symbol : reachablesymbols ) {
			// expand all deriving non-terminals to get more reachable symbols
			if ( ! isterminal(symbol) && grammar.count(symbol) > 0 && grammar[symbol].size() > 0){ // If non-terminal and deriving
				for ( Rule rule : grammar[symbol] ) // for each rule it derives
					for ( Symbol tmpsymbol : rule ) // get all symbols
						if ( newreachablesymbols.count(tmpsymbol) == 0 ) // if not already present in reachable symbols
							cout << "New Reachable Symbol :" << tmpsymbol << endl,
							newreachablesymbols.insert(tmpsymbol); // insert into reachable symbols
			}
		} 
	} while (newreachablesymbols != reachablesymbols);
	// Print All reachable symbols
	cout << "All Reachable Symbols : " ; printsymbols(reachablesymbols);
	//printgrammar();
	// remove all rules such that the deriving symbol is not reachable
	Symbols nonterminals,terminals,derivingsymbols;
	getsymboldescription(nonterminals,terminals,derivingsymbols);

	for (Symbol derivingsymbol: derivingsymbols) {
		if (grammar.count(derivingsymbol) > 0 && // Just for sake of verification
			 reachablesymbols.count(derivingsymbol) == 0 ) { // but not a reachable symbol	
			grammar.erase(derivingsymbol);
			cout << "Removing Productions of " << derivingsymbol << endl ;
		}	
	}
	// not required but just for completeness
	removeunneccessaryderivingsymbols();
}