LIB_FLAGS = -liodbcinst
CFLAGS = -g -DHAVE_IODBC -I../../common/include

all: TestReregister

TestReregister: TestReregister.cpp
	g++ $(CFLAGS) -o TestReregister TestReregister.cpp $(LIB_FLAGS)

clean:
	rm TestReregister
