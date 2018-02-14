#include "simplifygrammar.h"

// this function used to print first and follow sets ( FFSet structure) 
void printffset(FFSet ffset){
	// get max size 
	// just for the looks of logs ..
	int maxsize1 = 0 ; // stores max size of symbol
	int maxsize2 = 0 ; // stores max count of symbols
	// this loop is not needed just for the looks
	for(pair<const Symbol,Symbols> &ffelem : ffset ){
		Symbol symbol = ffelem.first ;
		Symbols &symbols = ffelem.second ;
		// keep track of the maximum sizes so that it can be formatted well
		if ( symbol.size() > maxsize1 ) maxsize1 = symbol.size() ; 
		string count = string("(") + to_string( symbols.size() ) + string(")") ;
		if ( count.size() > maxsize2 ) maxsize2 = count.size() ;
	}
	// actual printing
	for(pair<const Symbol,Symbols> &ffelem : ffset ) {
		Symbol symbol = ffelem.first ;
		Symbols &symbols = ffelem.second ;
		string count = string("(") + to_string( symbols.size() ) + string(")") ; 
		cout << setw(maxsize1 + 3) << symbol <<  setw(maxsize2 + 1) <<  count <<  " : ";
		for ( Symbol symboltmp : symbols )
			cout << " " << symboltmp <<" ";
		cout << endl ;  
	}
}

// gets the first of vector of symbols ... useful in follow set computations
// takes as input the first set .. to avoid recomputations and vector of non-terminals
// it findsthe first of first symbol if contains null goes to the next .. till first symbol without null in their first set
// or end of rule reached then adds null
Symbols getfirst( FFSet first, Rule rule )  { // gets the first of vector of symbol
	Symbols firstset ;
	
	// if null production it contains "null" in its first set ...
	if ( rule.size() == 0) {// default case ..
		firstset.insert("null") ; // if 'null' production
		return firstset ;
	}

	bool nullpresent = false ; // flag to store if null is present in the current first set being looked at
	for (  int i = 0 ; i < rule.size() ; i ++ ) {
		Symbol symbol = rule[i] ;
		// for each symbol ...
		for ( Symbol tmpsymbol : first[symbol] ) 
			if ( tmpsymbol == "null" )
				nullpresent = true ;
			else 
				firstset.insert(tmpsymbol);
		// if null present we need to look at next symbol
		if ( nullpresent ) { // if current symbol derives null
			nullpresent = false ;
			if ( i == (rule.size() - 1) ) // if last element
				firstset.insert("null"); // add null to the set 
			continue ; // look at next symbol ...
		}
		break ; // if null not present ...
	}
	return firstset ;

}

// this function just prints first of each of the rules based on the RHS
// this is used to see what all terminals can a rule derive
void printfirstofrules() {
	FFSet first = getfirst(); // get first of all symbols
	for( RuleSet ruleset : grammar ) {
		// for each set of rules for a nonterminal
		Symbol nonterminal = ruleset.first ;
		Rules rules = ruleset.second ; // get the rules
		cout << "Rules of " << nonterminal << endl ;
		for (Rule rule : rules ) { // for each rule
			Symbols firstset = getfirst(first,rule); // compute its first of RHS
			printrule(nonterminal,rule);
			cout << "\t Derives " << " ==> " ; printsymbols(firstset);
		}
	}
}

// this function returns the first of current grammar
FFSet getfirst() {
	FFSet first;
	// get current information of terminals and non-terminals
	Symbols nonterminals,terminals,derivingsymbols;
	getsymboldescription(nonterminals,terminals,derivingsymbols);
	// Initialize it with terminals
	// first of terminal is the terminal itself
	for ( Symbol terminal :terminals )
		first[terminal].insert(terminal);

	FFSet newfirst = first ; // create another set ... update it each time new symbol must be added loop if new symbol is added
	do {
		first = newfirst ; // if they are different update the sets
		for ( RuleSet ruleset : grammar ) {
			Symbol nonterminal = ruleset.first ;
			Rules rules = ruleset.second ;
			for ( Rule rule: rules) {
				// for each rule ..
				if ( rule.size() == 0 ) // if null production ..
					newfirst[nonterminal].insert("null");
	
				else {
					// if not null production 
					// look at the symbols in each rule ...
					for (int i = 0 ; i < rule.size() ; i ++) {
						Symbol symbol = rule[i]; // get each symbol
						bool nullpresent = false ; // if null is present in its first we need to add the first of next symbol as well
						for (Symbol symboltmp : newfirst[symbol] )
							if ( symboltmp == "null" ) // if null value
								nullpresent = true ; // just set the flag and continue add other terminals
							else // if not null
								newfirst[nonterminal].insert(symboltmp);
						
						if ( nullpresent ) // if null was represent
							if ( i == ( rule.size() -1 ) ) // if last symbol add null 
								newfirst[nonterminal].insert("null"); // since no more rule to look at
							else 
								continue ; // if not last null
						else // if null absent
							break ; // no need to look forward
					}
				}
			}
		}
	} while ( first != newfirst ) ;
	// if no change / update all the first was obtained 
	// it can be done recursively or using dynamic programming 
	// make sure not to lead to a infinite loop ie deadlock
	return first ;
}
// this computes the follow set of each non-terminal in the grammar
FFSet getfollow() {
	FFSet follow ;
	// initialize the follow set 
	follow[startsymbol].insert("$") ; // follow of start symbol always contains end of string marker
	FFSet first = getfirst(); // get the first as follow is based on first of following symbols
	FFSet newfollow = follow ; // do the same thing as above loop till no more updates possible to follow set 
	// this technique does not suffer from infiinite looping thus terminates .. (USEFUL APPROACH)
	// RECURSION MAY LEAD TO INFINITE CALLS because of cycles ...
	do {
		follow = newfollow ;
		for ( RuleSet ruleset : grammar ) {
			Symbol nonterminal = ruleset.first ;
			Rules rules = ruleset.second ;
			for ( Rule rule : rules ) // for each rule
				for (int i = 0 ; i < rule.size() ; i ++) {
					if ( ! isterminal(rule[i]) ) {
						bool nullpresent = false ; //  checks if null present in the first of following non-terminals
						// if non-terminal 
						if ( i == (rule.size() -1 ) ) nullpresent = true ; // if end of rule then follow of current includes follow of parent
						else {
							Rule following ( rule.begin() + i + 1, rule.end() ) ; // symbols after current non-terminal
							Symbols firstset = getfirst(first,following); // get the first of following elements
							for ( Symbol symbol : firstset ) 
								if ( symbol == "null" ) nullpresent = true ; // if null just mark a flag .. NOTE: follow set NEVER contains a null
								else newfollow[rule[i]].insert(symbol); // add remaining symbols
						}
						if ( nullpresent ) // if null present ... Add the follow of deriving non-terminal
							for ( Symbol symbol : newfollow[nonterminal] )  // add symbols from follow of deriving non-terminal
								newfollow[rule[i]].insert(symbol);
						
					}
				}
		}
	} while ( follow != newfollow ); // repeat above till no change
	return follow ;
}

// checks if given grammar is LL1 or not .
// A Grammmar is LL1 iff whenever A -> a | b are two distinct productions of G, the following conditions hold
// 1. For no terminal x do both a and b derive strings begining with x.
// 2. Atmost one of a or b can dervice the empty string.
// 3, if b =*=> null, then a does not derive any string beginning with a terminal in FOLLOW(A).
//					Likewise, if a =*=> null, then b does not derive any string beginning with a terminal in FOLLOW(A).
// Note: This function can't handle duplicate rules and non-deriving non-terminals .
 bool checkforLL1Grammar() {
 	// cleaning if any anomaly like dulicate rules or non-deriving non-terminals exist ...
	removeduplicaterules(); 
	removeunneccessaryderivingsymbols() ;
	bool isLL1 = true ; // flag to check if LL1
 	FFSet first = getfirst (), follow = getfollow() ; // get first and follow sets ..
 	for ( RuleSet ruleset : grammar ) {
 		// fetch each ruleset ..
 		Symbol nonterminal = ruleset.first ; // current non-terminal in examination 
 		Rules rules = ruleset.second ; // get its rules 
 		// Look At All the rules and check for the properties ..
 		for ( int i = 0 ; i < rules.size() ; i ++ ) {
 			for ( int j = 0 ; j <  i ; j ++  ) {
 				// For each pair of rules ...
 				// Check Condition 1
 				// since null is also stored as element in first set so this takes care of second condition as well
 				// Also Check Condition 2
 				Symbols firstset1 = getfirst(first,rules[i]); // get the first of both sets 
 				Symbols firstset2 = getfirst(first,rules[j]);
 				Symbols commonsymbols ; // this store common elements ..
 				for ( Symbol symbol1 : firstset1 ) 
 					if ( firstset2.count(symbol1) > 0 ) 
 						// if symbol from firstset of rule 1 is in firstset of rule two ... NOT LL1
 						commonsymbols.insert(symbol1);// store common elements in first of sets
 
 				if ( commonsymbols.size() > 0 ) {
 					isLL1 = false ; // if common elements not LL1
 					if ( commonsymbols.count("null") > 0)  // if common elements contain null 
 						cout << "NOTE: Pair of rules deriving 'null'" << endl, commonsymbols.erase("null"); // erase null
 					if ( commonsymbols.size() > 0 ) // check for remaining common elements
 						cout << "NOTE: Common Symbols in First Set :" , printsymbols(commonsymbols);
 					printrule(nonterminal,rules[i]); // print those rules
 					printrule(nonterminal,rules[j]);
 				}
 				// check for condition 3
 				commonsymbols.clear();  // clear them for further check
 				if ( firstset1.count("null") > 0 ) { // if first set 1 contains null
 					for ( Symbol symbol2 : firstset2 )  // check for common in firstset2 and follow set 
 						if ( follow[nonterminal].count(symbol2) > 0 )
 							commonsymbols.insert(symbol2); // insert the common element
 					if ( commonsymbols.size() > 0 ) {
 						isLL1 = false ; // Not LL1
 						cout << "NOTE: Common Elements with follow " ; printsymbols(commonsymbols);
 						cout << "First contains null: " ; printrule(nonterminal,rules[i]);
 						printrule(nonterminal,rules[j]);
 						cout << "FOLLOW set of " << nonterminal << " : " ; printsymbols(follow[nonterminal]);
 					}
 				}
 				commonsymbols.clear();
 				if ( firstset2.count("null") > 0 ) { // if first set 2 contains null
 					for ( Symbol symbol1 : firstset1 )  // check for common in firstset1 and follow set 
 						if ( follow[nonterminal].count(symbol1) > 0 )
 							commonsymbols.insert(symbol1);
 					if ( commonsymbols.size() > 0 ) {
 						isLL1 = false ; // COndition 3 not satisfied 
 						// just print why not LL1
 						cout << "NOTE: Common Elements with follow " ; printsymbols(commonsymbols);
 						cout << "First contains null: " ; printrule(nonterminal,rules[j]);
 						printrule(nonterminal,rules[i]);
 						cout << "FOLLOW set of " << nonterminal << " : " ; printsymbols(follow[nonterminal]);
 					}
 				}
 			}
 		}
 	}
 	return isLL1 ;
 }