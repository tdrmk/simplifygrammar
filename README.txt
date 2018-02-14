This project is to help remove anomalities from grammar or/and simplify context free grammar.
Theorem 1: For every context free grammar G. there exist a CFG G1 such that each non-terminal in G1 derives some terminal string. 
Theorem 2: Given a CFG G, there exist CFG G1 such that every symbol in G1 appears in sentential form.
We need to remove non-terminals that cannot be reached from start symbol and we need to remove rules that don't derive symbols.
Thus simplifing the CFG without change in language. To achieve these following functions provided.
	void removenonderivingsymbols(); 
	void removenonreachablesymbols();

Unit productions also are not needed. They can result in cycles and not desired as it makes further processing difficult.
Unit productions can be eliminated without changing the language. For the same following function is provided.
	void removeunitproductions();

Also Null productions can cause lots of problem while processing. But it is easier to write grammar with null productions.
Thus following is provided to reduce to grammar to one without null productions. L(G1) = L(G) - {null}
	void removenullproductions();

The above functions can be used individually and wherever needed.
Recommended order of using them is
	1. Eliminate NULL productions
	2. Eliminate UNIT productions
	3. Eliminate Non-deriving symbols
	4. Eliminate Symbols not appearing in Sentential form 

Once all these anomalies are removed, grammar are simplified. To convert them to LL1 grammar if possible we need to eliminate left recursion and we nede to left factor the grammar.
	void removeleftrecursion(); 
	void leftfactor();

To obtain first and follow. Following function provided.
	FFSet getfirst() ; 
	FFSet getfollow();  

To check if grammar is LL(1) or not use the following function. 
	bool checkforLL1Grammar();

Try to use the simplification functions and check if resulting grammar is LL(1) or not.
Any time grammar can be printed using the following function.
	void printgrammar(); 

More Accessary and printing functions also there. Check the code files or header from more function.

Grammar is read from file using 
	void initgrammar(string filename);

For how grammar must be represented check initgrammar.cpp
Sample grammar and test file (main.cpp) is provided.
