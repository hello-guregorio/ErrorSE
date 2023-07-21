CC=g++
CFLAGS=-Wall
LINK=-lpthread
SRC=test.cc $(wildcard ../IndexModule/*.cc)
OBJ=$(patsubst %.cc, %.o, $(SRC))

INC=-I../ThirdParty/ \
	-I../ThirdParty/cppjieba \
	-I../ThirdParty/limonp


%.o:%.cc
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LINK)

test:$(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LINK)

clean:
	rm test