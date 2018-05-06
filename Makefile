# Main compiler 
CC := g++ -std=c++11

SRCDIR := src
BUILDDIR := build
CLIENTEXE := bin/sydClient.exe
SERVEREXE := bin/sydServer.exe
 
CFLAGS := -g -Wall
LIB := -pthread
INC := -I include

all: $(CLIENTEXE) $(SERVEREXE)
	@echo " ";
	@echo " Done!"

$(CLIENTEXE): $(BUILDDIR)/sydClient.o $(BUILDDIR)/Socket.o $(BUILDDIR)/Message.o $(BUILDDIR)/sydUtil.o $(BUILDDIR)/Connection.o
	@echo " ";
	@echo " Link client:";
	$(CC) $^ -o $(CLIENTEXE) $(LIB)

$(SERVEREXE): $(BUILDDIR)/sydServer.o $(BUILDDIR)/Socket.o $(BUILDDIR)/Message.o $(BUILDDIR)/sydUtil.o $(BUILDDIR)/Thread.o $(BUILDDIR)/ServerThread.o $(BUILDDIR)/Connection.o
	@echo " ";
	@echo " Link server:";
	$(CC) $^ -o $(SERVEREXE) $(LIB)

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

$(BUILDDIR)/Message.o: $(SRCDIR)/Message.cpp
	@echo " ";
	@echo " Compile Message:";
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/Thread.o: $(SRCDIR)/Thread.cpp
	@echo " ";
	@echo " Compile Thread:";
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/ServerThread.o: $(SRCDIR)/ServerThread.cpp
	@echo " ";
	@echo " Compile ServerThread:";
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/Connection.o: $(SRCDIR)/Connection.cpp
	@echo " ";
	@echo " Compile Connection:";
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Tests
#tester:
#	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester.exe

clean:
	@echo " Cleaning...";
	$(RM) $(BUILDDIR)/*.o $(CLIENTEXE) $(SERVEREXE);
	@echo " Cleaned!"

rebuild: clean all
