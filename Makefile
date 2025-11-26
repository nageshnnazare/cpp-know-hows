# Makefile for Design Patterns in C++
# Compiles and tests all 23 Gang of Four patterns

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Directories
CREATIONAL_DIR = creational
STRUCTURAL_DIR = structural
BEHAVIORAL_DIR = behavioral
ADVANCED_DIR = advanced_patterns
OPTIMIZATIONS_DIR = compiler_optimizations

# Source files
CREATIONAL_SRCS = $(wildcard $(CREATIONAL_DIR)/*.cpp)
STRUCTURAL_SRCS = $(wildcard $(STRUCTURAL_DIR)/*.cpp)
BEHAVIORAL_SRCS = $(wildcard $(BEHAVIORAL_DIR)/*.cpp)
ADVANCED_SRCS = $(wildcard $(ADVANCED_DIR)/*.cpp)
OPTIMIZATIONS_SRCS = $(wildcard $(OPTIMIZATIONS_DIR)/*.cpp)

# Executables
CREATIONAL_BINS = $(CREATIONAL_SRCS:.cpp=)
STRUCTURAL_BINS = $(STRUCTURAL_SRCS:.cpp=)
BEHAVIORAL_BINS = $(BEHAVIORAL_SRCS:.cpp=)
ADVANCED_BINS = $(ADVANCED_SRCS:.cpp=)
OPTIMIZATIONS_BINS = $(OPTIMIZATIONS_SRCS:.cpp=)

ALL_BINS = $(CREATIONAL_BINS) $(STRUCTURAL_BINS) $(BEHAVIORAL_BINS) $(ADVANCED_BINS) $(OPTIMIZATIONS_BINS)

# Colors for output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

.PHONY: all clean test test-creational test-structural test-behavioral test-advanced test-optimizations help

# Default target
all: $(ALL_BINS)
	@echo "$(GREEN)✓ All patterns compiled successfully!$(NC)"

# Compile individual categories
creational: $(CREATIONAL_BINS)
	@echo "$(GREEN)✓ Creational patterns compiled$(NC)"

structural: $(STRUCTURAL_BINS)
	@echo "$(GREEN)✓ Structural patterns compiled$(NC)"

behavioral: $(BEHAVIORAL_BINS)
	@echo "$(GREEN)✓ Behavioral patterns compiled$(NC)"

advanced: $(ADVANCED_BINS)
	@echo "$(GREEN)✓ Advanced patterns compiled$(NC)"

optimizations: $(OPTIMIZATIONS_BINS)
	@echo "$(GREEN)✓ Compiler optimizations compiled$(NC)"

# Generic rule for compiling
%: %.cpp
	@echo "$(BLUE)Compiling $<...$(NC)"
	@$(CXX) $(CXXFLAGS) -o $@ $<
	@echo "$(GREEN)✓ $@ compiled$(NC)"

# Test all patterns
test: all
	@echo ""
	@echo "$(YELLOW)======================================$(NC)"
	@echo "$(YELLOW)   TESTING ALL PATTERNS$(NC)"
	@echo "$(YELLOW)======================================$(NC)"
	@$(MAKE) test-creational
	@$(MAKE) test-structural
	@$(MAKE) test-behavioral
	@$(MAKE) test-advanced
	@$(MAKE) test-optimizations
	@echo ""
	@echo "$(GREEN)======================================$(NC)"
	@echo "$(GREEN)   ALL TESTS COMPLETED$(NC)"
	@echo "$(GREEN)======================================$(NC)"

# Test creational patterns
test-creational:
	@echo ""
	@echo "$(BLUE)Testing Creational Patterns...$(NC)"
	@echo "-----------------------------------"
	@for bin in $(CREATIONAL_BINS); do \
		echo "$(YELLOW)Running $$bin...$(NC)"; \
		if timeout 5 ./$$bin > /dev/null 2>&1; then \
			echo "$(GREEN)✓ $$bin passed$(NC)"; \
		else \
			echo "$(RED)✗ $$bin failed$(NC)"; \
		fi; \
		echo ""; \
	done

# Test structural patterns
test-structural:
	@echo ""
	@echo "$(BLUE)Testing Structural Patterns...$(NC)"
	@echo "-----------------------------------"
	@for bin in $(STRUCTURAL_BINS); do \
		echo "$(YELLOW)Running $$bin...$(NC)"; \
		if timeout 5 ./$$bin > /dev/null 2>&1; then \
			echo "$(GREEN)✓ $$bin passed$(NC)"; \
		else \
			echo "$(RED)✗ $$bin failed$(NC)"; \
		fi; \
		echo ""; \
	done

# Test behavioral patterns
test-behavioral:
	@echo ""
	@echo "$(BLUE)Testing Behavioral Patterns...$(NC)"
	@echo "-----------------------------------"
	@for bin in $(BEHAVIORAL_BINS); do \
		echo "$(YELLOW)Running $$bin...$(NC)"; \
		if timeout 5 ./$$bin > /dev/null 2>&1; then \
			echo "$(GREEN)✓ $$bin passed$(NC)"; \
		else \
			echo "$(RED)✗ $$bin failed$(NC)"; \
		fi; \
		echo ""; \
	done

# Test advanced patterns
test-advanced:
	@echo ""
	@echo "$(BLUE)Testing Advanced Patterns...$(NC)"
	@echo "-----------------------------------"
	@for bin in $(ADVANCED_BINS); do \
		echo "$(YELLOW)Running $$bin...$(NC)"; \
		if timeout 5 ./$$bin > /dev/null 2>&1; then \
			echo "$(GREEN)✓ $$bin passed$(NC)"; \
		else \
			echo "$(RED)✗ $$bin failed$(NC)"; \
		fi; \
		echo ""; \
	done

# Test compiler optimizations
test-optimizations:
	@echo ""
	@echo "$(BLUE)Testing Compiler Optimizations...$(NC)"
	@echo "-----------------------------------"
	@for bin in $(OPTIMIZATIONS_BINS); do \
		echo "$(YELLOW)Running $$bin...$(NC)"; \
		if timeout 5 ./$$bin > /dev/null 2>&1; then \
			echo "$(GREEN)✓ $$bin passed$(NC)"; \
		else \
			echo "$(RED)✗ $$bin failed$(NC)"; \
		fi; \
		echo ""; \
	done

# Run a specific pattern and show output
run-%:
	@if [ -f $(CREATIONAL_DIR)/$* ]; then \
		./$(CREATIONAL_DIR)/$*; \
	elif [ -f $(STRUCTURAL_DIR)/$* ]; then \
		./$(STRUCTURAL_DIR)/$*; \
	elif [ -f $(BEHAVIORAL_DIR)/$* ]; then \
		./$(BEHAVIORAL_DIR)/$*; \
	else \
		echo "$(RED)Pattern $* not found$(NC)"; \
	fi

# Compile and run specific pattern
demo-%: clean-pattern-%
	@if [ -f $(CREATIONAL_DIR)/$*.cpp ]; then \
		$(MAKE) $(CREATIONAL_DIR)/$* && ./$(CREATIONAL_DIR)/$*; \
	elif [ -f $(STRUCTURAL_DIR)/$*.cpp ]; then \
		$(MAKE) $(STRUCTURAL_DIR)/$* && ./$(STRUCTURAL_DIR)/$*; \
	elif [ -f $(BEHAVIORAL_DIR)/$*.cpp ]; then \
		$(MAKE) $(BEHAVIORAL_DIR)/$* && ./$(BEHAVIORAL_DIR)/$*; \
	else \
		echo "$(RED)Pattern $*.cpp not found$(NC)"; \
	fi

# Clean specific pattern binary
clean-pattern-%:
	@rm -f $(CREATIONAL_DIR)/$* $(STRUCTURAL_DIR)/$* $(BEHAVIORAL_DIR)/$*

# Clean all compiled binaries
clean:
	@echo "$(YELLOW)Cleaning all binaries...$(NC)"
	@rm -f $(ALL_BINS)
	@echo "$(GREEN)✓ Clean complete$(NC)"

# Verbose test (shows output)
test-verbose: all
	@echo ""
	@echo "$(YELLOW)======================================$(NC)"
	@echo "$(YELLOW)   VERBOSE TESTING$(NC)"
	@echo "$(YELLOW)======================================$(NC)"
	@for bin in $(ALL_BINS); do \
		echo ""; \
		echo "$(BLUE)========================================$(NC)"; \
		echo "$(BLUE)Running $$bin$(NC)"; \
		echo "$(BLUE)========================================$(NC)"; \
		if timeout 5 ./$$bin; then \
			echo "$(GREEN)✓ $$bin completed$(NC)"; \
		else \
			echo "$(RED)✗ $$bin failed or timed out$(NC)"; \
		fi; \
	done

# Count lines of code
stats:
	@echo "$(BLUE)Design Patterns Statistics:$(NC)"
	@echo "-----------------------------------"
	@echo "Creational patterns: $(words $(CREATIONAL_SRCS))"
	@echo "Structural patterns: $(words $(STRUCTURAL_SRCS))"
	@echo "Behavioral patterns: $(words $(BEHAVIORAL_SRCS))"
	@echo "Total patterns: $(words $(CREATIONAL_SRCS) $(STRUCTURAL_SRCS) $(BEHAVIORAL_SRCS))"
	@echo ""
	@echo "Lines of code:"
	@wc -l $(CREATIONAL_SRCS) $(STRUCTURAL_SRCS) $(BEHAVIORAL_SRCS) | tail -1

# Help message
help:
	@echo "$(BLUE)Design Patterns Makefile$(NC)"
	@echo "======================="
	@echo ""
	@echo "$(YELLOW)Targets:$(NC)"
	@echo "  make              - Compile all patterns"
	@echo "  make creational   - Compile only creational patterns"
	@echo "  make structural   - Compile only structural patterns"
	@echo "  make behavioral   - Compile only behavioral patterns"
	@echo "  make test         - Compile and test all patterns"
	@echo "  make test-verbose - Test with full output"
	@echo "  make demo-PATTERN - Compile and run specific pattern"
	@echo "  make run-PATTERN  - Run already compiled pattern"
	@echo "  make clean        - Remove all binaries"
	@echo "  make stats        - Show code statistics"
	@echo "  make help         - Show this help"
	@echo ""
	@echo "$(YELLOW)Examples:$(NC)"
	@echo "  make demo-01_singleton"
	@echo "  make demo-04_decorator"
	@echo "  make run-06_observer"
	@echo ""
	@echo "$(YELLOW)Pattern Names:$(NC)"
	@echo "  Creational: 01_singleton, 02_factory_method, 03_abstract_factory,"
	@echo "              04_builder, 05_prototype"
	@echo "  Structural: 01_adapter, 02_bridge, 03_composite, 04_decorator,"
	@echo "              05_facade, 06_flyweight, 07_proxy"
	@echo "  Behavioral: 01_chain_of_responsibility, 02_command, 03_iterator,"
	@echo "              04_mediator, 05_memento, 06_observer, 07_state,"
	@echo "              08_strategy, 09_template_method, 10_visitor, 11_interpreter"

