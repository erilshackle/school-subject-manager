#include "discipline.h"
#include "subjectsmanager.h"

#include <cstring>
#include <cmath>
#include <iomanip>

using namespace std;

Discipline::Discipline(class Course* course_s, string discipline_title, string professor, unsigned int semester)
{
    make_uppercase(discipline_title);
    convertToCase(professor, capital);

    this->_course = course_s;
    strcpy(this->_title,discipline_title.c_str());
    strcpy(this->_professor, professor.c_str());
    strcpy(this->_description, "");
//    strcpy(this->_evalfile, "");
    this->_semester = semester <= 0 ? 1 : semester;
    // defaults;
    _completed_ = false;
    __archived_ = false;
    _scheduling = 0;
    _num_of_evals = 0;
    _finalGrade = 0.0;
    _finalGradeRequired = (MAXMARKS/2.1);
    // evaluations
    _evals = new EvalsManager(this);//, &_num_of_evals, &_scheduling);

    // data
    string fname = this->_title;
    convertToCase(fname,case_type::lower);
    sprintf(_datafile, "%s.sbj", fname.c_str());

}

// getters
bool Discipline::titles(const std::string str)
{
    string title = str;
    return (this->getTitle().compare(title) == 0);
}
std::string Discipline::getTitle()
{
    return this->_title;
}
std::string Discipline::getDescription()
{
    return _description;
}
std::string Discipline::getProfessor()
{
    return _professor;
}
bool Discipline::isCompleted()
{
    _completed_ = (_evals->isFullScheduled() && isEvalsAllDone());
//    __archived_ = _course->isArchivedDiscipline(this);
    return _completed_;
}
bool Discipline::isArchived(bool refresh)
{
    if(refresh && _course != NULL){
        __archived_ = _course->isArchivedDiscipline(this);
    }
    return __archived_;
}
unsigned int Discipline::getSemester()
{
    return _semester;
}
float Discipline::getFinalGradeRequired(void)
{
    return _finalGradeRequired;
}
std::string Discipline::getStatus()
{
    string status = "0%";
    float grade = getEvalsGradeAverage(true);
    int progress = 0;
    if(!isCompleted()){
        if(getNumOfEvals() != 0){   // division by zero
//         progress = (getNumOfEvalsDone()/getNumOfEvals()) * getEvalsStatus();
         progress = ((getNumOfEvalsDone()/getNumOfEvals())*50) + (getEvalsStatus()/2);
        }
        status =  "Avaliando "+to_string(progress)+"%";
    } else
    if(grade >= getFinalGradeRequired()){
        status = "Passou!";
    } else {
        status = "Reprovado!";
    }
    return status;
}
std::string Discipline::getStatusProgress()
{
    string  status;
    bool    finished = (getEvalsStatus() == 100);
    double balance = 0;
    float current_mean=0, current_perc=0;
    // loop for calc the status balance
    for(int i = 0; i < getNumOfEvals(); i++){
        if(evals(i)->isDone()){
            current_perc += evals(i)->getPercentageInt();
            current_mean += evals(i)->getGradeWeighted();
        }
    }

    balance = current_mean - (BASE_GRADE-getFinalGradeRequired()) * ((current_perc)/100.00);

    if(finished && getEvalsGradeAverage() < getFinalGradeRequired() && (hasExamScheduled() && !isExamDone()))
    {
        status = "exame!";
    }
    else if(finished && isEvalsAllDone())
    {
        status = "completo!";
    }
    else if(getEvalsStatus() != 0 && balance >= 1){
        status = "bem";
    }
    else if(getEvalsStatus() != 0 && balance <= -1  ){
        status = "mal";
    }
    else if(getEvalsStatus() != 0 && balance < 0  ){
        status = "insuf";
    }
    else if(getEvalsStatus() != 0){
        status = "suf";
    } else {
        status = "-";
    }

    /*
    else if(getEvalsStatus() != 0 && balance >=  getFinalGradeRequired()*((current_perc)/100.00))
    {
        status = "muito bem";
    }
    else if(getEvalsStatus() != 0 && balance >= ( getFinalGradeRequired() * (0.6 + getFinalGradeRequired()/BASE_GRADE) ) )
    {
        status = "bem";
    }
    else if(getEvalsStatus() != 0 && balance >= ( getFinalGradeRequired() * (0.4 + getFinalGradeRequired()/BASE_GRADE) ) )
    {
        status = "normal";
    }
    else if(getEvalsStatus() != 0 && ( getFinalGradeRequired() * (0.2 + getFinalGradeRequired()/BASE_GRADE) ) )
    {
        status = "mal";
    }
    else if(getEvalsStatus() != 0 )
    {
        status = "muito mal";
    }
    else
    {
        status = "-";
    }
    */
    // passed failed recourse doing well doing bad
    return status;
}

// setters
bool Discipline::setDescription(std::string __fullname)
{
    if(__fullname.empty()) return false;
    capitalize(__fullname);
    if(__fullname.length() < MAXTITLESTR+MAXNAMESTR){
        strcpy(this->_description, __fullname.c_str());
        return true;
    }
    return false;
}
bool Discipline::setProfessor(std::string professor_name)
{
    if(professor_name.empty() || professor_name.length() >= MAXNAMESTR )
        return false;
    else if (!validateName(professor_name, true)) return false;
    convertToCase(professor_name, capital);
    strcpy(this->_professor, professor_name.c_str());
    return true;
}
bool Discipline::setSemester(unsigned int semester)
{
    if(semester < 1 || semester > 12) return false;
    _semester = semester;
    return true;
}
bool Discipline::setFinalGradeRequired(float min_required_grade)
{
    if(min_required_grade <= 0 || min_required_grade > MAXMARKS )
        return false;
    _finalGradeRequired = min_required_grade;
    return true;
}
bool Discipline::setSubjectArchived(Course* course) /** @return  mark this subject as archived */
{
    if(!isCompleted()) return false;
    else if(_course == course){
        __archived_ = true;
    }
    return __archived_;
}


//! Evaluation

class Evaluation* Discipline::evals(unsigned int index)
{
    //try{
    return _evals->getEval(index);
    //}catch(std::exception& ex){}
}
//struct EvaluationManager* Discipline::evals(){return _evals;}
Evaluation* Discipline::getEvalNextEvent()
{
    return _evals->getEventNext();
}
int Discipline::getNumOfEvals()
{
    return _evals->getNumOfEvals();
}
int Discipline::getNumOfEvalsDone()
{
    return _evals->getNumOfEvalsDone();
}
int Discipline::getNumOfEvals(EvaluationType type)
{
    int num = 0;
    for(int i=0;i<getNumOfEvals();i++){
        num += (int) evals(i)->isTypeOf(type);
    }
    return num;
}
int Discipline::getEvalsStatus()
{
    return _evals->getScheduling();
}
float Discipline::getEvalsGradeAverage(bool refresh)
{
    if(refresh){
        _finalGrade = _evals->getGradeAverage();
    }
    return _finalGrade;
}
time_t Discipline::getEvalNextEventTime()
{
    return _evals->getEventNextTime();
}
bool Discipline::hasEvalScheduled()
{
    return _evals->hasScheduled();
}
bool Discipline::hasEvalScheduled(eval_t type)
{
    return _evals->hasScheduled(type);
}
bool Discipline::hasExamScheduled()
{
    return _evals->isExamScheduled();
}
bool Discipline::isExamDone()
{
    if(!hasExamScheduled()) return false;
    return _evals->getExam()->isDone();
}
bool Discipline::isEvalsAllDone()
{
    return (getNumOfEvals()==getNumOfEvalsDone() && getNumOfEvals()!=0);
}
bool Discipline::isEvalsFullScheduled()
{
    return _evals->isFullScheduled();
}
bool Discipline::isEvalSchedulable(EvaluationType type)
{
    return (_evals->isSchedulable((eval_t)type));
}
void Discipline::printEvalsList()
{
    if(!hasEvalScheduled()){
        std::cout << "   Lista vazia: Nenhuma avaliacao agendada\n";
    }
    else {
        for(int i = 0; i < getNumOfEvals(); i++){
            Evaluation* eval = _evals->getEval(i);

            // printing
            std::cout << std::left
                << "   " << std::setw(17) << getEvalName(eval->getType())
                << " >> " << std::setw(14) << eval->getDateString()
                << "  (" << eval->getPercentageInt() << "%) " << std::setw(3);
            if(eval->isDone()){
                std::cout << "nota " << std::right << std::setw(5) << std::setprecision(2) << eval->getGrade();
            }else if(eval->isDoable()){
                std::cout << "realizado";
            } else {
                std::cout << "pendente";
            }
            std::cout.put('\n');
        }
    }
}
void Discipline::printEvalsBrief()
{
    Evaluation* nxeval = getEvalNextEvent();
     /*! 1st row - evaluations scheduled */
    try{
        cout << "Avaliacoes:     ";
        cout << getEvalName(Test, true )        << "("<< getNumOfEvals(Test)                  <<") + "
             << getEvalName(Assignment, true )  << "("<< getNumOfEvals(Assignment)            <<") + "
             << getEvalName(Project, true )     << "("<< getNumOfEvals(Project)               <<") + "
             << getEvalName(Others, true )      << "("<< (hasEvalScheduled(Others) ? "S":"N" )<<") + "
             << getEvalName(Exam, true )        << "("<< (hasExamScheduled() ? "S":"N" )      <<")\n";

        /*! 2nd row - evaluation schedule status, evaluations done, to be done, current FinalGrade */

        cout << "Estatistica:    "
            << getEvalsStatus() << "% agendado" << "  -  "
            << getNumOfEvalsDone() << " feitas & "
            << getNumOfEvals()-getNumOfEvalsDone() << " pendentes"
        << endl;
        /*!
        *   3rd row - next evaluation to do
        */
        //cout   << "Resultados:     " << "progresso: " << getStatusProgress() << "; Media Final Atual: " << setprecision(0)<< getEvalsGradeAverage() <<endl;
        cout << "Proximo Evento: ";
        if(nxeval!=NULL){
            std::cout << nxeval->detail(false) << "\b, " << getTimeLeftTo(nxeval->getDateTime()) << std::endl;
        } else {
            std::cout << "Sem eventos proximos." << std::endl;
        }
    }catch(std::exception& ex){}
    return;
}

bool Discipline::scheduleEvaluation(const EvaluationType type, float perc, int date[3])
{
    bool status = _evals->schedule(type,perc,date);
    //_evals->refresh__();
    return status;
}
bool Discipline::removeScheduledEvaluation(int index)
{
    bool status = _evals->removeScheduled(index);
    _evals->refresh__();
    return status;
}
bool Discipline::gradeEvaluation(int index, float grade)
{
    bool status = _evals->evaluate(index,grade);
    _evals->refresh__();
    return status;
}
void Discipline::refresh(void)
{
    this->_evals->refresh__();
}



// operators method
bool Discipline::equal(const std::string discpline_title)
{
    string title = discpline_title;
    make_uppercase(title);
    return this->titles(title);
}
bool Discipline::operator < (Discipline& dscp)
{
    return (this->getTitle() < dscp.getTitle());
}
bool Discipline::operator > (Discipline& dscp)
{
    return (this->getTitle() > dscp.getTitle()) ;
}
bool Discipline::operator == (Discipline& dscp)
{
    return (dscp.getTitle() == this->getTitle());
}
bool Discipline::operator && (Discipline& dscp)
{
    return this == &dscp ? true : false;
}

// data Save and Load

int Discipline::SaveData()
{
    int ret = 0;
    Course *hold_course = this->_course;
    EvalsManager *hold_evals = this->_evals;

    char filepath[50];
    sprintf(filepath, "%s%s", SSM_SUBJ, this->_datafile);

    ofstream dcpfile(filepath, ios::binary | ios::trunc);
    if(!dcpfile.fail()){
        _course = NULL;
        dcpfile.write((char*)this, sizeof(*this));
        ret++;
        dcpfile.close();
        if(SaveEvaluations()){
            ret++;
        }
        _course = hold_course;
        _evals = hold_evals;
    }
    return ret;
}
int Discipline::LoadData(std::string filename)
{
    int ret=0;
    char filepath[50];
    sprintf(filepath, "%s%s", SSM_SUBJ,
                ((filename.empty()) ? _datafile : filename.c_str()));

    EvalsManager* hold_eval = _evals;
    Course* hold_course = _course;

    ifstream dcpfile(filepath, ios::binary);
    if(dcpfile.is_open()){
        dcpfile.read((char*)this, sizeof(Discipline));
        //dcpfile.read((char*)_evals, sizeof(EvalsManager));
        dcpfile.close();
    }
    _course = hold_course;
    _evals = hold_eval;
    ret += (int) LoadEvaluations();
    return ret;
}
bool Discipline::SaveEvaluations()
{
    std::string fpath = SSM_EVAL;
    fpath += this->getTitle();
    convertToCase(fpath, lower);
    return _evals->saveData(fpath);
}
bool Discipline::LoadEvaluations()
{
    bool loaded;
    std::string fpath = SSM_EVAL;
    fpath += this->getTitle();
    convertToCase(fpath, lower);
    loaded = _evals->loadData(fpath);
    return loaded;
}
bool Discipline::DeleteData()
{
    string delfile = SSM_SUBJ;
    delfile += this->_datafile;

    if(remove((delfile).c_str()) == 0){
        delfile = SSM_EVAL;
        delfile += this->_datafile;
        remove((delfile+"evl").c_str());
    } else return false;
    return true;
}
std::string Discipline::getDatafileStr(void)
{
    return _datafile;
}


// private methods

//void Discipline::_e_update_nextevent(int upt_index)
//{
//    _e_nexteval = -1;
//    int new_one = 0;
////    time_t old_time = _eval[_e_nexteval]->getDateTime();
////    time_t upt_time = _eval[upt_index]->getDateTime();
////    if(difftime(old_time, upt_time) > 0){
////        _e_nexteval = upt_index;
////    }
//    for(int i = 0; i < (signed)getNumEvaluations(); i++){
//        if(!_eval[i]->isDone()){
//            if(_eval[i]->getDateTimeLeft() <= _eval[new_one]->getDateTimeLeft())
//                _e_nexteval = new_one = i;
//        }
//    }
//}

/** ASSESMENTS  */



/** out class functions */

bool validateTitle(std::string Title, bool reportError)
{
    while(Title.back() == ' ') Title.pop_back();
    if(Title.length() >= MAXTITLESTR || Title.length() < 2){
        if(reportError){
            std::cerr << "Erro: numero de caracteres = minimo 2 e maximo " << MAXTITLESTR-1 << '\n';
            SLEEP(1000);
        } return false;
    }
    else if(isalpha(Title.front())){
        for(unsigned int c = 1; c < Title.length(); c++){
            if(!isalnum((char)Title[c])){
                if(reportError){
                    std::cerr << "Erro: caracter nao aceite ("<<Title[c]<<")\n";
                    SLEEP(1000);
                } return false;
            }
        }
    } else {
            if(reportError){
            std::cerr << "Erro: texto comeca por um caracter nao alfabetico ("<<Title[0]<<")\n";
            SLEEP(1000);
            } return false;
    }
    return true;
}
void make_uppercase(string& str)
{
    unsigned int len = str.length();
    for(unsigned int i = 0; i < len; i++)
        str[i] = (char) toupper(str.at(i));
    return;
}
void make_uppercase(char* str)
{
    int len = strlen(str);
    for(int i = 0; i < len; i++)
        str[i] = (char) toupper(str[i]);
    return;
}


#include "evaluation.h"
