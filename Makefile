# Main compiler 
CC := g++ -std=c++11

SRCDIR := src
BUILDDIR := build
CLIENTEXE := bin/sydClient.exe
SERVEREXE := bin/sydServer.exe
 
CFLAGS := -g -Wall
#LIB := -pthread
INC := -I include

all: $(CLIENTEXE) $(SERVEREXE)
	@echo " ";
	@echo " Done!"

$(CLIENTEXE): $(BUILDDIR)/sydClient.o $(BUILDDIR)/Socket.o $(BUILDDIR)/MessageHeader.o $(BUILDDIR)/sydUtil.o $(BUILDDIR)/File.o
	@echo " ";
	@echo " Link client:";
	$(CC) $^ -o $(CLIENTEXE)

$(SERVEREXE): $(BUILDDIR)/sydServer.o $(BUILDDIR)/Socket.o $(BUILDDIR)/MessageHeader.o $(BUILDDIR)/sydUtil.o
	@echo " ";
	@echo " Link server:";
	$(CC) $^ -o $(SERVEREXE)

$(BUILDDIR)/sydClient.o: $(SRCDIR)/sydClient.cpp
	@echo " ";
	@echo " Compile client:";
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/sydServer.o: $(SRCDIR)/sydServer.cpp
	@echo " ";
	@echo " Compile server:";
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/sydUtil.o: $(SRCDIR)/sydUtil.cpp
	@echo " ";
	@echo " Compile util:";
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/Socket.o: $(SRCDIR)/Socket.cpp
	@echo " ";
	@echo " Compile Socket:";
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/MessageHeader.o: $(SRCDIR)/MessageHeader.cpp
	@echo " ";
	@echo " Compile MessageHeader:";
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/File.o: $(SRCDIR)/File.cpp
	@echo " ";
	@echo " Compile File:";
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Tests
#tester:
#	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester.exe

clean:
	@echo " Cleaning...";
	$(RM) $(BUILDDIR)/*.o $(CLIENTEXE) $(SERVEREXE);
	@echo " Cleaned!"
