#include <string>
#include <iostream>
#include <algorithm>

#include "csv.hpp"
#include "course.hpp"

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        std::cout << "Too few arguments." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string coursesFileName = argv[1];
    std::string CourseGradesFileName = argv[2];
    int step = std::stoi(argv[3]);

    CSVData coursesData = readCSVFile(coursesFileName);
    CSVData CourseGradesData = readCSVFile(CourseGradesFileName);

    Courses courses = parseCourses(coursesData);
    Student student = parseCourseGrades(CourseGradesData);

    runBoostan(courses, student, step);

    return 0;
}
