#include "simplifygrammar.h"

int main(int argc,char * argv[]) {
	if ( argc == 2 ) {	
		initgrammar(argv[1]); 
		// printgrammar();
		 //removenullproductions();
		 //printgrammar();
		 // removeunitproductions();
		 // printgrammar();
		 // removenonderivingsymbols();
		 // printgrammar();
		 // removenonreachablesymbols() ;
		 // printgrammar();
		removeleftrecursion();
		printgrammar();
		leftfactor();
		printgrammar();
		 printffset(getfirst());
		 printfirstofrules() ;
		 //printgrammar();
	  printffset(getfollow());
	  if (checkforLL1Grammar() ) cout << endl << "Is An LL1 Grammar" << endl << endl  ;
	  else cout << endl << "is NOT an LL1 Grammar." << endl << endl ;
	}
	else 
		cout << "Enter Grammar File.\n" ;
	return 0;
}