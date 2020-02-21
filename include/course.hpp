#include <string>
#include <vector>
#include "csv.hpp"

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

WeekDay getWeekDayFromString(std::string weekDay);

Courses parseCourses(const CSVData &data);
Course parseCourse(const CSVRow &row);
int findCourseIndexById(const Courses &courses, CourseId id);

Session parseSession(std::string dayOfWeek, std::string start, std::string end);
Prerequisites parsePrerequisites(std::string _prerequisites);
Time parseTime(std::string _time);

Student parseCourseGrades(const CSVData &data);
CourseGrade parseCourseGrade(const CSVRow &row);

void runBoostan(const Courses &courses, Student student, int step);

