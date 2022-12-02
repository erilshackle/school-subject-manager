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
        {0x70,  0x8F,   0x0E,       0x8A,   0x8E,   0x00,   0x87,   0x8B},  //  light 1
        {0x0F,  0x07,   0x0E,       0x0A,   0x0C,   0x00,   0x08,   0x0B},  //  dark 2
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
        else if(strcmp(argv[1], "notify")==0){
            std::cout << "no info..." << std::endl;
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
        std::cerr << "sintaxe: " << argv[0] << "[ update | notify | agenda | student yourusercode ] " << std::endl;
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
