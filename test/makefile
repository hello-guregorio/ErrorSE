CC=g++
CFLAGS=-Wall
LINK=-lpthread -lhiredis
SRC=test.cc $(wildcard ../IndexModule/*.cc) $(wildcard ../Server/*.cc) $(wildcard ../Server/Net/*.cc) \
	$(wildcard ../QueryModule/*.cc)
OBJ=$(patsubst %.cc, %.o, $(SRC))

INC=-I../ThirdParty/ \
	-I../ThirdParty/cppjieba \
	-I../ThirdParty/limonp \
	-I../Server \
	-I../Server/Net \
	-I../QueryModule


%.o:%.cc
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LINK)

test:$(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LINK)

clean:
	rm test