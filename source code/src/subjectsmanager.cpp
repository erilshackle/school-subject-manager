#include "subjectsmanager.h"
#include "../version.h"

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#else
#include <unistd.h>
#endif // _WIN32



#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <clocale>
#include <cstring>
#include <direct.h>

//extern int ATHEME = 0;

SSM* SSM_INIT(int argc, char **argv)
{
    SSM * config = new SM::SSM;

    std::cout << "Initializing configuration...\n";
    std::string v = "v";
    v+= std::to_string(AutoVersion::MAJOR)+"."+std::to_string(AutoVersion::MINOR)+"."+ std::to_string(AutoVersion::BUILD);
    strcpy(config->version, v.c_str());
    config->code = (10000*(atoi(AutoVersion::YEAR)%100))
                 + (100*atoi(AutoVersion::MONTH))
                 + (atoi(AutoVersion::DATE));
    strcpy(config->datafile, "ssmconfig.dat");
    strcpy(config->lc_language, "Portuguese");
    strcpy(config->lc_numeric, "English");
    config->atheme = 0;//ATHEME;
    int theme[MAXSMTHEMES][SSMTHCOLORS] = {
    //   MAIN   SECN    WARN        SCSS    ERRR    INVL    HINT    INPT    //colors constant, should change for unix
        {0x3F,  0x5F,   0x0E,       0x5A,   0x5E,   0x00,   0x57,   0x5B},  //  default 0
        {0x70,  0x8F,   0x0E,       0x8A,   0x8E,   0x00,   0x87,   0x8B},  //  light   1
        {0x0F,  0x07,   0x0E,       0x0A,   0x0C,   0x00,   0x08,   0x0B},  //  dark    2
    };
    for(int k = 0; k < MAXSMTHEMES*SSMTHCOLORS; k++) *(*(config->themes) +k) = *(*(theme)+k);

    SM::_RUN();
    SM::_LOAD(&config);
    SM::_APPLYCONFG(config);
    SSM_SAVE(config);

    // system calling
    if(argc == 2){
        if(strcmp(argv[1], "version")==0){
            std::cout <<"version="<< config->version <<" code="<<config->code <<std::endl;
        }
        else if(strcmp(argv[1], "update")==0){
            SM::_update_updater();
        }
        else if(strcmp(argv[1], "agenda")==0){
            std::cout << "no info..." << std::endl;
        }
        else{
            std::cerr << "cannot recognize '"<< argv[1] << "' as a " << argv[0] << "internal command." << std::endl;
        }
        exit(0);
    }
    else if(argc > 2){
        if(strcmp(argv[1], "notify")==0){
            if(strcmp(argv[2], "on")==0){
                std::cout << "You WILL BE notified about further events on each user login." << std::endl;
            } else if(strcmp(argv[2], "off")==0){
                std::cout << "You will NOT be notified about any further events." << std::endl;
            }
            exit(0);
        }
        std::cerr << "sintaxe: " << argv[0] << "[ update | notify [on|off] | agenda ] " << std::endl;
        exit(0);
    }

    return config;
}

bool SM::_LOAD(SSM* *CFG)
{
    SSM* ssmCheck = new SM::SSM;
    std::ifstream config( (*CFG)->datafile, std::ios::binary );
    if(!config.is_open()) return false;
    config.read((char*)ssmCheck, sizeof(SSM));
    if(ssmCheck->code == (*CFG)->code ){
        delete *CFG;
        *CFG = ssmCheck;
    } else {
        delete ssmCheck;
    }
    config.close();
    return true;
}

void SM::_RUN(void)
{
    char dir[300];

    if(DirectoryExists(SSM_ROOT)){
        sprintf(dir,"%s/", SSM_ROOT);
        std::clog << "Working Directory: SSMANAGER/" << std::endl;
    }else {
        sprintf(dir,"%s/%s/",getenv("appdata"), SSM_ROOT);
    }
    mkdir(dir);
    if(chdir(dir) != 0){
        perror("Working directory");
        Sleep(1000);
        abort();
    }
    else {  // create directories
        mkdir(SSM_USER);
        mkdir(SSM_SUBJ);
        mkdir(SSM_EVAL);
    }
    return;
}

void SM::_APPLYCONFG(SSM* CFG)
{
    setlocale(LC_ALL, CFG->lc_language );
    setlocale(LC_NUMERIC, CFG->lc_numeric);
    ssm_applyTheme(CFG, th_main, true);
}

bool SSM_SAVE(struct subjectmanager *CFG)
{
    std::ofstream config( CFG->datafile, std::ios::binary|std::ios::trunc);
    if(config.fail()) return false;
    config.write((char*)CFG, sizeof(struct subjectmanager));
    config.close();
    return true;
}

// user data list
bool user_list_loader(void)
{
    return false;
}
bool user_list_registry(class Student* user)
{
    if(user == NULL) return false;

    std::ofstream data(SSM_USERS_LIST, std::ios::out|std::ios::app);
    if(!data.is_open()) return false;
    /*code*/ data << user->getCode() << '\n';
    /*name*/ data << user->getName() << '\n';
    return true;
}
bool user_check_if_exist(char* code)
{
    return true;
}



bool SM::_login_as_(class Student** User, char* usercode, SSM* ssm)
{
    if (ssm == NULL) return false;
    std::string code, username, fname, ucode = usercode;
    bool logged = false;
    std::ifstream _list(SSM_USERS_LIST, std::ios::in);
    if(!_list.is_open()){
        perror("error while opening user list file");
        return false;
    }
    while(getline(_list, code)){        // get code
        getline(_list,username);        // then get username
        if(strcmp(usercode, code.c_str()) == 0){
            logged = true;
            break;
        }
    }
    if(!logged){
        std::cerr << "no user with code '" << usercode << "' registered!" << std::endl;
        return false;
    }
    fname = SSM_USER;
    fname += "student" + ucode + ".ssm";
    std::ifstream _file(fname, std::ios::binary);
    if(_file.is_open()){
        (*User)->Load(_file);
        (*User)->course().Load();
        (*User)->course().LoadDisciplines();
        std::cout << "user '"<< usercode << "' logged in successfully." << std::endl;
        _file.close();
    } else perror(usercode);
    return true;
}
bool SM::_registry_user_(std::string user, SSM* ssm)
{
    if (ssm == NULL) return false;
    std::ofstream _list(SSM_USERS_LIST, std::ios::app);
    if(!_list.is_open()){
        perror("error while opening user list file");
        return false;
    }
    chdir(SSM_USER);
    convertToCase(user, lower);
    if(mkdir(user.c_str()) != 0){
        perror("user registry error");
        return false;
    }
    chdir(user.c_str());
    SSM_SAVE(ssm);
    std::cout << "user '"<< user << "' logged in successfully." << std::endl;
    return true;
}
int SM::_update_updater()
{
    errno = 0;
    std::ifstream test(SSM_UPDATER);
    if(test.is_open()){
        test.close();
    } else {
        std::ofstream html(SSM_UPDATER, std::ios::trunc);
        html << UPDATER_HTML; // by include updater.html in header, exist a char[] constant in it named UPDATER_HTML
        html.close();
    }
    int ret = (int) ShellExecute(0, 0,SSM_UPDATER, 0, 0 , SW_SHOW );
    perror("updater");
    return ret;
}


void ssm_applyTheme(SSM* _ssm, int theme,  bool clearscreen)
{
    if(_ssm == NULL || theme > 0xFF) return;

    // apply the theme
    unsigned short th = _ssm->themes[_ssm->atheme][theme];
    WORD wAttributes = th;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);

    // clear screen
    if(clearscreen == true)
    {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD line, area;
        COORD homepoint = {0, 0};

        if (hStdOut == INVALID_HANDLE_VALUE) return;
        if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
        area = csbi.dwSize.X * csbi.dwSize.Y;
        if(!FillConsoleOutputCharacter(hStdOut,' ', area, homepoint, &line)) return;
        if(!FillConsoleOutputAttribute(hStdOut,csbi.wAttributes, area, homepoint, &line)) return;
        SetConsoleCursorPosition(hStdOut, homepoint);
    }
}

// Subjects Functions

int subjectsListing(Course* course, bool archived, int start, int stop)
{
    int elements = (!archived) ? course->getNumDisciplines() : course->getNumDisciplinesArchived();
    int _start = (elements > stop) ? stop : 0;
    printDiscilineListOf(*course, archived, _start, stop);
    if(start != 0) return elements+1;
    else return elements;
}
bool export_subject(Discipline* subject, int directory, bool is_archived)
{
    if(subject == NULL) return false;
    std::ofstream file;
    CHAR savefolder[MAX_PATH];
    SHGetFolderPath(NULL, directory, NULL, 0, savefolder);
    std::string save_path = savefolder;
    save_path += "\\";
    save_path += SSM_FOLDER;
    mkdir(save_path.c_str());
    save_path += subject->getTitle()+".txt";
    puts(save_path.c_str());
    file.open(save_path, std::ios::trunc|std::ios::out);
    if(!file.is_open()) return false;

    file << std::left << std::setprecision(1)
        <<std::setw(15) << "Disciplina:" << subject->getTitle() << " (" << subject->getDescription() << ")\n"
        <<std::setw(15) << "Professor:" << subject->getProfessor() << '\n'
        <<std::setw(15) << "Semestre:" << subject->getSemester() << "˚ semestre do curso" << '\n'
        <<std::setw(15) << "Avaliacoes: " << std::setw(4) <<subject->getNumOfEvals()
    <<std::endl;
    file << "------------------------------------------------------------\n";
    for(int i = 0; i < subject->getNumOfEvals(); i++){
        Evaluation* eval = subject->evals(i);
        file << std::left << std::fixed
             << " "<< std::setw(17) << getEvalName(eval->getType())
             << " >> " << std::setw(14) << eval->getDateString()
             << "  (" << eval->getPercentageInt() << "%) " << std::setw(3);
        file << "nota " << std::right << std::setw(5) << std::setprecision(2) <<std::fixed << eval->getGrade()
        << std::endl;
    }
    file << "------------------------------------------------------------\n";
    file << std::left << std::setprecision(1) << " Nota Final: "
         << std::setw(25) << subject->getEvalsGradeAverage()
         << " Estado: " << subject->getStatus() << '\n';
    file << "============================================================\n";
    file.close();
    return true;
}
bool export_agenda(Course* course, int directory)
{
    if(course == NULL) return false;
    std::ofstream file;
    CHAR savefolder[MAX_PATH];
    SHGetFolderPath(NULL, directory, NULL, 0, savefolder);
    std::string save_path = savefolder;
    save_path += "\\";
    save_path += SSM_FOLDER;
    mkdir(save_path.c_str());
    save_path += "Agenda.txt";
    puts(save_path.c_str());
    file.open(save_path, std::ios::trunc|std::ios::out);
    if(!file.is_open()) return false;
    // exporting
    time_t const now_c = time(NULL);
    Discipline* subject = NULL;
    Evaluation* eval = NULL;
    int i = 0;
    bool iknowwhois = false;    // set true when sibject title is shown once
    subject = course->discipline(i);
    while( (subject = course->discipline(i)) != NULL ){
        for(int k = 0; k < subject->getNumOfEvals(); k++){
            eval = subject->evals(k);
            if(!eval->isDoable()){
                if(!iknowwhois) file << std::left << std::setw(MAXTITLESTR+2) << subject->getTitle();
                else file << std::setw(MAXTITLESTR+2) << " ";
                iknowwhois = true;
                file << std::left
                << std::setw(30) << eval->detail(false) << " : "
                << std::setw(12) << getTimeLeftTo(eval->getDateTime())
                << '\n';
            }
        }
        file << std::endl;
        i++; iknowwhois = false;
    }
    file << "\nDATA DE EXPORTAÇÃO: " << std::put_time(localtime(&now_c), "%A, %d %b %Y às %H:%M");
    file.close();
    return true;
}
bool export_evaluation(Course* course, int directory)
{
    if(course == NULL) return false;
    std::ofstream file;
    CHAR savefolder[MAX_PATH];
    SHGetFolderPath(NULL, directory, NULL, 0, savefolder);
    std::string save_path = savefolder;
    save_path += "\\";
    save_path += SSM_FOLDER;
    mkdir(save_path.c_str());
    save_path += "notas.txt";
    puts(save_path.c_str());
    file.open(save_path, std::ios::trunc|std::ios::out);
    if(!file.is_open()) return false;
    // exporting
    time_t const now_c = time(NULL);
    Discipline* subject = NULL;
    Evaluation* eval = NULL;
    int i = 0;
    bool iknowwhois = false;    // set true when sibject title is shown once
    subject = course->discipline(i);
    while( (subject = course->discipline(i)) != NULL ){
        for(int k = 0; k < subject->getNumOfEvals(); k++){
            eval = subject->evals(k);
            if(eval != NULL && eval->isDone()){
                if(!iknowwhois) file << std::left << std::setw(MAXTITLESTR+2) << subject->getTitle();
                else file << std::setw(MAXTITLESTR+2) << " ";
                iknowwhois = true;
                file << std::left
                << std::setw(10) << getEvalName(eval->getType()) << " >>  "
                << eval->getPercentageInt() << "% >>  "
                << std::setw(10) << eval->getDateString() << " >>  "
                << std::setprecision(2) << eval->getGrade()
                << '\n';
            }
        }
        if(iknowwhois){
            file
            << std::setw(MAXTITLESTR+2) << " " << "------------------------------------------\n"
            << std::setw(MAXTITLESTR+2) << " "
            << "MÉDIA: " << std::setprecision(2) << subject->getEvalsGradeAverage()
            << " / " << subject->getFinalGradeRequired()
            << "\tESTADO: " << subject->getStatus() << '\n'
            << std::endl;
        }
        i++; iknowwhois = false;
    }
    file << "\nDATA DE EXPORTAÇÃO: " << std::put_time(localtime(&now_c), "%A, %d %b %Y às %H:%M");
    file.close();
    return true;
}

// functionals
bool DirectoryExists_W(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
bool DirectoryExists_U(LPCTSTR szPath)
{
    /* implement */
    return false;
}
void convertToCase(std::string &str, enum case_type _case)
{
    int (*concase)(int);

    if(_case == lower)  concase = &tolower;
    else if (_case == upper) concase = &toupper;
    else if (_case == capital)
    {
        str[0] = (char)toupper(str[0]);
        for(unsigned i = 1 ; i < str.length(); i++)
        if( str[i] == ' ') {
            i++;
            if(isalpha(str[i+1])) str[i] = toupper(str[i]);
        }
        return;
    } else return;

    for(unsigned int i = 0; i < str.length() ; i++)
        str[i] = (char) concase(str[i]);

    return;
}
