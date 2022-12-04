#ifndef DISCIPLINE_H
#define DISCIPLINE_H

#include <evaluation.h>

#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>

#define MAXTITLESTR     11
#define MAXNAMESTR      20

static const float MAXMARKS = 20.0;

// ccrate a class of node to ihnerit to Discipline class, use private methods




class Discipline
{
    public:
        Discipline(class Course*, std::string, std::string, unsigned int);
        ~Discipline(){ try{delete this;}catch(...){}}


        int SaveData();
        int LoadData(std::string = "");
        bool DeleteData();
        std::string getDatafileStr(void);

        // getters
        bool titles(const std::string);
        std::string getTitle();
        std::string getDescription();
        std::string getProfessor();
        unsigned int getSemester();
        std::string getStatus();
        std::string getStatusProgress();
        float getFinalGradeRequired(void);
        bool isCompleted();
        bool isArchived(bool=true);

        // setters
        bool setDescription(std::string);
        bool setProfessor(std::string);
        bool setSemester(unsigned int);
        bool setFinalGradeRequired(float);
        bool setSubjectArchived(Course* = NULL);

        //! Evaluation
        //struct EvaluationManager* evals();
        class Evaluation* evals(unsigned int index);
        Evaluation* getEvalNextEvent();
        int getNumOfEvals();
        int getNumOfEvals(EvaluationType type);
        int getNumOfEvalsDone();
        int getEvalsStatus();
        float getEvalsGradeAverage(bool=false);
        time_t getEvalNextEventTime();
        bool hasEvalScheduled();
        bool hasEvalScheduled(eval_t);
        bool hasExamScheduled();
        bool isExamDone();
        bool isEvalsAllDone();
        bool isEvalsFullScheduled();
        bool isEvalSchedulable(EvaluationType = eval_t::Generic);
        void printEvalsList();
        void printEvalsBrief();

        bool scheduleEvaluation(const EvaluationType , float , int[3]);
        bool removeScheduledEvaluation(int);
        bool gradeEvaluation(int, float);
        void refresh(void);


        // operators method
        bool operator < (Discipline* pdscp);
        bool operator > (Discipline* pdscp);
        bool operator < (Discipline& dscp);
        bool operator > (Discipline& dscp);
        bool operator == (Discipline& dscp);
        bool operator && (Discipline& dscp);    // equals by reference
        bool equal(const std::string);

    protected:

    private:
        char _title[MAXTITLESTR];
        char _professor[MAXNAMESTR];
        char _description[MAXTITLESTR+MAXNAMESTR];
        int _semester;
        int _scheduling;
        int _num_of_evals;
        float _finalGrade;
        float _finalGradeRequired;
        bool _completed_ = false;
        bool __archived_ = false;

        char _datafile[25];
        //char _evalfile[MAXEVALUATION*11];


        class Course* _course;
        //std::vector<Evaluation*> _evals = new std::vector<Evaluation*>;
        class EvalsManager* _evals;

        bool _check_index(int index){return (index > -1 && index < getNumOfEvals());}

        // data management
        bool SaveEvaluations();
        bool LoadEvaluations();
};

struct discipline_node {
    private:
    Discipline *_discipline = NULL;
    discipline_node *_next, *_prev;

    public:
    discipline_node(Discipline* discipline_ref){
        _discipline = discipline_ref;
        _next = _prev = NULL;
    }
    class Discipline* discipline(void){ return _discipline; }
    struct discipline_node* next(void){ return _next; }
    struct discipline_node* prev(void){ return _prev; }
    bool hasNext(void) { return (_next != NULL); }
    bool hasPrev(void) { return (_prev != NULL); }
    bool is_valid(void) { return (_discipline != NULL); }
    bool operator == (Discipline& discipline){ return (this->_discipline == &discipline); }

    bool push(struct discipline_node** node){
        try{
            if(*node == NULL)  return false;
            (*node)->_next = this->_next;
            (*node)->_prev = this;
            if(hasNext()) this->_next->_prev = *node;
            this->_next = *node;
        }catch(...){return false;}
        return true;
    }
    bool push_over(struct discipline_node** node){
        try{
            if(*node == NULL)  return false;
            (*node)->_next = this;
            (*node)->_prev = this->_prev;
            if(hasPrev()) this->_prev->_next = *node;
            this->_prev = *node;
        }catch(...){return false;}
        return true;
    }
    Discipline* unlink_node(bool delete_ref = false){
        if(hasNext()) this->_next->_prev = this->_prev;
        if(hasPrev()) this->_prev->_next = this->_next;
        this->_next = this->_prev = NULL;
        Discipline* __ref = _discipline;
        delete this;
        if(delete_ref){
            delete __ref;
            return NULL;
        }
        return __ref;
    }
};


typedef struct disciplines_linkedlist {
    private:
        struct discipline_node *_head;
        unsigned int _size;
        int _last_req = -1, _last_size = -1;
        struct discipline_node*  _last_node;

        // methods
        bool _remove(int node_i, Discipline** dcp_ref){
            struct discipline_node* node = this->node();
            if(node_i == 0){
                _head = _head->next();
            } else {
                int pos = 0;
                while(pos++ < node_i) node = node->next();
                if(node == NULL) return false;
            }
            *dcp_ref = (node->unlink_node(false));
            _size--;
            return true;
        }

    public:
        disciplines_linkedlist(Discipline** pdiscipline){
            _head = NULL;
            _size = 0;
            if(pdiscipline != NULL){
                struct discipline_node* H = new struct discipline_node(*pdiscipline);
                if(H){_head = H;}
                _size = 1;
            }
            _last_node = NULL;
            _last_req = -1;
        }

        Discipline* at(unsigned int index){
            if(_last_req > 2){
                if( _last_size != (signed)_size){
                    _last_size = (signed)_size;
                    _last_node = _head;
                    _last_req = 0;
                }
                else if((signed)index == _last_req){
                    return _last_node->discipline();
                }
                else if((signed)index == (_last_req+1)){
                    _last_req++;
                    _last_node = _last_node->next();
                    return _last_node->discipline();
                }
                else if((signed)index == (_last_req-1)){
                    _last_req--;
                    _last_node = _last_node->prev();
                    return _last_node->discipline();
                }
            }
            struct discipline_node* node = _head;
            unsigned int i = 0;
            while(node != NULL){
                if(i == index) break;
                node = node->next();
                i++;
            }
            if(node == NULL) return NULL;
            if(i > 2){
                _last_node = node;
                _last_req = i;
            }
            return node->discipline();
        }
        int indexOf(Discipline* discipline){
            struct discipline_node* node = this->node();
            int i;
            for(i = 0; i < (signed) this->size(); i++){
                if(node == NULL) return -1;
                else if( *(node->discipline()) && *discipline ) break;
                node = node->next();
            }
            return i;
        }
        bool insert(Discipline** discipline, int at_index = -1){
            if( (discipline == NULL) || (at_index < -1) || (at_index > (signed)_size) ){
                return false;  // (signed) is important!                 ^^^^^^^
            }
            struct discipline_node* it = this->node();
            struct discipline_node* newNode = new struct discipline_node(*discipline);
            if(_head == NULL){
                _head = newNode;
            } else if(at_index == 0){
                if(!it->push_over(&newNode)) return false;
                _head = newNode;
            } else if(at_index == -1 || (at_index == (signed)_size)){
                while(it->hasNext()) it = it->next();
                if(!it->push(&newNode)) return false;
            } else {
                int i = 0;
                while(it != NULL){
                    if(i++ == at_index) break;
                    it = it->next();
                }
                if(it == NULL) return false;
                if(!it->push_over(&newNode)) return false;
            }
            _size++;
            return true;
        }
        bool remove(const int index){
            if(index >= (signed)_size || index < 0) return false;
            bool del = false;
            Discipline* _ref = NULL;
            del = _remove(index, &_ref);
            delete &_ref;
            return del;
        }
        bool remove(const int index, Discipline** save_dcp_ref){
            if(index >= (signed)_size || index < 0) return false;
            bool del = false;
            del = _remove(index, save_dcp_ref);
            return del;
        }
        unsigned size(void){
            return _size;
        }
        bool empty(void){
            return (_size == 0 || _head == NULL);
        }
        struct discipline_node* node(void){
            return this->_head;
        }


} DisciplineList;

typedef struct discipline_iterator{
    protected:
    struct discipline_node* _pointer = nullptr;
    struct discipline_node* _reset = nullptr;
    public:
    discipline_iterator(DisciplineList* disciplinesLinkedList){
        _pointer = disciplinesLinkedList->node();
        _reset = _pointer;
    }
    struct discipline_node* begin(void){
        struct discipline_node* it = _pointer;
        while(it->hasPrev()) it = it->prev();
        return it;
    }
    struct discipline_node* end(void){
        struct discipline_node* it = _pointer;
        while(it->hasNext()) it = it->prev();
        return it;
    }
    struct discipline_node* nexti(unsigned int __n){
        struct discipline_node* it = _pointer;
        while(__n-- != 0) it = it->next();
        return _pointer = it;
    }
    struct discipline_node* next(){
        return _pointer = _pointer->next();
    }
    struct discipline_node* prev(){
        return _pointer = _pointer->prev();
    }
    struct discipline_node* data(){
        return _pointer;
    }
    class Discipline* data_discipline(){
        return _pointer->discipline();
    }
    void reset(void){
        _pointer = _reset;
    }
    bool is_valid(void){
        return (_pointer == NULL) ? false : true;
    }
} DisciplinesIterator;

typedef struct discipline_node SubjectNode;

bool validateTitle(std::string, bool = false);
void make_uppercase(std::string& str);
void make_uppercase(char* str);
#endif // DISCIPLINE_H
