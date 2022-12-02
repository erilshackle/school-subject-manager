#include "student.h"
#include "subjectsmanager.h"

#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <ctime>


Student::Student(std::string studentName, class Course& studentCourse, unsigned long int studentCode)
{
    std::string fname;

    _myCourse = NULL;
    strcpy(this->_name,studentName.c_str());
    sprintf(this->_code, "%ld", studentCode);
    strcpy(this->_code,std::to_string(studentCode).c_str());
    _registered = false;
    // defaults
    time_t now = time(0);
    tm* bd = localtime(&now);
    _birthdate[0] = bd->tm_mday;
    _birthdate[1] = bd->tm_mon+1;
    _birthdate[2] = (bd->tm_year+1900)-18;
    _finalGrade = 0.0;

    // data file
    fname = SSM_USER;
    fname += STUDENT0_DATAFILE;
    std::ifstream fstud(fname, std::ios::binary);
    if(fstud.is_open()){
        fname.clear();
        fname = "student" + std::to_string(studentCode) + ".ssm";
        strcpy(this->_datafile,fname.c_str());
        fstud.close();
    }
    else{
        strcpy(this->_datafile,STUDENT0_DATAFILE);
//        Save();
    }
    // STUDENT COURSE
    _myCourse = &studentCourse;             // aggregation
    studentCourse.registerStudent(*this);   // aggragation
}
Student::~Student()
{
    delete _myCourse;
}

class Course& Student::course(void)
{
    return *_myCourse;
};


// getters
std::string Student::getName()
{
    return _name;
}
std::string Student::getCode()
{
    return _code;
}
unsigned int Student::getAge() /// *!*
{
    struct tm bd = {0};
    bd.tm_year = _birthdate[2]-1900;
    bd.tm_mon = _birthdate[1]-1;
    bd.tm_wday = _birthdate[0];
    time_t birthtime = mktime(&bd);
    long int _age = (difftime(time(0), birthtime) + 86400L/2) / 86400L;
    return _age /= 365;
}
std::string Student::getBirthdate()
{
    std::string strBD, d, m, y;
    d = std::to_string(_birthdate[0]);
    m = std::to_string(_birthdate[1]);
    y = std::to_string(_birthdate[2]);
    strBD = d + "-" + m + "-" + y ;
    return strBD;
}
float Student::getFinalGrade()
{
    return _finalGrade;
}
bool Student::isRegistered()
{
    return _registered;
}

// setters
bool Student::setName(std::string newName)
{
    if(!validateName(newName, true)){
        return false;
    }
    capitalize(newName);
    strcpy(this->_name, newName.c_str());
    return true;
}
bool Student::setAge(int dd_mm_yy[3])
{
    int day = dd_mm_yy[0], month = dd_mm_yy[1], year = dd_mm_yy[2];
    if( (day*month*year) <= 0 ) return false;
    else if(year > 80 && year < 100) dd_mm_yy[2] += 1900;
    else if(year < 1000) dd_mm_yy[2] += 2000;

    struct tm bd= {0};
    bd.tm_year = year-1900;
    bd.tm_mon = month-1;
    bd.tm_wday = day;

    time_t birthtime = mktime(&bd);

    long int _age = (difftime(time(0), birthtime) + 86400L/2) / 86400L;
    _age /= 365;

    if (_age <= 11 || _age > 51) return false;
    for(int i = 0; i < 3; i++) this->_birthdate[i] = dd_mm_yy[i];
    return true;
}
bool Student::setCode(long int newCode)
{
    if(newCode < 1 || newCode > 9999999) return false;
    sprintf(this->_code, "%0ld", newCode);
    return true;
}
int Student::Register(long int stud_CODE, std::string stud_name, std::string stud_courseName, unsigned int course_semester)
{
    if(stud_CODE < 1 || stud_CODE > 9999999) return 0;
    else if(!validateName(stud_name)) return -1;
    else if(stud_courseName.length() > MAXCOURSESTR) return -2;
    else if(course_semester < 1 || course_semester > 12) return -3;
    setCode(stud_CODE);
    setName(stud_name);
    _myCourse->setSemester(course_semester);
    _myCourse->setCourse(stud_courseName);
    _registered = true;
    return stud_CODE;
}

// Course Methods
bool Student::hasSubjects(void)
{
    return _myCourse->getNumDisciplines() == 0 ? false : true;
}
std::string Student::getCourse(bool info_about_levtive)
{
    std::string CT = _myCourse->getCourse();
    if(info_about_levtive){
        CT = _myCourse->getCourseAcronym();
        CT += " - " + std::to_string(_myCourse->getLective()) + "* ano";
        CT += " - " + std::to_string(_myCourse->getSemester(true)) + "* Semestre";
    }
    return CT;
}

// Subject Methods
bool Student::hasEvaluations(void)
{
    for(int i =0; i < (signed) _myCourse->getNumDisciplines(); i++)
        if(_myCourse->discipline(i)->getNumOfEvals() != 0) return true;
    return false;
}
void Student::getSubjectsList(bool _archived)
{
    Discipline* d = NULL;
    unsigned int sz = _archived ? _myCourse->getNumDisciplinesArchived() : _myCourse->getNumDisciplines();
    for(unsigned int i = 0; i < sz; i++){
        d = _archived ? _myCourse->archived(i) : _myCourse->discipline(i);
        std::cout
            << ' '<< std::setw(MAXTITLESTR) << d->getTitle() << " : "
            << std::setw(MAXNAMESTR+MAXTITLESTR-1) << d->getDescription()
            << std::right << std::setw(3) << d->getEvalsStatus() << "% avaliado"
            << std::left << std::endl;
    }
    if(sz == 0){
            std::cout << "   Lista Vazia: Nao possui disciplinas " << (_archived ? "arquivadas.":"registradas.")<<'\n';
    }
 }


void Student::_calc_finalGrade(void)
{

}

// Save & Load Methods

bool Student::Save(void)
{
    std::string fpath = SSM_USER;
    fpath += this->_datafile;
    Course *c = _myCourse;

    std::ofstream file(fpath, std::ios::trunc | std::ios::binary);
    if(!file.is_open()) return false;
    _myCourse = NULL;
    file.write((char*)this, sizeof(*this));
    _myCourse = c;
    file.close();

    return true;
 }
bool Student::Load(void)
{

    Course *c = _myCourse;
    std::string fpath = SSM_USER;
    fpath += _datafile;

    std::ifstream file(fpath, std::ios::binary);
    if(!file.is_open()) return false;
    file.read((char*)this, sizeof(*this));
    _myCourse = c;
    file.close();

    return true;
 }
 bool Student::Load(std::ifstream& userdata)
{
    Course *c = _myCourse;

    if(!userdata.is_open()) return false;
    userdata.read((char*)this, sizeof(*this));
    _myCourse = c;
    userdata.close();

    return true;
}
bool Student::DeleteData()
{
    std::string delfile = SSM_USER;
    delfile += this->_datafile;
    if(remove(delfile.c_str()) != 0)
        return false;
    return true;
}
bool Student::LoadDefault(void)
{
    static bool isDefaultStudentLoaded = false;
    if(isDefaultStudentLoaded) return false;

    Course *c = _myCourse;
    std::string fpath = SSM_USER;
    fpath += STUDENT0_DATAFILE;

    std::ifstream file(fpath, std::ios::binary);
    if(!file.is_open()) return false;
    file.read((char*)this, sizeof(*this));
    _myCourse = c;
    file.close();

    isDefaultStudentLoaded = true;
    return true;
}

/** out class functions */

bool validateName(std::string Name, bool reportError)
{
    if(Name.length() >= (unsigned) MAXNAMESTR ) {
        if(reportError){
            std::cerr << "Erro: limite de caracters = " << MAXNAMESTR-1 << '\n';
            SLEEP(1000);
        } return false;
    }
    else {
        for(auto c: Name){
            if(!isalpha(c) && c != ' '){
                if(reportError){
                    std::cerr << "Erro: simbolos ou numeros nao sao permitidos (" <<c<<")\n";
                    SLEEP(1000);
                } return false;
            }
        }
    }
    return true;
}
void capitalize(std::string& str)
{
    str.front() = (char)toupper(str[0]);
    unsigned int len = str.length();
    for(unsigned i = 1 ; i < len; i++)
        if( str[i] == ' ') {
            i++;
            if(isalpha(str[i+1])) str.at(i) = toupper(str[i]);
        }
        else str.at(i) = tolower(str[i]);
}
