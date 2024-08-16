FNAME = file

CPP = g++
BISON = bison
FLEX = flex
TGT = client

SCAN = $(FNAME).l
PARSE = $(FNAME).y
HEADERS = $(FNAME).hh

OBJ = scan.o parse.tab.o
CFLAGS = -g

$(TGT): $(OBJ)
	$(CPP) --std=c++17  $(OBJ) -o $(TGT) -ly 

scan.o: scan.c $(HEADERS)
	$(CPP) --std=c++17  $(CFLAGS) -c $<

parse.tab.o:parse.tab.c $(HEADERS)
	$(CPP) --std=c++17  $(CFLAGS) -c $<

%.o: %.cc $(HEADERS)
	$(CPP) --std=c++17  $(CFLAGS) -c $<

scan.c : $(SCAN) parse.tab.h
	$(FLEX) -l --yylineno -o scan.c $(SCAN)

parse.tab.c parse.tab.h : $(PARSE)
	$(BISON) -b parse -dv  $(PARSE) -Wcounterexamples

clean :
	rm -f *.o *.output
	rm -f $(TGT) 
	rm -rf .vscode/ core
	rm -rf parse.tab.c parse.tab.h scan.c 
