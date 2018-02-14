#include<vector>
#include<iostream>
using namespace std;
int main(){
	string x = "Hello_Hwpp";
	int found = x.find_last_of("_");
	cout << "Char:" << x[found] << endl;
	try {
	cout << x.substr(0,found) << "|" << stoi(x.substr(found+1)) << endl ;
	
	} catch (invalid_argument& ia) {
		cout << " Error";
	}
	return 0;
}
