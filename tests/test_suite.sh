#!/bin/bash

# Configuration
EXE="./schedsim"
WORKLOAD_DIR="./workloads"
EXPECTED_DIR="./expected_output"
TEMP_OUT="actual_output.tmp"

# Create a function to run a test
run_test() {
    local algo=$1
    local input=$2
    local extra_args=$3
    
    echo -n "Testing $algo with $input... "
    
    # Run the scheduler and save output
    $EXE --algorithm=$algo --input=$WORKLOAD_DIR/$input $extra_args > $TEMP_OUT
    
    # Compare with expected (ignoring whitespace differences)
    if diff -wb $TEMP_OUT $EXPECTED_DIR/${algo}_${input}.out > /dev/null; then
        echo "PASS"
    else
        echo "FAIL"
        echo "Differences found:"
        diff -u $EXPECTED_DIR/${algo}_${input}.out $TEMP_OUT | head -n 10
    fi
}

# Run the suite
run_test "FCFS" "workload1.txt"
run_test "SJF"  "workload1.txt"
run_test "RR"   "workload1.txt" "--quantum=4"
run_test "STCF" "workload1.txt"

# Cleanup
rm $TEMP_OUT