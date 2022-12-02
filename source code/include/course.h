#ifndef COURSE_H
#define COURSE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

#define MAXCOURSESTR 40
#define COURSEDATAFILE "course.ssm"

// RETURN CONSTAT
#define DISCIPLINE_NOT_ADDED -1
#define LIMITSTUDENTREG 1

#include <student.h>
#include <discipline.h>
#include <evaluation.h>

typedef enum type_semester { first_semester=1, second_semester=2} typeSemester;
typedef enum type_disciplines { disciplines_registered, disciplines_archived} typeDiscipline;

class Course
{
    public:
        Course(std::string, short int = 1, enum type_semester = first_semester);
        ~Course() { delete _disciplines; delete _archived_disciplines;}

        class Discipline* discipline(const unsigned int);
        class Discipline* archived(unsigned int);
        class Student& student(void);

        bool Save();
        bool Load();
        bool DeleteData();
        bool SaveDisciplines(void);
        bool LoadDisciplines(void);

        // student methods
        bool registerStudent(class Student&);
        bool unregisterStudent(class Student&);
        bool hasStudentRegistered(void);

        // disciplines methods
        Discipline* getDiscipline(const std::string);
        bool add_discipline(std::string, std::string);
        bool add_discipline(class Discipline*);
        bool del_discipline(std::string, const typeDiscipline = disciplines_registered);
        bool del_disciplineRegistered(Discipline*);
        bool hasDiscipline();
        // archived
        Discipline* getDisciplineArchived(const std::string);
        bool archive_discipline(std::string);
        bool archive_discipline(Discipline*);
        bool del_disciplineArchived(Discipline*);
        bool hasDisciplineArchived();

        // getter
            std::string getCourse();
            std::string getCourseAcronym(void);
            unsigned int getSemester(bool=false);
            unsigned int getLective();
            unsigned int getCredits();

        // setter
            bool setCourse(std::string);
            bool setCourseAcronym(std::string);
            bool setSemester(unsigned short int);
            bool setLective(unsigned short int);

        // Subjects Getter & setter
            unsigned int getIndexOf(Discipline* dscp);
            unsigned int getIndexOf(std::string);
            unsigned int getNumDisciplines();
            unsigned int getNumDisciplinesArchived();

        // assessment methods
        int getNumOfEvaluation();
        int getNumOfEvalOfType(EvaluationType);
        int getNumOfExams();
        double getGradeMean();
        std::string getNextEvaluationEvent();

    protected:

    private:
        char _coursename[MAXCOURSESTR+1];
        char _acronym[6];
        unsigned short int _semester;
        unsigned short int _lective; // year: 1, 2, 3, 4

        char _datafile[25];
        int _credits;
        char _dcpdatalist[25];

        int _numOfDisciplines;
        int _numOfArchived;

        Student *_student;
        DisciplineList *_disciplines = NULL;
        DisciplineList *_archived_disciplines = NULL;

        // private methods
        bool _add_archived(class Discipline*);
        void _calc_credits(void);
        void _deleteStudent(int=0);
        void _updateDisciplineListFile();

};

void printDiscilineListOf(Course&, bool = false, int = 0, int = -1);
int subjectsSelectorIndex(Course*);
Discipline* subjectsSelector(Course*);
void CourseSaveDataOnThread(void*);

#endif // COURSE_H
