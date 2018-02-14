main: simplifygrammar main.o initgrammar.o grammarpreprocess.o grammerpostprocessing.o firstfollow.o 
	g++ -o main main.o initgrammar.o grammarpreprocess.o grammerpostprocessing.o firstfollow.o --std=c++11

simplifygrammar: simplifygrammar.h
	g++ simplifygrammar.h

main.o: main.cpp 
	g++ -c main.cpp simplifygrammar.h --std=c++11

initgrammar.o: initgrammar.cpp
	g++ -c initgrammar.cpp simplifygrammar.h --std=c++11

grammarpreprocess.o: grammarpreprocess.cpp
	g++ -c grammarpreprocess.cpp simplifygrammar.h --std=c++11

grammerpostprocessing.o: grammerpostprocessing.cpp
	g++ -c grammerpostprocessing.cpp simplifygrammar.h --std=c++11	

firstfollow.o: firstfollow.cpp
	g++ -c firstfollow.cpp simplifygrammar.h --std=c++11
clean:
	rm main *.o *.gch