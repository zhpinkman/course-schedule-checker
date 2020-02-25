#include "course.hpp"
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>

const int NOT_FOUND = -1;

WeekDay getWeekDayFromString(std::string weekDay)
{
    return std::map<std::string, WeekDay>{
        {"Sat", Sat},
        {"Sun", Sun},
        {"Mon", Mon},
        {"Tue", Tue},
        {"Wed", Wed},
        {"Thu", Thu},
        {"Fri", Fri}
    }.at(weekDay);
}

Courses parseCourses(const CSVData &data)
{
    Courses courses;

    for (auto &&row : data)
    {
        Course newCourse = parseCourse(row);
        int courseIndex = findCourseIndexById(courses, newCourse.id);
        if (courseIndex != NOT_FOUND)
        {
            courses[courseIndex].sessions.push_back(newCourse.sessions.front());
        }
        else
        {
            courses.push_back(newCourse);
        }
    }

    return courses;
}

const std::string COLUMN_KEY_ID = "Id";
const std::string COLUMN_KEY_NAME = "Name";
const std::string COLUMN_KEY_UNITS = "Units";
const std::string COLUMN_KEY_DOW = "DoW";
const std::string COLUMN_KEY_START = "Start";
const std::string COLUMN_KEY_END = "End";
const std::string COLUMN_KEY_PREREQUISITES = "Prerequisites";
const std::string COLUMN_KEY_GRADE = "Grade";

Course parseCourse(const CSVRow &row)
{
    Course course;

    course.id = std::stoi(row.at(COLUMN_KEY_ID));
    course.name = row.at(COLUMN_KEY_NAME);
    course.units = std::stoi(row.at(COLUMN_KEY_UNITS));
    course.sessions.push_back(
        parseSession(
            row.at(COLUMN_KEY_DOW),
            row.at(COLUMN_KEY_START),
            row.at(COLUMN_KEY_END)
        )
    );
    course.prerequisites = parsePrerequisites(row.at(COLUMN_KEY_PREREQUISITES));

    return course;
}

int findCourseIndexById(const Courses &courses, CourseId id)
{
    for (size_t i = 0; i < courses.size(); i++)
    {
        if (courses[i].id == id)
        {
            return i;
        }
    }
    return NOT_FOUND;
}

Session parseSession(std::string dayOfWeek, std::string start, std::string end)
{
    Session session;

    session.weekDay = getWeekDayFromString(dayOfWeek);
    session.start = parseTime(start);
    session.end = parseTime(end);

    return session;
}

const char PREREQUISITE_DELIMITER = '-';
Prerequisites parsePrerequisites(std::string _prerequisites)
{
    Prerequisites prerequisites;
    std::stringstream prerequisitesStream(_prerequisites);
    std::string prerequisite;
    while (std::getline(prerequisitesStream, prerequisite, PREREQUISITE_DELIMITER))
    {
        CourseId id = std::stoi(prerequisite);
        if (id > 0)
        {
            prerequisites.push_back(id);
        }
    }
    return prerequisites;
}

Time parseTime(std::string _time) // time format is HH:MM
{
    Time time;

    size_t colonIndex = _time.find(':');
    time.hour = std::stoi(_time.substr(0, colonIndex));
    time.minute = std::stoi(_time.substr(colonIndex + 1, 2));

    return time;
}

Student parseCourseGrades(const CSVData &data)
{
    Student student;

    for (auto &&row : data)
    {
        CourseGrade courseGrade = parseCourseGrade(row);
        student.courseGrades[courseGrade.first] = courseGrade.second;
    }

    return student;
}

CourseGrade parseCourseGrade(const CSVRow &row)
{
    CourseGrade courseGrade;

    courseGrade.first = std::stoi(row.at(COLUMN_KEY_ID));
    courseGrade.second = std::stof(row.at(COLUMN_KEY_GRADE));

    return courseGrade;
}

void print(Time time)
{
    std::cout << (time.hour < 10 ? "0" : "") << time.hour << ':' << (time.minute < 10 ? "0" : "") << time.minute;
}

void print(Session session)
{
    std::cout << session.weekDay << ' ';
    print(session.start);
    std::cout << ' ';
    print(session.end);
}

void print(Course course, bool withDetails = false)
{
    std::cout << course.id;
    if (withDetails)
    {
        std::cout << ' ' << course.units << ' ';
        for (auto &&prerequisite : course.prerequisites)
        {
            std::cout << prerequisite << ' ';
        }
        std::cout << '\t' << course.name << "\t\t";
        for (auto &&session : course.sessions)
        {
            print(session);
            std::cout << ' ';
        }
    }
    std::cout << std::endl;
}

void print(Courses courses)
{
    for (auto &&course : courses)
    {
        print(course);
    }
}

Grade calculateGPA(const Courses &courses, Student student)
{
    Grade sumOfGrades = 0;
    int numberOfUnits = 0;

    for (auto &&courseGrade : student.courseGrades)
    {
        CourseId id = courseGrade.first;
        Grade grade = courseGrade.second;
        Course course = courses[findCourseIndexById(courses, id)];

        numberOfUnits += course.units;
        sumOfGrades += (course.units * grade);
    }

    return sumOfGrades / numberOfUnits;
}

bool hasPassed(Student student, CourseId id)
{
    return student.courseGrades[id] >= 10;
}

bool hasPassed(Student student, Course course)
{
    return hasPassed(student, course.id);
}

bool canGetCourse(Student student, Course course)
{
    if (hasPassed(student, course))
    {
        return false;
    }

    for (auto &&prerequisite : course.prerequisites)
    {
        if (!hasPassed(student, prerequisite))
        {
            return false;
        }
    }

    return true;
}

Courses findAvailableCourses(const Courses &courses, Student student)
{
    Courses availableCourses;
    for (auto &&course : courses)
    {
        if (canGetCourse(student, course))
        {
            availableCourses.push_back(course);
        }
    }
    return availableCourses;
}

bool compareCourseByName(const Course &lhs, const Course &rhs)
{
    return (lhs.name != rhs.name && lhs.name < rhs.name) || lhs.id < rhs.id;
}

bool compareCourseByUnitsAndName(const Course &lhs, const Course &rhs)
{
    return (lhs.units != rhs.units && lhs.units < rhs.units) || compareCourseByName(lhs, rhs);
}

Courses findNextTermCourses(const Courses &availableCourses, Grade currentGPA)
{
    Courses nextTermCourses;
    std::cout << currentGPA << std::endl;
    return nextTermCourses;
}

void runBoostan(const Courses &courses, Student student, int step)
{
    Courses availableCourses = findAvailableCourses(courses, student);
    if (step == 1)
    {
        std::sort(availableCourses.begin(), availableCourses.end(), compareCourseByName);
        print(availableCourses);
    }
    else if (step == 2)
    {
        std::sort(availableCourses.begin(), availableCourses.end(), compareCourseByUnitsAndName);
        print(findNextTermCourses(availableCourses, calculateGPA(courses, student)));
    }
}
