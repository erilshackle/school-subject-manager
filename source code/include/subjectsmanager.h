#ifndef SUBJECTSMANAGER_H
#define SUBJECTSMANAGER_H

#ifndef VER_PRODUCTNAME_STR
#define VER_PRODUCTNAME_STR                 "Students Subjects Manager"
#endif // VER_PRODUCTNAME_STR

/**< @name @author @copyright @link email @link website @version */
#define SSMANAGER_VNAME          VER_PRODUCTNAME_STR
#define SSMANAGER_VAUTHOR        "Eril Carvalho"
#define SSMANAGER_VCOPYRIGHT     "Copyright © 2022 SSM Erilando T.S Carvalho."
#define SSMANAGER_VEMAIL         "erilandocarvalho@yahoo.com"
#define SSMANAGER_VGHUB          "@erilshackle"
#define SSMANAGER_VWEBSITE       "subjects-manager.netlify.com"
#define SSMANAGER_VVERSION       VER_PRODUCTVERSION_STR

#if     _WIN32_WINNT < 0x0500
#undef  _WIN32_WINNT
#define _WIN32_WINNT   0x0500
#endif

#include <shlobj.h>
//#pragma comment(lib, "shell32.lib")

#include <student.h>
#include <course.h>
#include <discipline.h>
#include <utils.h>

// dependency includes
#include "html_begin.pp"
#include "updater.html"
#include "html_end.pp"
// dirs
#define SSM_FOLDER "SSManager\\"
#define SSM_ROOT "ssmanager\\"
#define SSM_USER "user\\"
#define SSM_SUBJ "subjects\\"
#define SSM_EVAL "evals\\"
// files
#define SSM_USERS_LIST "ulist.ini"
#define SSM_UPDATER "updater.html"
// constants
#define MAXSMTHEMES 3
#define SSMTHCOLORS 8

#ifdef WIN32
#define __DELSSMDATA "RD /S /Q /F evals subjects user && DEL /F /Q *.*"
#else
#define __DELSSMDATA "rem -r -f .*."
#endif // WIN32

namespace SM
{
    struct subjectmanager
    {
        char version[5];
        int  code;              // version date: yymmdd
        char lc_language[10];
        char lc_numeric[10];
        int atheme = 0;
        int  themes[MAXSMTHEMES][SSMTHCOLORS];      // main, secondary, waring, success, error, hint, input
        char datafile[15];

        int theme(unsigned int theme_color){
            if(theme_color > SSMTHCOLORS-1) return 0xff;
            return themes[atheme][theme_color];
        }
        concolor::colorsname themecolor(unsigned int theme_color){
            if(theme_color > SSMTHCOLORS-1) return concolor::Color;
            return (concolor::colorsname) themes[atheme][theme_color];
        }

    };
    typedef struct subjectmanager SSM;
    void _RUN(void);
    bool _LOAD(SSM* *CFG);
    void _APPLYCONFG(SSM* CFG);

    // system calling
    bool _login_as_(class Student**, char*, SSM*);
    bool _registry_user_(std::string, SSM*);
    int _update_updater();
}
using namespace SM;

SSM* SSM_INIT(int argc, char **argv);
bool SSM_SAVE(struct subjectmanager *CFG);

// user data list
bool user_list_loader(void);
bool user_list_registry(class Student*);
bool user_check_if_exist(char*);


//extern int ATHEME;
enum theme_color {th_main, th_secnd, th_warn, th_success, th_error, th_invalid, th_hint, th_input};
void ssm_applyTheme(SSM* _ssm, int theme,  bool clearscreen = true);

// Subjects Functions
int subjectsListing(Course*, bool, int=0, int=17); /**@return num of disciplines */
bool export_subject(Discipline*,INT=CSIDL_PERSONAL, bool=false);
bool export_agenda(Course*,INT=CSIDL_DESKTOP);
bool export_evaluation(Course*,INT=CSIDL_DESKTOP);

// functional
bool DirectoryExists_W(LPCTSTR szPath);
bool DirectoryExists_U(LPCTSTR szPath);
enum case_type {lower = -1, upper = 1, capital = 0};
extern void convertToCase(std::string &str, enum case_type _case);

// inline checks
inline bool existFile(const std::string& filepath){
    std::ifstream f(filepath);
    if(!f.is_open()) return false;
    f.close();
    return true;
}

// os dependent functions
#ifdef WIN32
    #define DirectoryExists(LPCTSTR) DirectoryExists_W(LPCTSTR)
#else
    #define DirectoryExists(LPCTSTR) DirectoryExists_U(LPCTSTR)
#endif // WIN32

#endif // SUBJECTSMANAGER_H
