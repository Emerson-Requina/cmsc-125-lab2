#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

LOG_DIR="logs"
mkdir -p $LOG_DIR

# 1. Pre-flight check: Is Valgrind installed?
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}Error: valgrind is not installed.${NC}"
    exit 1
fi

echo -e "${GREEN}Building project...${NC}"
make clean && make > /dev/null

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed.${NC}"
    exit 1
fi

INPUT_FILE="workload.txt"
MLFQ_CONF="mlfq.conf"
ALGOS=("FCFS" "SJF" "STCF" "RR" "MLFQ")

echo -e "${YELLOW}Starting Memory Leak Tracking...${NC}"
echo "------------------------------------------------"

run_valgrind() {
    local mode_name=$1
    local cmd_args=$2
    local log_file="$LOG_DIR/valgrind_${mode_name}.log"

    echo -en "Testing: ${YELLOW}$mode_name${NC} ... "

    # Execute valgrind and redirect its report to a log file
    # We use --log-file to keep the simulation output separate from the leak report
    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --error-exitcode=1 \
             --log-file="$log_file" \
             $cmd_args > /dev/null 2>&1

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}PASSED${NC}"
        rm "$log_file" # Clean up logs for passing tests
    else
        echo -e "${RED}FAILED${NC}"
        # Extract the summary line from the log to show the user immediately
        LEAK_SUMMARY=$(grep "definitely lost:" "$log_file" | sed 's/==[0-9]*== //')
        echo -e "  ${CYAN}Summary:${NC} $LEAK_SUMMARY"
        echo -e "  ${CYAN}Details:${NC} See $log_file"
    fi
}

# 2. Test each algorithm
for algo in "${ALGOS[@]}"; do
    run_valgrind "$algo" "./schedsim -i $INPUT_FILE -a $algo -m $MLFQ_CONF -q 10"
done

# 3. Test Comparative Analysis Mode
run_valgrind "COMPARE" "./schedsim -i $INPUT_FILE --compare -m $MLFQ_CONF"

# 4. Test "Invalid Config" Scenario (This is where your specific leak was)
# We temporarily rename the config to force a load failure
mv "$MLFQ_CONF" "${MLFQ_CONF}.tmp"
run_valgrind "INVALID_CONFIG" "./schedsim -i $INPUT_FILE -a MLFQ -m $MLFQ_CONF"
mv "${MLFQ_CONF}.tmp" "$MLFQ_CONF"

echo "------------------------------------------------"
echo -e "${GREEN}Memory Testing Complete.${NC}"