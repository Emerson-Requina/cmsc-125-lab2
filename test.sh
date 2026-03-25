#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 1. Pre-flight check: Is Valgrind installed?
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}Error: valgrind is not installed.${NC}"
    echo "Install it with: sudo apt install valgrind"
    exit 1
fi

echo -e "${GREEN}Building project...${NC}"
make clean && make

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed. Check your code for syntax errors.${NC}"
    exit 1
fi

# Define the test files
INPUT_FILE="workload.txt"
MLFQ_CONF="mlfq.conf"

# List of algorithms to test in single mode
ALGOS=("FCFS" "SJF" "STCF" "RR" "MLFQ")

echo -e "${YELLOW}Starting Memory Leak Tests...${NC}"
echo "------------------------------------------------"

# 2. Test each algorithm in Single Mode
for algo in "${ALGOS[@]}"; do
    echo -en "Testing Algorithm: ${YELLOW}$algo${NC} ... "
    
    # Run valgrind
    # --error-exitcode=1 makes the command return 1 if a leak is found
    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --error-exitcode=1 \
             ./schedsim -i "$INPUT_FILE" -a "$algo" -m "$MLFQ_CONF" -q 10 > /dev/null 2>&1

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}PASSED${NC}"
    else
        echo -e "${RED}FAILED (Leaks detected)${NC}"
        # Optional: remove the '> /dev/null 2>&1' above to see the error details
    fi
done

echo "------------------------------------------------"

# 3. Test Comparative Analysis Mode
echo -en "Testing ${YELLOW}COMPARATIVE ANALYSIS${NC} mode ... "
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --error-exitcode=1 \
         ./schedsim -i "$INPUT_FILE" --compare -m "$MLFQ_CONF" > /dev/null 2>&1

if [ $? -eq 0 ]; then
    echo -e "${GREEN}PASSED${NC}"
else
    echo -e "${RED}FAILED (Leaks detected)${NC}"
fi

echo "------------------------------------------------"
echo -e "${GREEN}Memory Testing Complete.${NC}"