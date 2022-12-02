#ifndef STUDENT_H
#define STUDENT_H

#include <iostream>
#include <vector>

#define MAXNAMESTR 20
#define STUDENT0_DATAFILE "student.ssm"

#include <discipline.h>
#include <course.h>

class Student
{
    public:
        Student(std::string, class Course&,unsigned long int);
        ~Student();

        class Course& course(void);

        bool Save(void);
        bool Load(void);
        bool DeleteData();
        bool Load(std::ifstream &);
        bool LoadDefault(void);

        // getters
            std::string getName();
            std::string getCode();
            unsigned int getAge();
            std::string getBirthdate();
            float getFinalGrade();
            bool isRegistered();

        // setters
            bool setName(std::string);
            bool setAge(int[3]);
            bool setAge(unsigned int, unsigned int, unsigned int);
            bool setCode(long int);
            int Register(long int, std::string, std::string, unsigned int);

        // Course Methods
            std::string getCourse(bool=false);
            bool hasSubjects(void);

        // subjects methods
            void getSubjectsList(bool = false);
            bool hasEvaluations(void);


    protected:
        bool _registered;

    private:
        char _code[7 +1];                                   // limt is 7 chars, beware
        char _name[MAXNAMESTR];
        int _birthdate[3];
        float _finalGrade;

        char _datafile[25];

        class Course* _myCourse;

        /* private methods */
        void _calc_finalGrade(void);
};

bool validateName(std::string, bool = false);
void capitalize(std::string&);

#endif // STUDENT_H
