all: Regex_Matcher

clean:
	rm *.o ./Regex_Matcher
Compact_DFA.o: Compact_DFA.h Compact_DFA.cpp fa_transition.h
	clang++ -c Compact_DFA.cpp

DFA_Table.o: DFA_Table.h DFA_Table.cpp NFA.h Compact_DFA.h
	clang++ -c DFA_Table.cpp

NFA.o: NFA.h NFA.cpp fa_transition.h
	clang++ -c NFA.cpp

Regex_Parser.o: Regex_Parser.h Regex_Parser.cpp
	clang++ -c Regex_Parser.cpp

Regex_Matcher.o: Compact_DFA.h NFA.h Regex_Parser.h DFA_Table.h Regex_Matcher.cpp
	clang++ -c Regex_Matcher.cpp

Regex_Matcher: NFA.o Regex_Parser.o Regex_Matcher.o DFA_Table.o Compact_DFA.o
	clang++ -o Regex_Matcher NFA.o Regex_Parser.o Regex_Matcher.o DFA_Table.o Compact_DFA.o
