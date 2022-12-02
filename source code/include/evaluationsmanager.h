#ifndef EVALUATIONSMANAGER_H
#define EVALUATIONSMANAGER_H

#define EVALS_MAX 9
#define EVALS_MAX_DESCRIPTION_TEXT 21

static float EVALS_BASE_GRADE = 20.00;

#include <iostream>
#include <discipline.h>
#include <ctime>

enum EvalsType {
   /* generico     teste      Trabalho       Projeto       OEA       Exame*/
    EvalGeneric, EvalTest, EvalAssignment, EvalProject, EvalOthers, EvalExam
};

class EvaluationsManager
{
    private:
        char _datafile[20];
        Discipline* _subject = NULL;
        struct _Evaluations
        {
            int type;
            char description[EVALS_MAX_DESCRIPTION_TEXT];
            int date[3];
            int percentage;
            float grade;
            bool done;
        } _evals[EVALS_MAX+1];
        int _num_evals;
        int _scheduling;
        float _required_grade_average;
        short int _event_next = -1;
        struct _Evaluations *_exam = NULL;
        // internal methods
        void _refresh(void);
        void _refresh_next_event(int=-1);
        void _refresh_scheduling(void);
        void _reset_eval(int);
        bool _index(int i) {return(i>-1&&i<_num_evals);}
        bool _validate_date(int*, bool=false);

    public:
        EvaluationsManager(Discipline*);
        const struct _Evaluations* selectEval(int);
        // generals
        int getNumEvals();
        int getNumEvalsDone();
        int getNumEvalsType(EvalsType);
        int getEvalsScheduling();
        float getEvalsAverage();
        float getRequiredGradeAverade();
        bool isFullScheduled();
        std::string getStatus();
        // evals getter
        EvalsType getEvalType(int);
        std::string getEvalText(int);
        std::string getEvalTextShort(int);
        std::string getEvalDescription(int);
        tm getEvalDate(int);
        time_t getEvalDateTime(int);
        double getEvalDateTimeLeft(int);
        std::string getEvalDateString(int);
        int getEvalPercentage(int);
        float getEvalGrade(int);
        float getEvalGradeWeighted(int);
        bool isEvalScheduable(EvalsType);
        bool hasEvalScheduled(EvalsType=EvalsType::EvalGeneric);
        bool isEvalType(int,EvalsType);
        bool isEvalDone(int);
        bool isEvalDoable(int); // if eval eval time has passed.
        std::string getEvalDetails(int);
        // evals setter
        bool setEvalType(int,EvalsType);
        bool setEvalDescription(int,std::string);
        bool setEvalDate(int,int[3]);
        bool setEvalPercentage(int,int);
        //bool setEvalGrade(int,float);
        bool setEvalDone(int,bool);
        // exam
        bool hasExam();
        bool isExamDone();
        // next event
        bool hasNextEvent();
        int getNextEventIndex();
        time_t getNextEventTime();
        double getNextEventTimeLeft();
        std::string getNextEventDateString();
        std::string getNextEventDetails();
        // management
        bool gradeEval(int,float);
        bool schedule(EvalsType,int,int[3]);
        bool scheduleExam(int[3]);
        bool removeScheduled(int);
        void printListEvalsDetail();
        // data
        bool saveData();
        bool loadData(std::string = "");
};

std::string  getTimeLeftTill(time_t timeT);

#endif // EVALUATIONSMANAGER_H
