#!/bin/bash

EXEC="./search_index"
IN_DIR="tests/in"
OUT_DIR="tests/out"
DIFF_DIR="tests/diff"
TEMP="temp.out"

normalize() {
    sed 's/[[:space:]]*$//' "$1" | sed '/^$/d'
}

# Score table: test number -> points
declare -A SCORE_TABLE
for i in $(seq 1 10);  do SCORE_TABLE[$i]=1.25; done   # tests for ADD and DEL
for i in $(seq 11 20); do SCORE_TABLE[$i]=0.75; done   # tests for ADDKW and DELKW
for i in $(seq 21 25); do SCORE_TABLE[$i]=1.00; done   # tests for FIND
for i in $(seq 26 30); do SCORE_TABLE[$i]=3.00; done   # tests for TOPK
for i in $(seq 31 40); do SCORE_TABLE[$i]=1.00; done   # tests for PREFIX - BONUS
for i in $(seq 41 50); do SCORE_TABLE[$i]=1.25; done   # tests for ADD and DEL
for i in $(seq 51 60); do SCORE_TABLE[$i]=0.75; done   # tests for ADDKW and DELKW
for i in $(seq 61 65); do SCORE_TABLE[$i]=1.00; done   # tests for FIND
for i in $(seq 66 70); do SCORE_TABLE[$i]=3.00; done   # tests for TOPK
for i in $(seq 71 80); do SCORE_TABLE[$i]=1.00; done   # tests for PREFIX - BONUS

echo "Running tests..."

# Build the executable
make build
if [ $? -ne 0 ]; then
    echo "Build failed!"
    echo "Score: 0 / 120"
    exit 1
fi

echo "Build succeeded. +5 pts"
score="5.00"
total="5.00"

readme_found=$(find . -maxdepth 2 -iname "*README*" | head -1)
if [ -n "$readme_found" ]; then
    score=$(echo "scale=2; $score + 5" | bc)
    total=$(echo "scale=2; $total + 5" | bc)
    echo "README found ($readme_found) - +5 pts"
else
    total=$(echo "scale=2; $total + 5" | bc)
    echo "README not found - 0/5 pts"
fi

passed=0

for input_file in "$IN_DIR"/test*.in; do
    test_name=$(basename "$input_file" .in)
    expected="$OUT_DIR/${test_name}.out"

    if [ ! -f "$expected" ]; then
        continue
    fi

    test_num="${test_name#test}"
    test_num=$((10#$test_num))
    points="${SCORE_TABLE[$test_num]:-1}"
    total=$(echo "scale=2; $total + $points" | bc)

    cp "$input_file" indexare.in
    "$EXEC"
    cp indexare.out "$TEMP"

    normalize "$TEMP" > "${TEMP}.norm"
    normalize "$expected" > "${expected}.norm"

    if diff -q "${TEMP}.norm" "${expected}.norm" > /dev/null 2>&1; then
        score=$(echo "scale=2; $score + $points" | bc)
        echo "$test_name ($points pts) - PASSED"
        passed=$((passed + 1))
    else
        mkdir -p "$DIFF_DIR"
        diff_file="$DIFF_DIR/${test_name}.diff"
        diff -u "${TEMP}.norm" "${expected}.norm" > "$diff_file"
        echo "$test_name ($points pts) - FAILED - see diff in $diff_file"
    fi

    rm -f "${TEMP}.norm" "${expected}.norm"
done

rm -f "$TEMP" indexare.in indexare.out

if [ "$passed" -gt 10 ]; then
    score=$(echo "scale=2; $score + 10" | bc)
    total=$(echo "scale=2; $total + 10" | bc)
    echo "More than 10 tests passed - +10 pts (PRINT function)"
else
    total=$(echo "scale=2; $total + 10" | bc)
    echo "10 or fewer tests passed - 0/10 pts (PRINT function)"
fi

echo "Score: $score / $total"
