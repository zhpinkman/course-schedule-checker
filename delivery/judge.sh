#!/usr/bin/env bash

CODE_ADDR=$2

EXE=a.out
COMPILER="g++ -std=c++11 -o $EXE"
VERBOSE=false
TEMP_DIR="temp"
TESTCASES_DIR="../testcases"
OUT_EXT="out"
TIME_LIM=5s
VERBOSE_DIFF_TOOL="sdiff -sWw 60"
# VERBOSE_DIFF_TOOL="p4merge"
DIFF_TOOL="sdiff -sW"
DIR_DIFF_TOOL="diff"

RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
B="\033[1;4m"
NC="\033[0m"

CODE_ADDR=$(readlink -f "$CODE_ADDR")

if [[ ! -e $TEMP_DIR ]]; then
    mkdir $TEMP_DIR
fi

pushd "$TEMP_DIR" > /dev/null
rm -r * 2> /dev/null
passed=0
failed=0
compiled=true
echo -e "\n${YELLOW}Compiling...${NC}"
if ! $COMPILER "$CODE_ADDR"; then
    echo -e "${RED}Compile Error${NC}"
    compiled=false
    failed=$(ls "$TESTCASES_DIR" | wc -l)
else
    echo -e "${GREEN}Compiled!${NC}"
    echo -e "\n${YELLOW}Running...${NC}"
    for testcase in $(ls "$TESTCASES_DIR"); do
        solDir="$TESTCASES_DIR/$testcase/outputs/$1"
        output="$testcase.$OUT_EXT"
        coursesFile="$TESTCASES_DIR/$testcase/courses.csv"
        gradesFile="$TESTCASES_DIR/$testcase/grades.csv"
        printf "Testcase $testcase: "
        if timeout $TIME_LIM ./$EXE "$coursesFile" "$gradesFile" > "$output"; then
            if [ "$1" -eq 3 ];then
                outDir="$testcase"

                mkdir -p $outDir
                mv *.sched "$outDir/"

                if $DIR_DIFF_TOOL "$outDir" "$solDir" > /dev/null; then
                    echo -e "${GREEN}Accepted${NC}"
                    ((passed+=1))
                else
                    echo -e "${RED}Wrong Answer${NC}"
                    ((failed+=1))
                fi
            else
                sol="$solDir/stdout"
                if $DIFF_TOOL "$output" "$sol" > /dev/null; then
                    echo -e "${GREEN}Accepted${NC}"
                    ((passed+=1))
                else
                    echo -e "${RED}Wrong Answer${NC}"
                    if $VERBOSE; then
                        printf "%28s | %28s\n" "< $output" "> $sol"
                        $VERBOSE_DIFF_TOOL "$output" "$sol"
                    fi
                    ((failed+=1))
                fi
            fi
        else
            echo -e "${RED}Timed out${NC}"
            ((failed+=1))
        fi
    done
fi

echo -e "\n${YELLOW}Report${NC}"
echo -e "Part: $1"
printf "Code: $2 (compiled: %b)\n" $compiled
echo -e "Passed:\t${GREEN}$passed${NC} out of $((passed + failed))"
echo -e "Failed:\t${RED}$failed${NC} out of $((passed + failed))"
popd  > /dev/null
