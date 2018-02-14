#include "simplifygrammar.h"

// this function returns a new non-terminal that does not exist in grammar by modifying given symbol ...
Symbol nextavailablenonterminal ( Symbol symbol) { 
// NOTE: Call with only non-terminal only checks with non-terminal and also why add new terminal
	Symbols nonterminals,terminals,derivingsymbols;
	getsymboldescription(nonterminals,terminals,derivingsymbols); // get existing symbols 
	Symbol newnonterminal ; // construct a new non-terminal
	while (true) { // keep looping till a new symbol is found
		if ( nonterminals.count(symbol) == 0 )  // if found return it
			return symbol ;
		// if non-terminal symbol not available
		// construct a new symbol and check its availability
	
		int index = symbol.find("_"); // check if contains '_' underscore
		if (index == string::npos ) { // if not found
			// if underscore not found
			newnonterminal = symbol + "_1" ; // add and check
			// we don't know if it is available
		}
		else {
			// if underscore found
			try {
				int number = stoi(symbol.substr(index+1));
				number ++ ;
				newnonterminal = symbol.substr(0,index) + "_" + to_string(number) ; // check with increased count ...
			} catch (const invalid_argument &ia) {
				// if next is not a number
				newnonterminal = symbol + "_1" ; // if _ part of symbol
			}
			// still we dont know availability
		}
		symbol = newnonterminal ;

	}
	return newnonterminal ; // not reachable
}

// Run this only after reprocessing
// Note: Using Algorithm from TEXT BOOK
// Eliminate Cycles and Null productions before running these .. 
// Note: This one generates NULL productions
void removeleftrecursion(){
	removeduplicaterules(); removeunneccessaryderivingsymbols() ;// just for sake of completeness 
	Symbols nonterminals,terminals,derivingsymbols;
	getsymboldescription(nonterminals,terminals,derivingsymbols);
	vector<Symbol> nonterminalslist =  vectorize(nonterminals);
	// deriving and non-terminals must be same ...
	for (int i = 0 ; i < nonterminalslist.size() ; i ++) {
		Symbol nonterminal = nonterminalslist[i] ;
		if ( grammar.count(nonterminal) > 0 && grammar[nonterminal].size() > 0)  // just for check .. not needed if preprocessing functions run before
		{
			
			for (int j = 0 ; j < i ; j ++) {
				// replace each production of the form Ai-> Ajx by 
				// prodcutions Ai -> a1 x | a2 x| .. | an x where
				// Aj -> a1 | a2 | a3 are all current Aj productions
				Symbol prevsnonterminal = nonterminalslist[j]; // a non-terminal from j < i
				Rules &rules = grammar[nonterminal]; // get all the rules corresponding to non-terminal
				for ( int m = 0 ; m < rules.size() ; m ++ ) {
					Rule rule = rules[m] ;
					// for each rule produced by non-terminal check if contains Aj at the start
					if ( rule.size() > 0 && rule[0] ==  prevsnonterminal && 
						grammar.count(prevsnonterminal) > 0 && grammar[prevsnonterminal].size() > 0 ) {
						// if non-terminal matching replace the existing rules of non-terminal
						// using productions of  prevsnonterminal
						rules.erase(rules.begin()+m); // remove the rule iff prevsnonterminal derives something
						rule.erase(rule.begin()); // remove the starting non-terminal
						for ( Rule otherrule: grammar[prevsnonterminal])  // append the original part to the otherrule and add that rule
							otherrule.insert(otherrule.end(), rule.begin(), rule.end()) , addrule(nonterminal,otherrule);
						m -- ; // to check next rule
					}
				}
			}
			Rules rules = grammar[nonterminal]; // get all the rules corresponding to non-terminal
			// eliminate the immediate left recursion among Ai productions
			Symbol newnonterminal = nextavailablenonterminal(nonterminal);
			Rules terminations, followers ; 
			// terminations are those rules without left recursion 
			// while followers are those following left recursion
			for ( Rule rule : rules )
				if ( rule.size() > 0 && rule[0] == nonterminal) // if first symbol matching the non-terminal 
					rule.erase(rule.begin()), followers.push_back(rule); // remove the first symbol and add to followers
				else 
					terminations.push_back(rule); // else add to terminations
			// if left recursion exist
			if (followers.size() > 0) {
				grammar.erase(nonterminal); // erase all existing rules of that non-terminal
				for ( Rule rule : terminations ) // construct rules for existing non-terminal from terminations
					rule.push_back(newnonterminal), addrule(nonterminal,rule);  // append newnonterminal to each rule and then add
				Rule emptyrule ;
				addrule(newnonterminal,emptyrule); // add a null production
				for ( Rule rule : followers ) // add new rules for newnonterminal based on followers first append newnonterminal
					rule.push_back(newnonterminal), addrule(newnonterminal, rule);
			}
		}
	}
}
// returns the length of common prefix ...
int commonelements(Rule rule1, Rule rule2) {
	if ( rule1.size() == 0 ||  rule2.size() == 0 ) 
		return 0 ;
	// if none are zero ...
	int num = (rule1.size() > rule2.size() ) ? rule2.size(): rule1.size() ;
	int i ;
	for (i = 0 ; i < num ; i ++) 
		if ( rule1[i] == rule2[i] ) continue ;
		else break ;
	return i ; 
}
// rule this after preprocessing only...
void leftfactor() {
	removeduplicaterules(); 
	removeunneccessaryderivingsymbols() ;
	
	Symbols nt,terminals,derivingsymbols;
	getsymboldescription(nt,terminals,derivingsymbols);
	vector<Symbol> nonterminals = vectorize(nt);
	// for each non-terminal	
	for (int i = 0 ; i < nonterminals.size() ; i ++) {
		Symbol nonterminal = nonterminals[i] ;
		if ( grammar.count(nonterminal) > 0 && grammar[nonterminal].size() > 0 ) {
			Rules rules = grammar[nonterminal]; // get all rules of the non-terminal
			Grammar tmpgrammar ; // construct a temporary grammar structure with first symbol ..
			for ( Rule rule : rules ) 
				if ( rule.size() == 0 ) // null production
					tmpgrammar["null"].push_back(rule); // add null productions under the name of "null"
				else 
					tmpgrammar[rule[0]].push_back(rule);
			grammar.erase(nonterminal); // remove the original set of rules ...
			for ( RuleSet ruleset : tmpgrammar ) { // for each ruleset in tmp grammar
				Symbol first = ruleset.first ; // get the first symbol 
				Rules tmprules = ruleset.second ; // and corresponding rules with same first symbol
				if ( first == "null" ) // If the first is a null
					addrule(nonterminal,tmprules[0]); // empty rule ( add a null production )
				else if (tmprules.size() == 0); // if no rule (not possible)
				else if ( tmprules.size() == 1) // if single rule for given first ...
					addrule(nonterminal,tmprules[0]) ; // add that rule
				else {
					// if more than one rule ...
					vector <int> commonalities ; // store length of common prefix among consequtive rules 
					for ( int m = 1 ; m < tmprules.size() ; m ++ ) {
						commonalities.push_back (commonelements(tmprules[m],tmprules[m-1]) ); // add push them to vector
					}
					int min = *min_element(commonalities.begin(),commonalities.end()); // get the minimum number of common elements
					// this will be size of common prefix ..
					// Note: min will atleast be 1 ...
					Rule tmprule(tmprules[0].begin(),tmprules[0].begin()+min); // store the common first part 
					for (int m = 0 ; m < tmprules.size() ; m ++ )  // remove it from other rules ..
						tmprules[m].erase(tmprules[m].begin(),tmprules[m].begin() + min); // delete first min elements ..
					Symbol newnonterminal = nextavailablenonterminal (nonterminal); // get a new non-terminal
					tmprule.push_back(newnonterminal); // add it to the prefix
					addrule(nonterminal,tmprule); // add that as a new rule
					for (int m = 0 ; m < tmprules.size() ; m ++)
						addrule(newnonterminal,tmprules[m]); // place the suffix under a new non-terminal
					nonterminals.push_back(newnonterminal); // add the new non-terminal for further examination 
				}
			}
		}
	}
	// Just for sake of completeness to clean any mess..
	removeduplicaterules(); 
	removeunneccessaryderivingsymbols() ;
}