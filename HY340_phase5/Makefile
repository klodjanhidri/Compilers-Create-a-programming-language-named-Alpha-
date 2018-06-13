CC = g++-5
OBJF = def.o quads.o manage.o generate.o parser.o lexical_analysis.o 
AVM_OBJF =def.o quads.o generate.o Alpha_Virtual_Machine.o
DFLAGS = -g
EXEC = alphac
AVM = alpha

all: $(EXEC) $(AVM)

$(EXEC): $(OBJF)
	$(CC) $(DFLAGS) -o $@ $^
	@echo " ";
	@echo "_________________________________AVM_________________________________";


$(AVM): $(AVM_OBJF)
	$(CC) $(DFLAGS) -o $@ $^
parser.o: parser.y
	bison -v --yacc --defines --output=parser.cpp parser.y
	$(CC) -c parser.cpp

scanner.o: scanner.l
	flex --outfile=scanner.cpp scanner.l
	$(CC) -c scanner.cpp

def.o: def.cpp def.hpp
	$(CC) -c def.cpp

quads.o: quads.cpp quads.hpp def.hpp
	$(CC) -c quads.cpp
manage.o: manage.cpp manage.hpp quads.hpp def.hpp
	$(CC) -c manage.cpp
generate.o: generate.cpp generate.hpp quads.hpp def.hpp
	$(CC) -c generate.cpp

Alpha_Virtual_Machine.o: Alpha_Virtual_Machine.cpp Alpha_Virtual_Machine.hpp def.hpp quads.hpp generate.hpp
	$(CC) -c Alpha_Virtual_Machine.cpp 
clean:
	rm -f alphac $(OBJF) scanner.cpp parser.cpp parser.hpp *.output readable.txt *.abc
	rm -f alpha $(AVM_OBJF) loaded.txt
