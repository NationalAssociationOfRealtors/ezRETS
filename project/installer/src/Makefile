LIB_FLAGS = -liodbcinst -lboost_program_options
CFLAGS = -g -D_MAC_ -DHAVE_IODBC -I../../common/include -Wall -I/opt/include -I/Users/kgarner/src/odbcrets/universals/boost_1_33_1

all: RegisterDriver

RegisterDriver: RegisterDriver.cpp
	g++ $(CFLAGS) -o RegisterDriver RegisterDriver.cpp $(LIB_FLAGS)

clean:
	rm RegisterDriver
