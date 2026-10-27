# Build and run the characterization tests: `make` runs all three.
CXX      ?= g++
CXXFLAGS ?= -std=c++17
LIBS      = -lgtest -lgtest_main -pthread

TESTS = first_try_test update_test agent_tests

.PHONY: all clean $(addprefix run-,$(TESTS))

all: $(addprefix run-,$(TESTS))

%: %.cpp ../snake.cpp
	$(CXX) $(CXXFLAGS) $< $(LIBS) -o $@

# first_try_test is meant to fail: its failure message is the lesson
run-first_try_test: first_try_test
	-./first_try_test

run-update_test: update_test
	./update_test

run-agent_tests: agent_tests
	./agent_tests

clean:
	rm -f $(TESTS)
