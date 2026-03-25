#!/bin/bash

# Configuration
EXE="../schedsim"
WORKLOAD_DIR="."
EXPECTED_DIR="./expected_output"
TEMP_OUT="actual_output.tmp"

echo "Running CMSC 125 Lab 2 Test Suite..."

# File-based Regression Test
run_test() {
    local algo=$1
    local input=$2
    local extra_args=$3
    
    echo -n "Checking $algo with $input... "
    
    # 1. Ensure the workload file exists
    if [ ! -f "$WORKLOAD_DIR/$input" ]; then
        echo "MISSING WORKLOAD"
        return
    fi

    # 2. Run the scheduler
    $EXE --algorithm=$algo --input=$WORKLOAD_DIR/$input $extra_args > $TEMP_OUT 2>/dev/null
    
    # 3. If expected output doesn't exist, create it (Bootstrap mode)
    if [ ! -f "$EXPECTED_DIR/${algo}_${input}.out" ]; then
        mv $TEMP_OUT "$EXPECTED_DIR/${algo}_${input}.out"
        echo "CREATED BASELINE"
        return
    fi

    # 4. Compare with expected
    if diff -wb $TEMP_OUT $EXPECTED_DIR/${algo}_${input}.out > /dev/null; then
        echo "PASS"
    else
        echo "FAIL"
        echo "--- Difference (Expected vs Actual) ---"
        diff -u $EXPECTED_DIR/${algo}_${input}.out $TEMP_OUT | grep -E "^\+|^-" | head -n 6
    fi
}

mkdir -p $WORKLOAD_DIR
mkdir -p $EXPECTED_DIR

run_test "FCFS" "workload1.txt"
run_test "SJF"  "workload1.txt"
run_test "RR"   "workload1.txt" "--quantum=20"
run_test "STCF" "workload1.txt"

# Cleanup
echo "-----------------------------------------------"
echo "Test suite complete."
[ -f $TEMP_OUT ] && rm $TEMP_OUT