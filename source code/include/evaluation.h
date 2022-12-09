#ifndef EVALUATION_H
#define EVALUATION_H


#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>

#define MAXEVALDESCRIPTION 15
#define MAXEVALUATION   7
#define ALLEVALSNUM   6
#define EVAL_MAX_PERC   100

static float BASE_GRADE = 20.00;

enum EvaluationType {
    Test = 0,               // Teste
    Assignment = 1,         // Trabalho
    Project = 2,            // Projeto
    Others = 3,             // Outros Elementos de avaliacao
    Exam = 4,               // Exam
    Generic = 5,            // Avaliacao qualquer
};
enum ExamEvaluationType {
    EXAM = 4
};

typedef enum EvaluationType     eval_t;
typedef enum ExamEvaluationType exam_t;

#include <discipline.h>

class Evaluation
{
    public:
        Evaluation(class Discipline*subject);
        Evaluation(Discipline*sbj, EvaluationType, const unsigned int, int[3]={0} );
        //Evaluation(class Discipline*, ExamEvaluationType, int[3]);
        //Evaluation& operator=(const EvaluationType);
        bool hasSubjectOf(void);

        // methods
        std::string detail(bool=true);

        // getters
        std::string getDescription();
        EvaluationType getType();
        float getRequiredGrade();
        float getPercentage();
        int getPercentageInt();
        float getGrade();
        float getGradeWeighted();
        tm getDate();
        time_t getDateTime();
        double getDateTimeLeft();
        std::string getDateString(bool=false);
        bool isTypeOf(EvaluationType);
        bool isExam();
        const bool isDoable();
        bool isDone();
        std::string getTimeStatus(void);
        std::string getResultStatus(void);
        // setters
        bool setType(const EvaluationType);
        bool setDescription(std::string&);
        bool setGrade(const float);
        bool setPercentage(const unsigned int);
        bool setDate(int[3]);
        bool setRequiredGrade(const float);
        void setDone(const bool);
        // changed
        bool getChangedStatus(void);
        // datafile
        bool loadData(std::string);
        bool saveData(std::string);
        bool _checkDataCode(std::string);
        std::string _getDataCode();

    private:

        Discipline* _subjects_of = NULL;
        EvaluationType _type;
        int _percentage = 0;
        char _description[MAXEVALDESCRIPTION];
        float _grade = 0.0;
        float _requiredGrade = 0.0;
        int _date[3];
        bool _done = false;
        bool _changed = false;

        char __datacode[8];

};

//class ExamEvaluation : public Evaluation
//{
//    public:
//        ExamEvaluation(unsigned int[3]);
//
//        // methods
//        // setters
//        void setType(void);
//        bool setPercentage(void);
//        // getters
//        TypeOfExamEvaluation getType();
//
//    private:
//    const ExamEvaluationType _type;
//    const int _percentage = 100;
//};

/* evaluations general functions */

typedef struct EvaluationNode {
    private:
    Discipline* _subject_of = NULL;
    Evaluation* _eval = NULL;
    struct EvaluationNode* _next = NULL;
    struct EvaluationNode* _prev = NULL;

    public:
    EvaluationNode(){
        _eval = NULL;
        _next = _prev = NULL;
    }
    Evaluation* operator () (){
        return get();
    }

    Evaluation* get(void){
        return this->_eval;
    }
    EvaluationNode* next(void){
        return _next;
    }
    Evaluation* getNext(int i){
        struct EvaluationNode* eval = this;
        while(i-- > 0) eval = eval->next();
        return eval->_eval;
    }
    bool push(Evaluation* eval){
        if(!eval)
            return false;
        else if(_eval == NULL){
            _eval = eval;
        }
        else {
            _next = new struct EvaluationNode();
            _next->_eval = eval;
            _next->_prev = this;
        }
        return true;
    }
    bool remove(void){
        if(!_eval) return false;
        struct EvaluationNode* delNode;
        if(_prev != NULL){
            delNode = this;
            _prev->_next = this->_next;
            _next = NULL;
        } else {
            delNode = _next;
            _eval = _next->_eval;
            _next = _next->_next;
            _prev = NULL;
        }
        delete delNode;
        return true;
    }
    // gets
    int pos(void){
        struct EvaluationNode* tmp = this;
        int pos = 0;
        while(tmp->_prev != NULL){
            tmp = tmp->_prev;
            pos++;
        }
        return pos;
    }
    bool is_last(void){
        return (_next == NULL);
    }
    bool is_eval(Evaluation* eval){
        return (get() == eval);
    }

} EvalNode;

class EvalsManager
{
    private:
        class Discipline* _subject;
        Evaluation* _evals[MAXEVALUATION+1];
        Evaluation *_exam, *_event_next;
        int* _num_of_evals;  // 0
        int* _scheduling;    // 0%
        bool index_ok(int i){return (i>-1 && i<*_num_of_evals);}

        char __evalfile[MAXEVALUATION+1][24];

        EvalNode* getNode(){
            EvalNode* node = new EvaluationNode();
            for(int i = 0; i < getNumOfEvals(); i++){
                node->push(_evals[i]);
            }
            return node;
        }

    public:
        EvalsManager(Discipline* sbj){
            _subject = sbj;
            _num_of_evals = new int(0);
            _scheduling = new int(0);
            for(int i=0;i<=MAXEVALUATION;i++){
                _evals[i]=NULL;
            }
            _exam = NULL;
            _event_next = NULL;
            *_num_of_evals = 0;
            *_scheduling = 0;
        }
        EvalsManager(Discipline* sbj, int* num_evals_ref, int* scheduling_ref){
            _subject = sbj;
            _scheduling = scheduling_ref;
            _num_of_evals = num_evals_ref;
            for(int i=0;i<=MAXEVALUATION;i++)
                _evals[i]=NULL;
            _exam = NULL;
            _event_next = NULL;
        }
        float getGradeAverage(){
            float avg = 0;
            if(isExamScheduled()){
                return _exam->getGrade();
            }
            for(int i=0;i<getNumOfEvals();i++)
                avg += _evals[i]->getGradeWeighted();
            return avg;
        }
        // eval
        Evaluation* getEval(int index){
            if(!index_ok(index)) return NULL;
            return _evals[index];
        }
        int getNumOfEvals(void){
            return *_num_of_evals;
        }
        int getNumOfEvalsDone(void){
            int num = 0;
            for(int i=0;i<getNumOfEvals();i++)
                num += _evals[i]->isDone() ? 1 : 0;
            return num;
        }
        int getScheduling(void){
            int scheduling = 0;
            if(_exam != NULL){
                scheduling = EVAL_MAX_PERC;
            } else {
                for(int i = 0; i < getNumOfEvals(); i++){
                    scheduling += _evals[i]->getPercentageInt();
                }
            }
            return scheduling;
        }
        bool hasScheduled(void){
            return (getNumOfEvals() > 0);
        }
        bool hasScheduled(eval_t type){
            for(int i=0;i<getNumOfEvals();i++){
                if(_evals[i]->isTypeOf(type)) return true;
            }
            return false;
        }
        bool isSchedulable(eval_t type = eval_t::Generic){
            if(type == Exam){
                if( _exam != NULL) return false;
            } else
            if(getNumOfEvals() >= MAXEVALUATION){
                return false;
            } else
            if(getScheduling() >= EVAL_MAX_PERC){
                return false;
            }else
            if(type == Others){
                if(hasScheduled(Others))
                    return false;
            }
            return true;
        }
        bool isFullScheduled(void){
            return (getScheduling() >= EVAL_MAX_PERC);
        }
        // exam
        Evaluation* getExam(void){
            return _exam;
        }
        bool isExamScheduled(void){
            return (_exam!=NULL);
        }
        // event
        Evaluation* getEventNext(){
            return _event_next;
        }
        time_t getEventNextTime(){
            return getEventNext()->getDateTime();
        }
        // updates
        void refresh__(){
            // update data
            Evaluation* eval;
            *_scheduling = 0;
            _event_next = NULL;
            time_t time1 = 0, time2 = 0;
            for(int i = 0; i < getNumOfEvals();i++){    // general
                eval = _evals[i];
                // check next
                if(!eval->isTypeOf(Others) && !eval->isDone() && !eval->isDoable()){
                    time2 = eval->getDateTime();
                    if(time2 < time1 || time1 == 0){
                        time1 = time2;
                        _event_next = eval;
                    }
                }
                // calc scheduling
                if(!eval->isExam()){
                    *_scheduling += eval->getPercentageInt();
                }
                // check exam
                else {
                    _exam = eval;
                }
            }
        }
        // management
        bool scheduleExam(int date[3]){
            if(isExamScheduled()) return false;
            Evaluation* new_eval = new Evaluation(_subject,Exam, EVAL_MAX_PERC, date);
            if(new_eval){
                _evals[*_num_of_evals] = new_eval;
                _exam = new_eval;
            } else return false;
            *_num_of_evals += 1;
            refresh__();
            return true;
        }
        bool schedule(eval_t type, int perc, int date[3]){
            if(!isSchedulable(type)){
                std::cout << "Esta avaliacao nao pode ser mais agendada.\n";
                return false;
            } else
            if(type == eval_t::Exam){
                return scheduleExam(date);
            }
            // schedule
            if(perc <= 0 || perc > EVAL_MAX_PERC){
                std::cout << "percentagem invalida.\n";
                return false;
            } else
            if((perc + getScheduling()) > EVAL_MAX_PERC){
                std::cout << "percentagem total ultrapassa " <<EVAL_MAX_PERC<< "%.\n";
                return false;
            }
            Evaluation* new_eval = new Evaluation(_subject, type, perc, date);
            if(new_eval == NULL) return false;
            _evals[(*_num_of_evals)] = new_eval;
            *_scheduling += perc;
            *_num_of_evals += 1;
            refresh__();
            return true;
        }
        bool evaluate(int index, double grade){
            if(!index_ok(index)) return false;
            if(!_evals[index]->setGrade(grade))
                return false;
            refresh__();
            return true;
        }
        bool removeScheduled(int index){
            if(!index_ok(index)) return false;
            Evaluation* del = _evals[index];
            for(int i=index;i<getNumOfEvals()-1;i++){
                _evals[i] = _evals[i+1];
            }
            *_num_of_evals -= 1;
            _evals[*_num_of_evals] = NULL;
            if(del->isExam()) _exam = NULL;
            delete del;
            refresh__();
            return true;
        }

        bool saveData(std::string& filepath){
            std::string filename;
            char datastream[25];
            remove((filepath+"00.evl").c_str());
            std::ofstream filedata( (filepath+"00.evl"), std::ios::trunc);
            if(filedata.fail()) return false;
            filedata.flush();
            filedata << "# Do not touch this file\n";
            for(int i = 0; i < getNumOfEvals(); i++){
                sprintf(datastream,"%s%02i.evl", filepath.c_str(),i+1);
//                sprintf(datastream,"%s%s.evl", filepath.c_str(),_evals[i]._getDataCode());
                filename = datastream;
                if(_evals[i]->saveData(filename)){
                    filedata << datastream << ':' << _evals[i]->_getDataCode() << std::endl;
                    //filedata.write((char*)&datastream, sizeof(datastream));
                }
            }
            filedata.close();
            return true;
        }
        bool loadData(std::string& filepath){
            std::string filename;
            std::string line, key;
            std::string::size_type pos;
            int k=0, perc=0;
            //char datastream[25];
            // reset data
            _exam = _event_next = NULL;
            Discipline* hold_sbj = _subject;
            std::ifstream filedata((filepath+"00.evl"), std::ios::in);
            if(!filedata.is_open()) return false;
            // load
            while(getline(filedata, line)){
                if(line.front() == '#') continue;
                Evaluation* load_eval = new Evaluation(_subject);
                pos = line.find(":");
//                pos = in_str.find("_"); // %s%s.evl
                key = line.substr (pos+1);
                line = line.substr (0, pos);
                if(!load_eval->loadData(line)){
                    continue;
                } else
                if(!load_eval->_checkDataCode(key)){
                    std::cout << "evaluation violation!";
                    continue;
                }
                _evals[k] = load_eval;
                if(load_eval->isExam()){
                    _exam = load_eval;
                } else {
                    perc += load_eval->getPercentageInt();
                }
                k++;
                load_eval = NULL;
            }
            filedata.close();
            // load data END
            *_num_of_evals = k;
            *_scheduling = perc;
            _subject = hold_sbj;
            refresh__();
            return true;
        }
        bool loadData_old(std::string& filepath){
            return false;
            /*
            if(filepath.empty()) return false;
            //std::string filename = _subject->getDatafileStr();
            std::ifstream datafile(filepath, std::ios::binary);
            if(!datafile.is_open()) return false;
            // reset data
            _exam = _event_next = NULL;
            Discipline* hold_sbj = _subject;
            // load data
            for(int i=0; i<MAXEVALUATION;i++) _evals[i] = NULL;
            int k =0, perc = 0;
            while(!datafile.eof()){
                Evaluation* load_eval = new Evaluation(_subject);
                if(load_eval->loadData(datafile)){
                    _evals[k] = load_eval;
                    if(load_eval->isExam()){
                        _exam = load_eval;
                    } else {
                        perc += load_eval->getPercentageInt();
                    }
                    k++;
                }
                load_eval = NULL;
            }
            // load data END
            datafile.close();
            *_num_of_evals = k;
            *_scheduling = perc;
            _subject = hold_sbj;
            refresh__();
            return true;
        */
        }
        bool saveData_old(std::string& datfilepath){
            return false;
            /*
            if(datfilepath.empty()) return false;
            remove(datfilepath.c_str());
//            std::ofstream datafile(datfilepath, std::ios::binary | std::ios::app);
            std::ofstream datafile(datfilepath, std::ios::binary);
            if(datafile.fail()) return false;
            // save data
            for(int i = 0; i < getNumOfEvals(); i++){
                //_evals[i]->saveData(datafile);
            }
            // save data END
            datafile.close();
            return true;
            */
        }
        // Node


};


//void printListOfAvaliableAssessmentsOf(Discipline&);
bool validateDate(int[3], bool = false);
std::string getEvalName(EvaluationType, bool = false);
std::string getTimeLeftTo(time_t);

#endif // EVALUATION_H
