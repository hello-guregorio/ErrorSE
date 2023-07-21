CC=g++
CFLAGS=-Wall
LINK=-lpthread
SRC=test.cc $(wildcard ../IndexModule/*.cc) $(wildcard ../Server/*.cc) $(wildcard ../Server/Net/*.cc)
OBJ=$(patsubst %.cc, %.o, $(SRC))

INC=-I../ThirdParty/ \
	-I../ThirdParty/cppjieba \
	-I../ThirdParty/limonp \
	-I../Server \
	-I../Server/Net


%.o:%.cc
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LINK)

test:$(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LINK)

clean:
	rm test