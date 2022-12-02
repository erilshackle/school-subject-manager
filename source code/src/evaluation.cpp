#include "evaluation.h"

#include <conio.h>
#include <cstring>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <utils.h>

Evaluation::Evaluation(class Discipline* sbj_of)
{
    _subjects_of = sbj_of;
    _type = EvaluationType::Generic;
    _grade = _requiredGrade = 0.0;
    _percentage = 0;
    _done = false;
    strcpy(_description, "");
    for(int i = 0; i < 3; i++)
        _date[i] = 0;
    // code
    srand(time(NULL));
    int randcode = 1000+rand()%9999L;
    sprintf(__datacode, "%04d", randcode);
}

Evaluation::Evaluation(class Discipline* sbj_of, EvaluationType type_of_assessment, const unsigned int percentage, int date[3])
{
    //Evaluation(sbj_of);
    _subjects_of = sbj_of;
    _type = type_of_assessment;
    if(percentage > 0 && percentage <= 100)
        _percentage = percentage;
    strcpy(_description, "");
    if(validateDate(date, false)){
        for(int i = 0; i < 3; i++)
            _date[i] = date[i];
    }
    _grade = _requiredGrade = 0.0;
    _done = false;
    // code
    srand(time(NULL));
    int randcode = 1000+rand()%9999L;
    sprintf(__datacode, "%04d", randcode);
}


Evaluation& Evaluation::operator=(const EvaluationType eval)
{
    this->setType(eval);
    return *this;
}

bool Evaluation::hasSubjectOf()
{
    return (_subjects_of != NULL);
}

// methods
std::string Evaluation::detail(bool with_status)
{
    std::string brief, status;
    if(with_status == false){
        status = "\b\b ";   // removes comma
    } else if(isDone()){
        status = ("nota "+std::to_string(getGrade()));
    } else if(isDoable()){
        status = "realizado";
    } else {
        status = "pendente";
    }

    brief += getEvalName(getType()) + " (" + std::to_string(getPercentageInt())
            + "%) " + getDateString() + ", " + status;
    return brief;
}


// getters
EvaluationType Evaluation::getType()
{
    return _type;
}

std::string Evaluation::getDescription(){
    return _description;
}
float Evaluation::getRequiredGrade()
{
    return _requiredGrade;
}
float Evaluation::getPercentage()
{
    return (getPercentageInt()/100.0);
}
int Evaluation::getPercentageInt()
{
    return _percentage;
}
float Evaluation::getGrade()
{
    return _grade;
}
float Evaluation::getGradeWeighted()
{
    return getGrade()*getPercentage();
}
tm Evaluation::getDate()
{
    time_t date = getDateTime();
    tm dt = *localtime(&date);
    return dt;
}
time_t Evaluation::getDateTime()
{
    tm dt = {0,0,0,0,0,0,0,0,0};
    dt.tm_mday = _date[0];
    dt.tm_mon = _date[1]-1;
    dt.tm_year = _date[2]-1900;
    return mktime(&dt);
}
double Evaluation::getDateTimeLeft()
{
    if((_date[0]*_date[1]*_date[2]) == 0) return 0;
    return difftime(getDateTime(), time(0));
}
std::string Evaluation::getDateString()
{
    std::string date = "--/--/--";
    if(validateDate(_date,false)){
        time_t datetime = getDateTime();
        tm tmdate = *localtime(&datetime);
        date = std::to_string(tmdate.tm_mday)+
        "/"  + std::to_string(tmdate.tm_mon+1)+
        "/"  + std::to_string(tmdate.tm_year+1900);
    }
    return date;
}
bool Evaluation::isTypeOf(EvaluationType type)
{
    return (_type == type);
}
bool Evaluation::isExam()
{
    return (_type == EvaluationType::Exam);
}
const bool Evaluation::isDoable(){
    return (getDateTimeLeft() <= 0);
}
bool Evaluation::isDone()
{
    return _done;
}
std::string Evaluation::getTimeStatus(void)
{
    if(isDone()){
        return "avaliado";
    } else if(isDoable()){
        return "realizado";
    } else {
        return "pendente";
    }
}
std::string Evaluation::getResultStatus(void)
{
    if(isDone()){
        if(getGrade() >= getRequiredGrade()){
            return "arp. " + std::to_string(getGrade());
        } else {
            return "rep. "  + std::to_string(getGrade());;
        }
    } else {
        return "pendente";
    }
}

// setters
bool Evaluation::setType(const EvaluationType type)
{
    // subject rules
    if(hasSubjectOf())
        if(!(_subjects_of->isEvalSchedulable(type)))
            return false;

    if(type == Exam){
        _percentage = 100;
    }
    else if(type == Others){
        for(int i = 0; i < 3; i++)
            _date[i] = 0;
    }
    _type = type;
    return true;
}
bool Evaluation::setDescription(std::string& about)
{
    if(about.length() >= MAXEVALDESCRIPTION)
        return false;
    strcpy(_description, about.c_str());
    return true;
}
bool Evaluation::setGrade(const float grade)
{
    if(grade < 0 || grade > BASE_GRADE)
        return false;
    _grade = grade;
    setDone(true);
    return true;
}
bool Evaluation::setPercentage(const unsigned int percentage)
{
    if(isExam()) return false;
    if(percentage <= 0 || percentage > 100)
        return false;

    // subject rules
    if(hasSubjectOf())
        if((percentage + (_subjects_of->getEvalsStatus() - getPercentageInt()) ) > 100)
            return false;

    _percentage = percentage;
    return true;
}
bool Evaluation::setDate(int date[3])
{
    if(isTypeOf(Others)) return false;
    if(!validateDate(date, true))
        return false;
    for(int i = 0; i < 3; i++)
        _date[i] = date[i];

    if(!isDoable()){
        setDone(false);
    }
    return true;
}
bool Evaluation::setRequiredGrade(const float required_grade)
{
    if(required_grade < 0 || required_grade > BASE_GRADE)
        return false;
    _requiredGrade = required_grade;
    return true;
}
void Evaluation::setDone(const bool done)
{
    if(done == false){
        _grade = 0.00;
    }
    _done = done;
}

// changed

bool Evaluation::getChangedStatus(void)
{
    if(_changed){
        _changed = !_changed;
        return true;
    }
    return _changed;
}

// datafile
bool Evaluation::loadData(std::string filepath)
{
    if(filepath.empty()) return false;
    std::ifstream filedata(filepath, std::ios::binary);
    if(!filedata.is_open()) return false;
    Discipline* this_sbj = _subjects_of;
    if(!filedata.read((char*)this, sizeof(class Evaluation))){
        return false;
    }
    _subjects_of = this_sbj;
    return true;
    /*
    if(!datafile.is_open() || datafile.eof())
        return false;
    Discipline* this_sbj = _subjects_of;
    if(!datafile.read((char*)this, sizeof(class Evaluation))){
        return false;
    }
    _subjects_of = this_sbj;
    return true;
    */
}
bool Evaluation::saveData(std::string filepath)
{
    if(filepath.empty()) return false;
    std::ofstream filedata(filepath, std::ios::trunc|std::ios::binary);
    if(filedata.fail()) return false;
    Discipline* this_sbj = _subjects_of;
    _subjects_of = NULL;
    filedata.write((char*)this, sizeof(class Evaluation));
    _subjects_of = this_sbj;
    return true;
    /* old
    if(datafile.fail()) return false;
    Discipline* this_sbj = _subjects_of;
    _subjects_of = NULL;
    datafile.write((char*)this, sizeof(class Evaluation));
    _subjects_of = this_sbj;
    return true;
    */
}

bool Evaluation::_checkDataCode(std::string key)
{
    std::string this_key = __datacode;
    return this_key == key;
}
std::string Evaluation::_getDataCode()
{
    return __datacode;
}

/**< Evaluations General Functions */

std::string getEvalName(EvaluationType evalType, bool shortname)
{
    switch(evalType)
    {
    case EvaluationType::Test:
        return shortname ? "TST" : "Teste";
    case EvaluationType::Assignment:
        return shortname ? "TRB" : "Trabalho";
    case EvaluationType::Project:
        return shortname ? "PRJ" : "Projecto";
    case EvaluationType::Others:
        return shortname ? "OEA" : "Outros Elementos";
    case EvaluationType::Exam:
        return shortname ? "EXM" : "Exame";
    default:
        return shortname ? "AVL" : "Avaliação";
    }
    return "";
}

//void printListOfAvaliableAssessmentsOf(Discipline& discipline)
//{
//    if(discipline.hasExamScheduled() || discipline.getEvalsStatus() == 100)
//    {
//        cout << "   ESTA DISCIPLINA POSSUI 100% DAS AVALIACOES AGENDADAS"<<endl;
//        return;
//    }
//    for(int i = 1; i <= 6; i++)
//    {
//        cout <<"   ";
//        if(discipline.isEvalAvaliable((AssessmentType)i))
//            mark(getEvalName((AssessmentType)i));
//        else
//             mark(getEvalName((AssessmentType)i), RED);
//        cout.put('\n');
//    }
//    return;
//}

bool validateDate(int* date, bool null)
{
    if(date == NULL) return null;
    if ( *(date+0) < 0 || *(date+1) < 0 || *(date+2) < 0)
    {
        return false;
    }
    if( (*(date+0) * *(date+1) * *(date+2)) == 0)
    {
        return null;
    }
    return true;
}

const std::string strWeek[] = {"Dom","Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};
const std::string strMonth[] = {"Jan","Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"};

std::string getTimeLeftTo(time_t datetime)
{
    std::string timeleft;
    if(datetime == -1 || !datetime){
        return " ";
    }
    tm* theDate = localtime(&datetime);
    double secs   = difftime(datetime, time(NULL));
    long int dd   = secs*1.0/(3600*24) + 1;
         int mn   = dd/30.0;
         int wk   = dd/7.0;
         int hh  = (secs/3600)/24;
         int yy  = dd/365.0;
//         int h24  = (secs/3600)%24 + 1;
//         int m60    = (int) ((secs/60)%60)+1;

    if(yy > 1){
        timeleft = "em " + strMonth[theDate->tm_mon] + " de " + std::to_string(theDate->tm_year+1900);
        //sprintf(timestr, "em %d meses", (int)(dd/30.0) );
    } else
    if(dd > 60){
        timeleft = "em " + std::to_string(mn) + " meses";
        //sprintf(timestr, "em %d meses", (int)(dd/30.0) );
    }
    else if(dd >= 14){
        timeleft = "em " + std::to_string(wk) + " semanas ("+strWeek[theDate->tm_wday]+")";
        //sprintf(timestr, "em %ld semanas (%s)", wk, _strWeek[theDate->tm_wday]);
    }
    else if(dd > 1){
        timeleft = "daqui a " + std::to_string(dd) + " dias";
        //sprintf(timestr, "daqui a %ld dias", dd);
    }
    else if(dd == 1){
        timeleft = "amanha";
        //strcpy(timestr, "amanha");
    }
    else if(dd == 0 && hh >= 0){
        timeleft = "hoje";
        //strcpy(timestr, "hoje");
    }
    else if(dd == 0 && hh < 0){
        timeleft = "ontem";
        //strcpy(timestr, "ontem");
    }
    else if(dd > -14){
        timeleft = "ha " + std::to_string(std::abs(dd-1)) + " dias";
        //sprintf(timestr,"ha %ld dias", abs(dd));
    }
    else if(dd >= -60){
        timeleft = "ha " + std::to_string(std::abs(wk)) + " semanas (" + strWeek[theDate->tm_wday] +")" ;
        //sprintf(timestr,"ha %ld semanas",abs(wk));
    }
    else if(dd > -195){
        timeleft = "ha " + std::to_string(std::abs(mn)) +  " meses";
        //sprintf(timestr,"ha %ld semanas",abs(wk));
    }
    else{
        timeleft = "desde " + strMonth[theDate->tm_mon] + " de " + std::to_string((theDate->tm_year+1900));
        //sprintf(timestr,"desde %s de %4d", _strMonth[theDate->tm_mon], theDate->tm_year+1900);
    }

    return timeleft;
}
