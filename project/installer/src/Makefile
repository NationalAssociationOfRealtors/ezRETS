LIB_FLAGS = -liodbcinst
CFLAGS = -g -DHAVE_IODBC -I../../common/include -Wall -I/opt/include -I/Users/kgarner/src/odbcrets/universals/boost_1_33_1

all: TestReregister RegisterDriver

TestReregister: TestReregister.cpp
	g++ $(CFLAGS) -o TestReregister TestReregister.cpp $(LIB_FLAGS)

RegisterDriver: RegisterDriver.cpp
	g++ $(CFLAGS) -o RegisterDriver RegisterDriver.cpp $(LIB_FLAGS)

clean:
	rm TestReregister RegisterDriver
