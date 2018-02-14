#include "simplifygrammar.h"

void printffset(FFSet ffset){
	// get max size 
	int maxsize1 = 0 ; // stores max size of symbol
	int maxsize2 = 0 ; // stores max count of symbols
	for(pair<const Symbol,Symbols> &ffelem : ffset ){
		Symbol symbol = ffelem.first ;
		Symbols &symbols = ffelem.second ;
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

Symbols getfirst( FFSet first, Rule rule )  { // gets the first of vector of symbol
	Symbols firstset ;
	
	if ( rule.size() == 0) {// default case ..
		firstset.insert("null") ; // if 'null' production
		return firstset ;
	}

	bool nullpresent = false ;
	for (  int i = 0 ; i < rule.size() ; i ++ ) {
		Symbol symbol = rule[i] ;
		// for each symbol ...
		for ( Symbol tmpsymbol : first[symbol] ) 
			if ( tmpsymbol == "null" )
				nullpresent = true ;
			else 
				firstset.insert(tmpsymbol);
		
		if ( nullpresent ) { // if current symbol derives null
			nullpresent = false ;
			if ( i == (rule.size() - 1) ) // if last element
				firstset.insert("null");
			continue ; // look at next symbol ...
		}
		break ; // if null not present ...
	}
	return firstset ;

}

void printfirstofrules() {
	FFSet first = getfirst();
	for( RuleSet ruleset : grammar ) {
		Symbol nonterminal = ruleset.first ;
		Rules rules = ruleset.second ;
		for (Rule rule : rules ) {
			Symbols firstset = getfirst(first,rule);
			cout << nonterminal << " -> " ;
			if (rule.size() == 0) cout << " --null-- " ;
			for ( Symbol symbol : rule) 
				cout << " " << symbol << " " ;
			cout << "==>" ; printsymbols(firstset);
		}
	}
}

FFSet getfirst() {
	FFSet first;
	Symbols nonterminals,terminals,derivingsymbols;
	getsymboldescription(nonterminals,terminals,derivingsymbols);
	// Initialize it with terminals
	for ( Symbol terminal :terminals )
		first[terminal].insert(terminal);

	FFSet newfirst = first ;
	do {
		first = newfirst ;
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
						Symbol symbol = rule[i];
						bool nullpresent = false ;
						for (Symbol symboltmp : newfirst[symbol] )
							if ( symboltmp == "null" ) // if null value
								nullpresent = true ;
							else // if not null
								newfirst[nonterminal].insert(symboltmp);
						
						if ( nullpresent ) 
							if ( i == ( rule.size() -1 ) ) // if last symbol add null 
								newfirst[nonterminal].insert("null");
							else 
								continue ; // if not last null
						else // if null absent
							break ; // no need to look forward
					}
				}
			}
		}
	} while ( first != newfirst ) ;
	return first ;
}
FFSet getfollow() {
	FFSet follow ;
	// initialize the follow set 
	follow[startsymbol].insert("$") ;
	FFSet first = getfirst();
	FFSet newfollow = follow ;
	do {
		follow = newfollow ;
		for ( RuleSet ruleset : grammar ) {
			Symbol nonterminal = ruleset.first ;
			Rules rules = ruleset.second ;
			for ( Rule rule : rules ) // for each rule
				for (int i = 0 ; i < rule.size() ; i ++) {
					if ( ! isterminal(rule[i]) ) {
						bool nullpresent = false ;
						// if non-terminal 
						if ( i == (rule.size() -1 ) ) nullpresent = true ;
						else {
							Rule following ( rule.begin() + i + 1, rule.end() ) ; // symbols after current non-terminal
							Symbols firstset = getfirst(first,following);
							for ( Symbol symbol : firstset ) 
								if ( symbol == "null" ) nullpresent = true ;
								else newfollow[rule[i]].insert(symbol);
						}
						if ( nullpresent ) // if null present ... Add the follow of deriving non-terminal
							for ( Symbol symbol : newfollow[nonterminal] ) 
								newfollow[rule[i]].insert(symbol);
						
					}
				}
		}
	} while ( follow != newfollow );
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
 						isLL1 = false ;
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