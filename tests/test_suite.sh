#!/bin/bash

# --- Configuration ---
TARGET="./schedsim"
INPUT_FILE="workload.txt"
MLFQ_CONF="mlfq.conf"
LOG_DIR="test_logs"
mkdir -p "$LOG_DIR"

# Colors for UI
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}=== CPU Scheduler Simulation Test Suite ===${NC}"

# 1. Build Check
echo -en "Checking build... "
make rebuild > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}SUCCESS${NC}"
else
    echo -e "${RED}BUILD FAILED${NC}"
    exit 1
fi

# Function to run a test case
# Usage: run_test <test_name> <args>
run_test() {
    local name=$1
    local args=$2
    local log="$LOG_DIR/${name}.log"
    local valgrind_log="$LOG_DIR/valgrind_${name}.log"

    echo -en "Testing ${YELLOW}$name${NC} ... "

    # Run Valgrind + Simulation
    valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
             --log-file="$valgrind_log" \
             $TARGET $args > "$log" 2>&1
    
    local status=$?

    if [ $status -eq 0 ]; then
        echo -e "${GREEN}PASSED${NC} (Memory Clean)"
    elif [ $status -eq 1 ]; then
        echo -e "${RED}FAILED (Memory Leak/Error)${NC}"
        echo -e "  Check: $valgrind_log"
    else
        echo -e "${RED}FAILED (Program Crashed/Exit $status)${NC}"
        echo -e "  Check: $log"
    fi
}

# --- 2. Functional Algorithm Tests ---
echo -e "\n${CYAN}--- Functional Tests ---${NC}"
run_test "FCFS" "-i $INPUT_FILE -a FCFS"
run_test "SJF"  "-i $INPUT_FILE -a SJF"
run_test "STCF" "-i $INPUT_FILE -a STCF"
run_test "RR"   "-i $INPUT_FILE -a RR -q 5"
run_test "MLFQ" "-i $INPUT_FILE -a MLFQ -m $MLFQ_CONF"

# --- 3. Mode Tests ---
echo -e "\n${CYAN}--- Mode Tests ---${NC}"
run_test "Compare_Mode" "-i $INPUT_FILE --compare -m $MLFQ_CONF"

# --- 4. Edge Case / Robustness Tests ---
echo -e "\n${CYAN}--- Robustness Tests ---${NC}"

# Test 4a: Missing Input File
run_test "Missing_Input" "-i non_existent.txt -a FCFS"

# Test 4b: Invalid MLFQ Config (Wrong Path)
run_test "Invalid_MLFQ_Path" "-i $INPUT_FILE -a MLFQ -m missing.conf"

# Test 4c: Logic Failure (Invalid Allotment in Config)
echo "Q0 10 5" > broken_mlfq.conf
echo "BOOST_PERIOD 100" >> broken_mlfq.conf
run_test "Logic_Failure_MLFQ" "-i $INPUT_FILE -a MLFQ -m broken_mlfq.conf"
rm broken_mlfq.conf

echo -e "\n${CYAN}=== Testing Complete ===${NC}"
echo -e "All logs saved in ${CYAN}$LOG_DIR/${NC}"