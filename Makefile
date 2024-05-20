.PHONY = all clean

CXX = g++
CXX_FLAGS = -Wall -Werror

SRCS := $(wildcard *.cpp)
BINS := $(SRCS:%.cpp=%)

all: ${BINS}

%: %.o
	@echo "Checking.."
	${CXX} $< -o $@

%.o: %.c
	@echo "Creating object.."
	${CXX} -c $<

clean:
	@echo "Cleaning up..."
	rm -rvf *.o ${BINS}
