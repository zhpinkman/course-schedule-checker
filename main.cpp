#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

const char CSV_DELIMITER = ',';
const char PREREQUISITE_DELIMITER = '-';

const int NOT_FOUND = -1;
const float GROWTH_RATE = 1.05;

const std::string COLUMN_KEY_ID = "Id";
const std::string COLUMN_KEY_NAME = "Name";
const std::string COLUMN_KEY_UNITS = "Units";
const std::string COLUMN_KEY_DOW = "DoW";
const std::string COLUMN_KEY_START = "Start";
const std::string COLUMN_KEY_END = "End";
const std::string COLUMN_KEY_PREREQUISITES = "Prerequisites";
const std::string COLUMN_KEY_GRADE = "Grade";

const std::string FILE_PREFIX = "semester";
const std::string FILE_SUFFIX = ".sched";

typedef std::string Key;
typedef std::vector<Key> CSVHeader;
typedef std::map<Key, std::string> CSVRow;
typedef std::vector<CSVRow> CSVData;

enum WeekDay
{
    Sat,
    Sun,
    Mon,
    Tue,
    Wed,
    Thu,
    Fri
};

struct Time
{
    int hour, minute;
};

struct Session
{
    Time start, end;
    WeekDay weekDay;
};

typedef int CourseId;
typedef float Grade;
typedef std::vector<CourseId> Prerequisites;

struct Course
{
    CourseId id;
    std::string name;
    int units;
    std::vector<Session> sessions;
    Prerequisites prerequisites;
};

typedef std::vector<Course> Courses;
typedef std::pair<CourseId, Grade> CourseGrade;

struct Student
{
    std::map<CourseId, Grade> courseGrades;
};


CSVData readCSVFile(std::string fileName);
CSVHeader parseCSVHeader(std::string row);
CSVRow parseCSVRow(std::string rawRow, CSVHeader header);

WeekDay getWeekDayFromString(std::string weekDay);
int maxAllowedUnits(Grade gpa);
Courses parseCourses(const CSVData &data);
Course parseCourse(const CSVRow &row);
int findCourseIndexById(const Courses &courses, CourseId id);
Session parseSession(std::string dayOfWeek, std::string start, std::string end);
Prerequisites parsePrerequisites(std::string _prerequisites);
Time parseTime(std::string _time); // time format is HH:MM
Student parseCourseGrades(const CSVData &data);
CourseGrade parseCourseGrade(const CSVRow &row);
void print(const Time &time, std::ostream &stream);
void print(const Session &session, std::ostream &stream);
void print(const Course &course, std::ostream &stream, bool withDetails);
void print(const Courses &courses, std::ostream &stream, bool withDetails);
void printToFile(std::string fileName, const Courses &courses);
Grade calculateGPA(const Courses &courses, Student student);
bool hasPassed(Student student, CourseId id);
bool hasPassed(Student student, Course course);
bool canGetCourse(Student student, Course course);
Courses findAvailableCourses(const Courses &courses, Student student);
bool compareCourseByName(const Course &lhs, const Course &rhs);
bool compareCourseByUnitsAndName(const Course &lhs, const Course &rhs);
int calculateNumberOfUnits(const Courses &courses);
bool canTakeCourse(const Courses &nextTermCourses, Grade currentGPA, Course course);
Courses findNextTermCourses(const Courses &availableCourses, Grade currentGPA);
bool hasPassedAllCourses(const Courses &courses, Student student);
void passCourses(Student &student, const Courses &courses, Grade grade);
void runBoostan(const Courses &courses, Student student, int step);

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Too few arguments." << std::endl;
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


CSVData readCSVFile(std::string fileName)
{
    std::ifstream fin(fileName);
    std::string row;

    std::getline(fin, row);
    CSVHeader header = parseCSVHeader(row);

    CSVData data;
    while (std::getline(fin, row))
    {
        data.push_back(parseCSVRow(row, header));
    }

    return data;
}

CSVHeader parseCSVHeader(std::string row)
{
    CSVHeader header;
    std::string cell;
    std::stringstream rowStream(row);

    while (std::getline(rowStream, cell, CSV_DELIMITER))
    {
        header.push_back(cell);
    }

    return header;
}

CSVRow parseCSVRow(std::string rawRow, CSVHeader header)
{
    CSVRow row;
    std::string cell;
    std::stringstream rowStream(rawRow);
    size_t index = 0;

    while (std::getline(rowStream, cell, CSV_DELIMITER) && index < header.size())
    {
        row[header[index++]] = cell;
    }

    return row;
}

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

int maxAllowedUnits(Grade gpa)
{
    return gpa >= 17 ? 24 : 20;
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
            row.at(COLUMN_KEY_END)));
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

void print(const Time &time, std::ostream &stream = std::cout)
{
    stream << (time.hour < 10 ? "0" : "") << time.hour << ':' << (time.minute < 10 ? "0" : "") << time.minute;
}

void print(const Session &session, std::ostream &stream = std::cout)
{
    stream << session.weekDay << ' ';
    print(session.start);
    stream << ' ';
    print(session.end);
}

void print(const Course &course, std::ostream &stream = std::cout, bool withDetails = false)
{
    stream << course.id;
    if (withDetails)
    {
        stream << ' ' << course.units << ' ';
        for (auto &&prerequisite : course.prerequisites)
        {
            stream << prerequisite << ' ';
        }
        stream << '\t' << course.name << "\t\t";
        for (auto &&session : course.sessions)
        {
            print(session);
            stream << ' ';
        }
    }
    stream << std::endl;
}

void print(const Courses &courses, std::ostream &stream = std::cout, bool withDetails = false)
{
    for (auto &&course : courses)
    {
        print(course, stream, withDetails);
    }
}

void printToFile(std::string fileName, const Courses &courses)
{
    std::ofstream fout(fileName, std::fstream::out | std::fstream::trunc);
    if (fout.is_open())
    {
        print(courses, fout);
        fout.close();
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

int calculateNumberOfUnits(const Courses &courses)
{
    int result = 0;
    for (auto &&course : courses)
    {
        result += course.units;
    }
    return result;
}

bool canTakeCourse(const Courses &nextTermCourses, Grade currentGPA, Course course)
{
    int numberOfUnits = calculateNumberOfUnits(nextTermCourses) + course.units;
    return numberOfUnits <= maxAllowedUnits(currentGPA);
}

Courses findNextTermCourses(const Courses &availableCourses, Grade currentGPA)
{
    Courses nextTermCourses;
    Courses::const_iterator courseIter = availableCourses.begin();
    while (courseIter != availableCourses.end())
    {
        Course course = *courseIter;
        if (canTakeCourse(nextTermCourses, currentGPA, course))
        {
            nextTermCourses.push_back(course);
        }
        courseIter++;
    }
    return nextTermCourses;
}

bool hasPassedAllCourses(const Courses &courses, Student student)
{
    for (auto &&course : courses)
    {
        if (!hasPassed(student, course))
        {
            return false;
        }
    }
    return true;
}

void passCourses(Student &student, const Courses &courses, Grade grade)
{
    for (auto &&course : courses)
    {
        student.courseGrades[course.id] = grade;
    }
}

void runBoostan(const Courses &courses, Student student, int step)
{
    if (step == 1)
    {
        Courses availableCourses = findAvailableCourses(courses, student);
        std::sort(availableCourses.begin(), availableCourses.end(), compareCourseByName);
        print(availableCourses);
    }
    else if (step == 2)
    {
        Courses availableCourses = findAvailableCourses(courses, student);
        std::sort(availableCourses.begin(), availableCourses.end(), compareCourseByUnitsAndName);
        print(findNextTermCourses(availableCourses, calculateGPA(courses, student)));
    }
    else if (step == 3)
    {
        int iteration = 1;
        while (!hasPassedAllCourses(courses, student))
        {
            Courses availableCourses = findAvailableCourses(courses, student);
            std::sort(availableCourses.begin(), availableCourses.end(), compareCourseByUnitsAndName);
            Grade currentGPA = calculateGPA(courses, student);
            Courses nextTermCourses = findNextTermCourses(availableCourses, currentGPA);
            passCourses(student, nextTermCourses, currentGPA * GROWTH_RATE);
            printToFile(FILE_PREFIX + std::to_string(iteration++) + FILE_SUFFIX, nextTermCourses);
        }
    }
}
