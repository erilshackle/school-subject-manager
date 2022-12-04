#include "course.h"
#include "subjectsmanager.h"

#include <cstring>
#include <iomanip>
#include <exception>
#include <stdexcept>
#include <cstdio>
#include <conio.h>



using namespace std;

Course::Course(std::string course, short int lective, typeSemester semester)
{
   string fname;

    // uniform string
    if(course.length() > 5) capitalize(course);
    else make_uppercase(course);

    // defaults
    this->_semester = (lective*2)-(semester%2);
    this->_lective = _semester%2 == 0 ? (int)(semester/2) : 1+( (int) semester/2);
    this->setCourse(course);    // call methods cannot work sometimes, but this works
    this->_credits = 0;

    strcpy(this->_datafile, "course0.ssm");
    strcpy(this->_dcpdatalist, "dcplist0.ini");

    // STUDENT & COURSE DISCIPLINES
    this->_student = NULL;
    this->_disciplines = new DisciplineList(NULL);
    this->_archived_disciplines = new DisciplineList(NULL);
    _numOfDisciplines = _numOfArchived = 0;
}

class Student& Course::student(void)
{
    return *(this->_student);
}
class Discipline* Course::discipline(const unsigned int at)
{
    return _disciplines->at(at);
}
class Discipline* Course::archived(unsigned int idx)
{
    if(_archived_disciplines == NULL) return NULL;
    return _archived_disciplines->at(idx);
}

// Student Methods
bool Course::hasStudentRegistered(void)
{
    return (_student != NULL);
}
bool Course::registerStudent(class Student& newStudent)
{
    if(hasStudentRegistered()) return false;
    this->_student = &newStudent;
    return true;
}
bool Course::unregisterStudent(class Student& regStudent)
{
    if(!hasStudentRegistered()) return false;
    this->_student = NULL;
    return true;
}


// Discipline methods
Discipline* Course::getDiscipline(const std::string subject_title)
{
    string _search = subject_title;
    make_uppercase(_search);
    SubjectNode* sbj = _disciplines->node();
    while( sbj != NULL){
        if( sbj->discipline()->titles(_search) ) return sbj->discipline();
        sbj = sbj->next();
    }
    return NULL;
}
bool Course::add_discipline(std::string title, std::string professor)
{
    if(validateTitle(title, true) == false) return false;
    class Discipline *dcp = new Discipline(this, title,professor,this->_semester);
    if(!dcp) return false;
    return add_discipline(dcp);;
}
bool Course::add_discipline(class Discipline* newDiscipline)
{
    if(newDiscipline == nullptr) return false;
    else if(getDiscipline(newDiscipline->getTitle()) != NULL ){
        cout << "Error: "<< newDiscipline->getTitle() << " already EXISTS." << endl;
        return false;
    }else if(getDisciplineArchived(newDiscipline->getTitle())){
        cout << "Error: "<< newDiscipline->getTitle() << " is an ARCHIVED discipline!" << endl;
        return false;
    }
    if(_disciplines->empty()){
        _disciplines->insert(&newDiscipline, 0);
    } else {
        SubjectNode* sbj = _disciplines->node();
        int i = 0;
        while( sbj != NULL){
            if( *(sbj->discipline()) > *newDiscipline) break;
            sbj = sbj->next();
            i++;
        }
        if(!_disciplines->insert(&newDiscipline, i)) return false;
    }
    _numOfDisciplines++;
    return true;
}
bool Course::del_discipline(std::string subject_title, const typeDiscipline typeofSbj)
{
    Discipline* dscp = getDiscipline(subject_title);
    if(dscp == NULL) return false;
    if(typeofSbj == disciplines_registered)
        return del_disciplineRegistered(dscp);
    else
        return del_disciplineArchived(dscp);
}
bool Course::del_disciplineRegistered(Discipline* subject)
{
    int idx = _disciplines->indexOf(subject);
    if(idx == -1) return false;
    subject->DeleteData();
    if(!_disciplines->remove(idx)) return false;
    _numOfDisciplines--;

    return true;
}
bool Course::hasDiscipline()
{
    return (getNumDisciplines() == 0) ? false : true;
}
// archived
Discipline* Course::getDisciplineArchived(const std::string subject_title)
{
    if(_archived_disciplines == NULL) return NULL;
    string _search = subject_title;
    make_uppercase(_search);
    discipline_node* sbj = _archived_disciplines->node();
    while( sbj != NULL){
        if( sbj->discipline()->titles(_search) ) return sbj->discipline();
        sbj = sbj->next();
    }
    return NULL;
}
bool Course::isArchivedDiscipline(Discipline* subject)
{
    if(subject == NULL) return false;
    if(_archived_disciplines == NULL) return false;
    int i = 0;
    while( this->archived(i) != NULL){
        if(this->archived(i) == subject) return true;
        i++;
    }
    return false;
}
bool Course::archive_discipline(std::string subject_title)
{
    Discipline* dscp = getDiscipline(subject_title);
    if(dscp == NULL) return false;
    return archive_discipline(dscp);
}
bool Course::archive_discipline(Discipline* subject)
{
    //! CRASHING, quick solution of trust
    if(subject == NULL || subject->isArchived(false))
        return false;
    int id = _disciplines->indexOf(subject);
    if(id == -1) return false;
    Discipline* a = NULL;
    if(!_disciplines->remove(id, &a)) return false;
    _numOfDisciplines--;
    a->setSubjectArchived(this);
    // add to archived
    return _add_archived(subject);
}
bool /*private*/ Course::_add_archived(class Discipline* arch)
{
    _archived_disciplines->insert(&arch, 0);
    _numOfArchived++;
    return true;
}
bool Course::del_disciplineArchived(Discipline* asubject)
{
    int idx = _archived_disciplines->indexOf(asubject);
    if(idx == -1) return false;
    asubject->DeleteData();
    if(!_archived_disciplines->remove(idx)) return false;
    _numOfArchived--;
    return true;
}
bool Course::hasDisciplineArchived()
{
    return (getNumDisciplinesArchived() == 0) ? false : true;
}


// getter

std::string Course::getCourse()
{
    return _coursename;
}
std::string Course::getCourseAcronym(void)
{
    return _acronym;
}
unsigned int Course::getSemester(bool current_year)
{
    return current_year ? (2 - (_semester % 2)) : _semester;
}
unsigned int Course::getLective()
{
    return _lective;
}
unsigned int Course::getCredits()
{
    return _credits;
}


// setter

bool Course::setCourse(std::string course_name)
{
    if(course_name.length() > MAXCOURSESTR || course_name.empty())
        return false;
    capitalize(course_name);
    strcpy(this->_coursename, course_name.c_str());
    if(course_name.length() <= 5) setCourseAcronym(course_name);
    else setCourseAcronym("");
    return true;
}
bool Course::setCourseAcronym(std::string newAcronym)
{
    if(newAcronym.length() > 5) return false;
    if(newAcronym.empty()){
        string full = _coursename;
        newAcronym = full.front();
        unsigned int len = full.length();
        for(unsigned i = 1 ; i < len; i++)
            if( full[i] == ' ') {
                i++;
                // check the next 3 chars if they are alpha
                if( isalpha(full[i+1]) && isalpha(full[i+2]) && isalpha(full[i+3]) ){
                    newAcronym += full.at(i);
                    if(newAcronym.length() == 5) break;
                }
            }
    }
    if( !isalpha(newAcronym.front())) return false;
    convertToCase(newAcronym, upper);
    strcpy(this->_acronym, newAcronym.c_str());
    return true;
}
bool Course::setSemester(unsigned short int semester)
{
    if(semester < 1 || semester > 12) return false;
    this->_semester = semester;
    this->_lective = semester%2 == 0 ? (int)(semester/2) : 1+( (int) semester/2);
    return true;
}
bool Course::setLective(unsigned short int lective)
{
    if(lective < 1 || lective > 12) return false;
    this->_lective = lective;
    return true;
}

/* Disciplines Getters & Setters */

unsigned int Course::getIndexOf(Discipline* dscp)
{
    SubjectNode* sbj = _disciplines->node();
    for(int i = 0; i < (signed)getNumDisciplines(); i++){
        if(*(sbj->discipline()) == *dscp) return i;
        sbj = sbj->next();
    }
    return -1;
}
unsigned int Course::getIndexOf(std::string dscp_title)
{
    Discipline* d = getDiscipline(dscp_title);
    return getIndexOf(d);
}
unsigned int Course::getNumDisciplines()
{
    //return numOfDisciplines;
    return _disciplines->size();
}
unsigned int Course::getNumDisciplinesArchived()
{
    //return numOfArchived;
    return _archived_disciplines->size();
}


// assessment methods
int Course::getNumOfEvaluation()
{
    int eval = 0;
    for(int i = 0; i < (signed)getNumDisciplines(); i++){
        eval += discipline(i)->getNumOfEvals();
    }
    return eval;
}
int Course::getNumOfEvalOfType(EvaluationType type_of_eval)
{

    int evaltype = 0;
    for(int i = 0; i < (signed)getNumDisciplines(); i++){
        evaltype += discipline(i)->getNumOfEvals(type_of_eval);
    }
    return evaltype;
}
int Course::getNumOfExams()
{
    int exams = 0;
    for(int i = 0; i < (signed)getNumDisciplines(); i++){
        exams += (discipline(i)->hasExamScheduled() ? 1 : 0);
    }
    return exams;
}
double Course::getGradeMean()
{
    if(getNumDisciplines() == 0) return 0;
    double fmean = 0.00000;
    for(int i = 0; i < (signed)getNumDisciplines(); i++){
        fmean += discipline(i)->getEvalsGradeAverage();
    }
    return fmean/(getNumDisciplines()*1.0);
}
std::string Course::getNextEvaluationEvent()
{
    SubjectNode* sbjNode = _disciplines->node();
    Discipline *sbj = NULL, *nextSbj = NULL;
    Evaluation *eval = NULL, *nextEval;
    time_t time1 = 0, time2 = 0;

    while(sbjNode != NULL){
        if( (sbj = sbjNode->discipline()) != NULL){
            for(int k = 0; k < sbj->getNumOfEvals(); k++){
                eval = sbj->evals(k);
                if(!eval->isDoable()){
                    // check next
                    time2 = eval->getDateTime();
                    if(time2 < time1 || time1 == 0){
                        time1 = time2;
                        nextEval = eval;
                        nextSbj = sbj;
                    }
                }
            }
        }
        sbjNode = sbjNode->next();
    }
    if(nextSbj == NULL) return "Sem eventos proximos.";
    return ( "("+nextSbj->getTitle()+") " + nextEval->detail() );
}


/* Save and Load Methods */
bool Course::Save()
{
    /* *waring* you should prevent multiples files for saving, restrict student to set code multiple time
     or try to delete old files after set new code... */

    // course data file
    string code = (hasStudentRegistered()) ? _student->getCode() : "0";

    char fname[2][30];
    sprintf(fname[0], "course%s.ssm",code.c_str());
    strcpy(this->_datafile, fname[0]);
    // disciplines list file
    sprintf(fname[1], "dcplist%s.ini",code.c_str());
    strcpy(this->_dcpdatalist, fname[1]);

    int nsbj = _numOfDisciplines, nasbj = _numOfArchived;   // hold important values
    Student *stud = _student;                               // hold important values

    char fpath[50];
    sprintf(fpath, "%s\\%s",SSM_USER, _datafile);

    /* unset important values */
    _numOfDisciplines =  _numOfArchived = 0;
    _student = NULL;

    ofstream file(fpath, ios::trunc | ios::binary);
    if(file.fail()) return false;
    file.write((char*)this, sizeof(*this));
    file.close();

    /* reset important variables */
    _numOfDisciplines = nsbj;
    _numOfArchived = nasbj;
    _student = stud;

    return true;
}
bool Course::Load()
{
    char fname[2][30];
    sprintf(fname[0], "course%s.ssm",_student->getCode().c_str());
    sprintf(fname[1], "dcplist%s.ini",_student->getCode().c_str());
    strcpy(this->_datafile, fname[0]);
    strcpy(this->_dcpdatalist, fname[1]);

    // hold important references
    Student* user = _student;
    DisciplineList* sbj = _disciplines;
    DisciplineList* asbj = _archived_disciplines;

    char fpath[50];
    sprintf(fpath, "%s\\%s",SSM_USER, _datafile);

    ifstream file(fpath, ios::binary);
    if(!file.is_open()) return false;
    file.read((char*)this, sizeof(*this));
    file.close();

    // reset important references
    _student = user;
    _disciplines = sbj;
    _archived_disciplines = asbj;

    return true;
}
bool Course::DeleteData()
{
    string delfile = SSM_USER;
    delfile += this->_datafile;
    if(remove(delfile.c_str()) != 0)
        return false;
    return true;
}
bool Course::SaveDisciplines(void)
{
    SubjectNode* sbj = _disciplines->node();
    SubjectNode* asbj = _archived_disciplines->node();
    while(sbj != NULL){
        sbj->discipline()->SaveData();
        sbj = sbj->next();
    }
    while(asbj != NULL){
        asbj->discipline()->SaveData();
        asbj = asbj->next();
    }
    _updateDisciplineListFile();
    return true;
}
bool Course::LoadDisciplines(void)
{
    string line="", loadtype;
    char fpath[50];
    sprintf(fpath, "%s\\%s",SSM_USER, _dcpdatalist);

    std::ifstream datalist(fpath, std::ios::in);
    if(!datalist.is_open()) return false;


    // load
    while(getline(datalist, line)){
        if(line.front() == '#') continue;
        else if(line.front() == '@'){
                line.erase(line.begin());
                loadtype = line;
                cout << "loading " << loadtype << endl;
                continue;
        }
        Discipline* dscp = new Discipline(NULL,"???","???",1);
        if(!dscp->LoadData(line)){
            continue;
        }
        // load type
        if(loadtype == loaddscptype[0] /* registered */){
            this->add_discipline(dscp);
        } else
        if(loadtype == loaddscptype[1] /* archived */ ){
            this->_add_archived(dscp);
        }
        perror(line.c_str());
    }
    datalist.close();
    return true;
}

// private functions
void Course::_calc_credits(void)
{return;}
/*
void Course::_deleteStudent(int index)
{
    int i = index;
    for(i = index; i < (_numOfStudents-1); i++){
        _student[i] = _student[i+1];
    }
    _student[i] = NULL;
    _numOfStudents--;
}
*/
void Course::_updateDisciplineListFile()
{
    SubjectNode* sbj = NULL;
    char fname[50];
    sprintf(fname, "%s\\%s",SSM_USER, this->_dcpdatalist);

    std::ofstream dcplist(fname, std::ios::trunc | std::ios::out);
    dcplist.flush();
    dcplist << "# Do not touch this file\n";
    dcplist << "@" << loaddscptype[0] << "\n";
    sbj = _disciplines->node();
    while(sbj != NULL){
        dcplist << sbj->discipline()->getDatafileStr() << std::endl;
        sbj = sbj->next();
    }
    dcplist << "@" << loaddscptype[1] << "\n";
    sbj = _archived_disciplines->node();
    while(sbj != NULL){
        dcplist << sbj->discipline()->getDatafileStr() << std::endl;
        sbj = sbj->next();
    }
    dcplist.close();
}

/// FUNTIONS OUTSIDE

void CourseSaveDataOnThread(void* course_ref){
//    string cmd = "del /s/f ";
//    cmd += SSM_SUBJ + " >nul";
//    system(cmd);
    Course* c = (Course*)(course_ref);
    c->Save();
    c->SaveDisciplines();
    return;
}

void printDiscilineListOf(Course& myCourse, bool _archived, int start_from, int limit)
{
    Discipline* d = NULL;
    unsigned int sz = _archived ? myCourse.getNumDisciplinesArchived() : myCourse.getNumDisciplines();
    start_from = (start_from < (signed)sz && start_from > 0) ? start_from : 0;
    limit = (limit > 0 && limit > start_from && limit < (signed)sz ) ? limit : (signed)sz;
    std::cout << std::left;
    for(int i = start_from; i < limit; i++){
        d = _archived ? myCourse.archived(i) : myCourse.discipline(i);
        if(_archived==false)
            std::cout
                << "   "
                << std::setw(MAXTITLESTR) << d->getTitle() << " - "
                << std::setw(MAXNAMESTR) << d->getProfessor() << " (" << std::right
                << std::setw(3) << d->getEvalsStatus() << "% avaliado) "
                << (d->hasExamScheduled() ? "EXM" : " ")
                << std::left << std::endl;
        else
            std::cout
            << "   "
            << std::setw(MAXTITLESTR) << d->getTitle() << " - "
            << std::setw(MAXNAMESTR) << d->getProfessor() << " (Media Final = " << std::right
            << std::setprecision(1) << d->getEvalsGradeAverage() << ")"
            << std::left << std::endl;
    }
    if(sz == 0) std::cout << "   (X) Sem Disciplinas " << (_archived ? "Arquivadas.":"Registradas.")<<'\n';
}

int subjectsSelectorIndex(Course* course)
{
   std::string in;
    std::cin.clear();
    getline(std::cin, in);
    return course->getIndexOf(in);
}
Discipline* subjectsSelector(Course* course)
{
    std::string in;
    std::cin.clear();
    getline(std::cin, in);
    return course->getDiscipline(in);
}


