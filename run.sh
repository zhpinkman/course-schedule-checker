PROG=Boostan.out

courses_file_name=$1
grades_file_name=$2
step_number=$3

g++ -std=c++11 -Wall -pedantic main.cpp -o $PROG
./$PROG $courses_file_name $grades_file_name $step_number
