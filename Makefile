all: Regex_Matcher

clean:
	rm *.o ./Regex_Matcher

DFA.o: DFA.h DFA.cpp NFA.h DFA_Transition.h DFA_State.h
	clang++ -c DFA.cpp

DFA_State.o: DFA_State.h DFA_State.cpp
	clang++ -c DFA_State.cpp

NFA.o: NFA.h NFA.cpp NFA_Transition.h
	clang++ -c NFA.cpp

Regex_Parser.o: Regex_Parser.h Regex_Parser.cpp NFA.h
	clang++ -c Regex_Parser.cpp

Regex_Matcher.o: DFA.h NFA.h Regex_Parser.h Regex_Matcher.cpp
	clang++ -c Regex_Matcher.cpp

Regex_Matcher: DFA.o DFA_State.o NFA.o Regex_Parser.o Regex_Matcher.o
	clang++ -o Regex_Matcher DFA.o DFA_State.o NFA.o Regex_Parser.o Regex_Matcher.o
