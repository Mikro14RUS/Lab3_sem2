CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2 -Iinclude -Itests/catch2
GCOVR ?= $(if $(wildcard $(HOME)/.local/bin/gcovr),$(HOME)/.local/bin/gcovr,gcovr)
CPPLINT ?= $(if $(wildcard $(HOME)/.local/bin/cpplint),$(HOME)/.local/bin/cpplint,cpplint)

# Ищем Catch2 v3 сначала в обычных префиксах, затем в локальном каталоге,
# который использовался при подготовке проекта. Если Catch2 установлен
# в другое место, путь можно передать явно: make test CATCH_DIR=/path/to/prefix
CATCH2_PREFIXES := /usr/local /usr /tmp/opencode/catch2-pkg/root/usr
CATCH2_FOUND := $(firstword $(foreach prefix,$(CATCH2_PREFIXES),$(if $(wildcard $(prefix)/include/catch2/catch_all.hpp),$(prefix),)))
CATCH_DIR ?= $(if $(CATCH2_FOUND),$(CATCH2_FOUND),/usr)
CATCH_INCLUDE ?= $(CATCH_DIR)/include
CATCH_LIBDIR ?= $(CATCH_DIR)/lib
CATCH_LIBS ?= -L$(CATCH_LIBDIR) -lCatch2Main -lCatch2

# Нельзя добавлять /usr/include через -isystem: glibc использует
# #include_next, и стандартные заголовки после этого не находятся.
CATCH_ISYSTEM := $(if $(filter /usr/include,$(abspath $(CATCH_INCLUDE))),,-isystem $(CATCH_INCLUDE))

GTEST_PREFIXES := $(CURDIR)/third_party /usr/local /usr /tmp/opencode/deps/root/usr
GTEST_FOUND := $(firstword $(foreach prefix,$(GTEST_PREFIXES),$(if $(wildcard $(prefix)/include/gtest/gtest.h),$(prefix),)))
GTEST_DIR ?= $(if $(GTEST_FOUND),$(GTEST_FOUND),/usr)

BOOST_PREFIXES := $(CURDIR)/third_party /usr/local /usr /tmp/opencode/deps/root/usr
BOOST_FOUND := $(firstword $(foreach prefix,$(BOOST_PREFIXES),$(if $(wildcard $(prefix)/include/boost/test/unit_test.hpp),$(prefix),)))
BOOST_DIR ?= $(if $(BOOST_FOUND),$(BOOST_FOUND),/usr)

MULTIARCH ?= $(shell $(CXX) -print-multiarch)
GTEST_INCLUDE ?= $(GTEST_DIR)/include
GTEST_LIBDIR ?= $(GTEST_DIR)/lib/$(MULTIARCH)
BOOST_INCLUDE ?= $(BOOST_DIR)/include
BOOST_LIBDIR ?= $(BOOST_DIR)/lib/$(MULTIARCH)

# Та же проблема с #include_next, что и у Catch2: не добавляем
# стандартный /usr/include через -isystem.
GTEST_ISYSTEM := $(if $(filter /usr/include,$(abspath $(GTEST_INCLUDE))),,-isystem $(GTEST_INCLUDE))
BOOST_ISYSTEM := $(if $(filter /usr/include,$(abspath $(BOOST_INCLUDE))),,-isystem $(BOOST_INCLUDE))

GTEST_LIBS = $(GTEST_LIBDIR)/libgtest.a $(GTEST_LIBDIR)/libgtest_main.a -pthread
BOOST_LIBS = $(BOOST_LIBDIR)/libboost_unit_test_framework.a -pthread

TEST_TARGET = tests/lab3_tests
BENCH_TARGET = bench/lab3_bench

SRC = src/Array.cpp \
      src/SinglyLinkedList.cpp \
      src/DoublyLinkedList.cpp \
      src/Stack.cpp \
      src/Queue.cpp \
      src/HashTable.cpp \
      src/FullBinaryTree.cpp \
      src/Serialization.cpp

TEST_SRC = tests/test_array.cpp \
           tests/test_lists.cpp \
           tests/test_stack_queue.cpp \
           tests/test_hash.cpp \
           tests/test_tree.cpp \
           tests/test_serialization.cpp \
           tests/test_edge_cases.cpp

COMPAT_TEST_SRC = $(TEST_SRC)
GTEST_OBJ_DIR = build-gtest/obj
BOOST_OBJ_DIR = build-boost/obj
GTEST_OBJS = $(patsubst src/%.cpp,$(GTEST_OBJ_DIR)/%.o,$(SRC))
BOOST_OBJS = $(patsubst src/%.cpp,$(BOOST_OBJ_DIR)/%.o,$(SRC))
GTEST_BINS = $(patsubst tests/%.cpp,build-gtest/%,$(COMPAT_TEST_SRC))
BOOST_BINS = $(patsubst tests/%.cpp,build-boost/%,$(COMPAT_TEST_SRC))

.PRECIOUS: $(GTEST_OBJS) $(BOOST_OBJS)

all: check-catch2 $(TEST_TARGET)

check-catch2:
	@test -f "$(CATCH_INCLUDE)/catch2/catch_all.hpp" || { \
		echo "Catch2 v3 не найден в $(CATCH_INCLUDE)."; \
		echo "Укажите путь: make test CATCH_DIR=/path/to/catch2-prefix"; \
		exit 1; \
	}

check-gtest:
	@test -f "$(GTEST_INCLUDE)/gtest/gtest.h" || { \
		echo "GoogleTest не найден в $(GTEST_INCLUDE)."; \
		echo "Укажите путь: make test_gtest GTEST_DIR=/path/to/gtest-prefix"; \
		exit 1; \
	}

check-boost:
	@test -f "$(BOOST_INCLUDE)/boost/test/unit_test.hpp" || { \
		echo "Boost.Test не найден в $(BOOST_INCLUDE)."; \
		echo "Укажите путь: make test_boost BOOST_DIR=/path/to/boost-prefix"; \
		exit 1; \
	}

$(TEST_TARGET): $(SRC) $(TEST_SRC) include/*.hpp tests/catch2/catch.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CATCH_ISYSTEM) -o $@ $(SRC) $(TEST_SRC) $(CATCH_LIBS)

$(BENCH_TARGET): $(SRC) bench/Benchmark.cpp include/*.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ bench/Benchmark.cpp $(SRC)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(GTEST_OBJ_DIR)/%.o: src/%.cpp include/*.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BOOST_OBJ_DIR)/%.o: src/%.cpp include/*.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build-gtest/%: tests/%.cpp $(GTEST_OBJS) tests/compat_gtest/catch.hpp
	@mkdir -p $(dir $@)
	$(CXX) -Itests/compat_gtest $(CXXFLAGS) $(GTEST_ISYSTEM) \
		-o $@ $< $(GTEST_OBJS) $(GTEST_LIBS)

build-boost/%: tests/%.cpp $(BOOST_OBJS) tests/compat_boost/catch.hpp
	@mkdir -p $(dir $@)
	$(CXX) -Itests/compat_boost $(CXXFLAGS) $(BOOST_ISYSTEM) \
		-o $@ $< $(BOOST_OBJS) $(BOOST_LIBS)

test_gtest: check-gtest $(GTEST_BINS)
	@set -e; for test in $(GTEST_BINS); do \
		echo "== $$test"; ./$$test; \
	done

test_boost: check-boost $(BOOST_BINS)
	@set -e; for test in $(BOOST_BINS); do \
		echo "== $$test"; ./$$test; \
	done

test_all: test test_gtest test_boost

bench: $(BENCH_TARGET)
	./$(BENCH_TARGET)

coverage: check-catch2
	rm -rf build-cov build-cov-gtest build-cov-boost coverage
	mkdir -p build-cov coverage
	$(CXX) $(CXXFLAGS) -O0 -g --coverage -fprofile-abs-path \
		$(CATCH_ISYSTEM) -o build-cov/lab3_tests \
		$(SRC) $(TEST_SRC) $(CATCH_LIBS)
	./build-cov/lab3_tests
	$(GCOVR) --root . --filter 'src/' \
		--exclude-unreachable-branches --exclude-throw-branches \
		--html-details coverage/index.html \
		--txt coverage/summary.txt --txt-metric line --txt-metric branch \
		--fail-under-line 85 --fail-under-branch 85 --fail-under-function 85
	cat coverage/summary.txt

coverage_gtest: check-gtest
	rm -rf build-cov build-cov-gtest build-cov-boost coverage-gtest
	mkdir -p build-cov-gtest/obj build-cov-gtest/tests build-cov-gtest/bin coverage-gtest
	@set -e; objs=""; \
	for source in $(SRC); do \
		obj="build-cov-gtest/obj/$${source%.cpp}.o"; \
		mkdir -p "$$(dirname "$$obj")"; \
		$(CXX) $(CXXFLAGS) -O0 -g --coverage -fprofile-abs-path -c "$$source" -o "$$obj"; \
		objs="$$objs $$obj"; \
	done; \
	for test in $(COMPAT_TEST_SRC); do \
		name=$$(basename "$$test" .cpp); \
		testobj="build-cov-gtest/tests/$$name.o"; \
		bin="build-cov-gtest/bin/$$name"; \
		$(CXX) -Itests/compat_gtest $(CXXFLAGS) -O0 -g --coverage \
			-fprofile-abs-path $(GTEST_ISYSTEM) -c "$$test" -o "$$testobj"; \
		$(CXX) --coverage -o "$$bin" "$$testobj" $$objs $(GTEST_LIBS); \
		echo "== $$bin"; ./"$$bin"; \
	done
	$(GCOVR) --root . --filter 'src/' \
		--exclude-unreachable-branches --exclude-throw-branches \
		--html-details coverage-gtest/index.html \
		--txt coverage-gtest/summary.txt --txt-metric line --txt-metric branch \
		--fail-under-line 85 --fail-under-branch 85 --fail-under-function 85
	cat coverage-gtest/summary.txt

coverage_boost: check-boost
	rm -rf build-cov build-cov-gtest build-cov-boost coverage-boost
	mkdir -p build-cov-boost/obj build-cov-boost/tests build-cov-boost/bin coverage-boost
	@set -e; objs=""; \
	for source in $(SRC); do \
		obj="build-cov-boost/obj/$${source%.cpp}.o"; \
		mkdir -p "$$(dirname "$$obj")"; \
		$(CXX) $(CXXFLAGS) -O0 -g --coverage -fprofile-abs-path -c "$$source" -o "$$obj"; \
		objs="$$objs $$obj"; \
	done; \
	for test in $(COMPAT_TEST_SRC); do \
		name=$$(basename "$$test" .cpp); \
		testobj="build-cov-boost/tests/$$name.o"; \
		bin="build-cov-boost/bin/$$name"; \
		$(CXX) -Itests/compat_boost $(CXXFLAGS) -O0 -g --coverage \
			-fprofile-abs-path $(BOOST_ISYSTEM) -c "$$test" -o "$$testobj"; \
		$(CXX) --coverage -o "$$bin" "$$testobj" $$objs $(BOOST_LIBS); \
		echo "== $$bin"; ./"$$bin"; \
	done
	$(GCOVR) --root . --filter 'src/' \
		--exclude-unreachable-branches --exclude-throw-branches \
		--html-details coverage-boost/index.html \
		--txt coverage-boost/summary.txt --txt-metric line --txt-metric branch \
		--fail-under-line 85 --fail-under-branch 85 --fail-under-function 85
	cat coverage-boost/summary.txt

lint:
	$(CXX) $(CXXFLAGS) -fsyntax-only $(SRC)
	$(CPPLINT) --quiet --extensions=hpp,cpp --recursive include src
	@echo "Lint: OK"

clean:
	rm -rf build-cov build-cov-gtest build-cov-boost build-gtest build-boost \
		coverage coverage-gtest coverage-boost \
		$(TEST_TARGET) $(BENCH_TARGET) *.gcda *.gcno *.gcov

.PHONY: all test test_gtest test_boost test_all bench coverage coverage_gtest coverage_boost lint clean check-catch2 check-gtest check-boost
