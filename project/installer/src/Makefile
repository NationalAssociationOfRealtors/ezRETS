LIB_FLAGS = -liodbcinst -lboost_program_options
CFLAGS = -g -D_MAC_ -DHAVE_IODBC -I../include -I../../common/include -Wall -I/opt/include -I/Users/kgarner/src/odbcrets/universals/boost_1_33_1

all: RegisterDriver

RegisterDriver: RegisterDriver.cpp RegisterFuncs.cpp
	g++ $(CFLAGS) -o RegisterDriver RegisterDriver.cpp RegisterFuncs.cpp $(LIB_FLAGS)

clean:
	rm RegisterDriver
