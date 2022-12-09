#include <stdlib.h>
#include <iostream>
#include <iomanip>

#include "version.h"

#include <subjectsmanager.h>
SM::SSM* ssm = nullptr;

#include <utils.h>
#include <concolor.hpp>
BREADCUMBS BC;
CONSOLECOLORING Cor;

#include <ctime>
#include <chrono>
#include <thread>
#include <future>
#include <cmath>
#include <sys/file.h>

// util functons
void alertOnSuccess(std::string msg){
    std::cout << ssm->themecolor(th_success) << msg >> concolor::Color;
}
void alertOnError(std::string msg){
    std::cout << ssm->themecolor(th_error) << msg >> concolor::Color;
}
void alertOnWarring(std::string msg){
    std::cout << ssm->themecolor(th_warn) << msg >> concolor::Color;
}
void onMidlle(std::string str){
    int mlen = str.length()/2.0;
    std::cout << std::setw(getConfigScreenX()/2-mlen)<<' '<<str<<'\n';
    return;
}

int ret_0_ifnot_L(int L, int bound){
    if(L > bound) return bound;
    else return 0;
}

using namespace std;
using namespace concolor;

/// MENU
void menu_mainMenu(class Student&student);
void menu_student(Student&);
void menu_evaluations(class Course*);
void menu_subjects(class Course*);
void menu_settings(class Student*);

/// SUBMENU
void submenu_course(class Course*);
void submenu_subjectArchived(Course*, class Discipline*);
void submenu_manageSubjects(class Discipline*, Course&);
void submenu_manageEvaluations(class Discipline*);
void submenu_scheduleEvaluation(class Discipline*);
bool submenu_modifyEvaluation(class Discipline*, int);

/// SHOWING
void show_studentAgenda(Course*);
void show_tableOfSubjects(class Course*, bool=true, bool=false);
void show_tableOfEvaluationBrief(class Course*);
void show_aboutThisVersion(void);

void drawSSM(void){
    cout << right;
    cout << setw(55)<< " ___  ___   _   _ "         << '\n'
         << setw(55)<< "/  _)/  _) ( `-' )"         << '\n'
         << setw(55)<< "\\_\"-.\\_\"-. | \\_/ |"    << '\n'
         << setw(55)<< " __) )__) )( ) ( )"         << '\n'
         << setw(55)<< "/___//___/ /_\\ /_\\"       << '\n';
    cout << left;
}

void tryStudentRegistry(Student *student){
    if(student->isRegistered()) return;
    string user_input, aux,
            std_name,
            crs_name = student->getCourse();
    int opt,
        std_semester = student->course().getSemester(),
        std_lective = student->course().getLective();
    long int std_code = StringToNumber<int>(student->getCode());

    int def_theme = ssm->atheme;
    ssm->atheme = 2;
    ssm_applyTheme(ssm, th_main, true);
    heading("WELCOME TO SSM");
    printcolor("* Seja bem-vido ao STUDENT SUBJECTS MANAGER, um programa\n",ssm->themecolor(th_hint));
    printcolor("  simples feito para ajuda-lo a facilmente controlar suas\n",ssm->themecolor(th_hint));
    printcolor("  disciplinas, avaliacoes, eventos e geir suas notas.\n",ssm->themecolor(th_hint));
    printcolor("* Entre com as informacoes a seguir, para o registro.\n",ssm->themecolor(th_hint));

    std_name = getenv("username");
    name: // line 6
        printcolor(("\nEntre com o seu nome:\n"),ssm->themecolor(th_hint));
        printcolor("nome >> ",ssm->themecolor(th_input));
        getline(cin, user_input);
        if(!user_input.empty()){
            if(!validateName(user_input, true)){
                SLEEP(750);
                clearScreen_(6);
                goto name;
            }
            std_name = user_input;
        } else if(!validateName(std_name)){
            alertOnWarring("Erro: nome padrao nao aceite, degte seu nome");
            SLEEP(500);
            clearScreen_(6);
            goto name;
        }
    code: // line 9
        user_input.clear();
        printcolor(("\nEntre com seu codigo de estudante:\n"),ssm->themecolor(th_hint));
        printcolor("#codigo >> ",ssm->themecolor(th_input));
        getline(cin, user_input);
        if(!user_input.empty()){
            if(user_input == "-1"){ clearScreen_(6); goto name;}
            else if((StringToNumber<int>(user_input)) == 0 ){
                alertOnWarring("Codigo invalido: use numeros ou deixe vazio");
                SLEEP(750);
                clearScreen_(9);
                goto code;
            }
            std_code = (long) StringToNumber<int>(user_input);
        }
    course: // line 12
        user_input.clear();
        printcolor(("\nEntre com o seu curso:\n"),ssm->themecolor(th_hint));
        printcolor("curso >> ",ssm->themecolor(th_input));
        getline(cin, user_input);
        if(!user_input.empty()){
            if(user_input == "-1"){ clearScreen_(9); goto code;}
            else if(user_input.length() > MAXCOURSESTR){
                alertOnWarring("Caracteres maximo para atingido ("+to_string(MAXCOURSESTR)+")");
                SLEEP(750);
                clearScreen_(12);
                goto course;
            }
            crs_name = user_input;
        }
    lective_semester: // line 15
        user_input.clear();
        printcolor(("\nEntre o ano letivo e o semestre:\n"),ssm->themecolor(th_hint));
        printcolor("ano, semestre >> ",ssm->themecolor(th_input));
        getline(cin, user_input);
        if(!user_input.empty()){
            if(user_input == "-1"){ clearScreen_(12); goto course;}
            else if( (StringToNumber<int>(user_input)) > 12 || (StringToNumber<int>(user_input)) <= 0 ){
                alertOnWarring("Erro: use numeros validos.  ex: 3,1  (3*ano, 1* semestre)");
                SLEEP(750);
                clearScreen_(15);
                goto lective_semester;
            }
            string::size_type pos = user_input.find(",");
            if(pos != string::npos){
                aux = user_input.substr (pos+1);
                std_semester = StringToNumber<int>(aux);
                user_input = user_input.substr (0, pos);
                std_lective = StringToNumber<int>(user_input);
                std_semester = (std_semester%2 == 0) ? std_lective*2 : (std_lective*2)-1 ;
            }
            else std_semester = StringToNumber<int>(user_input);
        }
    basegrade: // line 18
        user_input.clear();
        printcolor(("\nSelecione o Sistema de Avaliacao: [nao podera ser alterado]\n"),ssm->themecolor(th_hint));
        cout << SELECTABLE << "0 a 20" << '\n' << SELECTABLE << "0 a 10" << '\n' << SELECTABLE << "0 a 100";
        opt = optionSelector(20, 3, false);
        if(opt != ESCAPED){
            switch(opt){
            case 0: BASE_GRADE = 20; break;
                case 1: BASE_GRADE = 10; break;
                case 2: BASE_GRADE = 100; break;
            }
        }

    if(student->Register(std_code, std_name, crs_name, std_semester) > 0){
        student->Save();
        student->course().Save();
        alertOnSuccess("Registro Conluido");
    }
    ssm->atheme = def_theme;
    SLEEP(1000);
    return;
}


int main(int argc, char* argv[])
{
    srand(time(NULL));
    std::cout << std::fixed << std::setprecision(2) << std::left;
    // init
    configScreen("STUDENTS SUBJECTS MANAGER", 60, 25);
    ssm =  SSM_INIT(argc, argv);
    Course* course = new Course("-");
    Student* user = new Student("Student", *course, 100000+rand()%999999L);
    course->registerStudent(*user);
    // load data
    user->LoadDefault();
    user->course().Load();
    user->course().LoadDisciplines();

    // check for user login
    if(argc == 3){
        if( strcmp(argv[1], "student") == 0 ){
            if(SM::_login_as_(&user ,argv[2], ssm) == false){
                PAUSESCREEN;
                return EXIT_FAILURE;
            } else SLEEP(1000);
        }
    } else if(argc == 2){
        if(strcmp(argv[1], "reset") == 0){
            conprint("Error").color(ssm->themecolor(th_error));
        }
    }

    menu_mainMenu(*user);       // main menu

    // save data
    if(ssm->atheme == 1)    Cor >> ssm->theme(th_input);
    else                    Cor >> ssm->theme(th_success);
    cout << "A guardar & sair";
    SSM_SAVE(ssm);
    cout.put('.'); SLEEP(250);
    user->course().Save();
    user->course().SaveDisciplines();
    cout.put('.'); SLEEP(250);
    user->Save();
    cout.put('.'); SLEEP( 250);
    cout << " ok!";
    Cor >> off;
    SLEEP(250);
    return EXIT_SUCCESS;
}

void menu_mainMenu(class Student& student)
{
    int opt;

    time_t now;
    tm* timing;

    tryStudentRegistry(&student);

    do{
        ssm_applyTheme(ssm, th_main, true);
        now = time(NULL);
        timing = localtime(&now);
        printcolor("MENU PRINCIPAL", backcolor(), textcolor());
        cout << "\t   " << put_time(timing,"%a, %d %b - %H:%M")
             << "\t\t " << ssm->version<<endl;
        fillch('=');    // user info
        Cor >> ssm->theme(th_hint);
//        concor << (corname)ssm->themecolor(th_hint);
        cout << "Usuario    : " << student.getName() << "  <"<< student.getCode() << '>' << '\n';
        cout << "Courso     : " << student.getCourse()<<'\n';
        cout << "Disciplinas: " << student.course().getNumDisciplines() << " registradas"
        <<endl;
//        concor >> (corname)ssm->themecolor(th_hint);
        Cor >> off;
        fillch('-');    // user info
        cout << SELECTABLE << "Perfil\n"
             << SELECTABLE << "Agenda\n"
             << SELECTABLE << "Avaliacoes\n"
             << SELECTABLE << "Disciplinas\n"
             << SELECTABLE << "Configuracoes\n"
             << SELECTABLE << "(X) Salvar & Sair"
            << endl;
        gotoxy(0,19);
        drawSSM();

        opt = optionSelector(6,6,true,1,false);
        //opt = optionMouseSelector(6,6);

        switch(opt){
            case 0:{    // perfil
                BC="Meu Perfil";
                menu_student(student);
                break;
            }
            case 1:{    // agenda
                BC="Minha Agenda";
                show_studentAgenda(&(student.course()));
                break;
            }
            case 2:{    // avaliacoes
                BC="Menu Avaliacoes";
                menu_evaluations(&student.course());
                break;
            }
            case 3:{    // disciplinas
                BC="Menu Disciplinas";
                menu_subjects(&(student.course()));
                break;
            }
            case 4:{    // definicoes
                BC="Preferencias";
                menu_settings(&student);
                break;
            }
            default:
            break;
        }
    } while( opt != ESCAPED);
    return;
}

void menu_student(Student& student)
{
    int opt, in_date[3];
    int long in_code;
    bool savedata = false, refreshonce = true;
    string in_str;
    ssm_applyTheme(ssm, th_secnd, false);

    do{
        CLEARSCREEN;
        heading(BC());
        cout.clear();
        Cor>>ssm->theme(th_hint);
        cout << left
             <<setw(14) << "Nome:" <<student.getName() << '\n'
             <<setw(14) << "Codigo:" <<student.getCode() << '\n'
             <<setw(14) << "Idade:" << student.getAge()<< " anos\t" << '{'<< student.getBirthdate()<<'}' << '\n'
             <<setw(14) << "Curso:" << student.getCourse(true)<< " letivo" << '\n'
             <<setw(14) << "Media Actual:" <<  (int)ceil( student.getFinalGrade(refreshonce) )
        <<endl;
        refreshonce = false;
        Cor>>off;
        fillch('-');
        cout << SELECTABLE << "Informacoes do Curso " << ssm->themecolor(th_input) << ">>" >> Color << '\n'
             << SELECTABLE << "Alterar Nome\n"
             << SELECTABLE << "Alterar Codigo" << ssm->themecolor(th_warn) << "!" >> Color << '\n'
             << SELECTABLE << "Definir Data de Nascimento\n"
             << SELECTABLE << "Minha Lista de Disciplinas\n"
             << SELECTABLE << "Exportar Minha Agenda e Notas" << ssm->themecolor(th_hint) << "/>" >> Color << '\n'
             << SELECTABLE << "(X) "<<(savedata?"Salvar & ":"") <<"Voltar";
        opt = optionSelector(8,7);
        switch(opt){
            case 0:{
                submenu_course(&(student.course()));
                break;
            }
            case 1:{    // alterar nome
                cout << ssm->themecolor(th_input) << "Digite seu novo nome: " >> Color;
                in_str = stringInputController(MAXNAMESTR, false, Black);
                if(in_str.empty()) break;
                if(student.setName(in_str)){
                    alertOnSuccess("> Feito!");
                    savedata = true;
                } else {
                     alertOnError("> Erro!");
                }
                SLEEP(500);
                break;
            }
            case 2:{    // alterar codigo
                cout << ssm->themecolor(th_input) << "Digite seu codigo (num) : " >> Color;
                in_str.clear();
                in_str = stringInputController(7, true);
                if(in_str.empty()) break;
                in_code = (long) StringToNumber<int> (in_str);
                if(student.setCode(in_code)){
                    alertOnSuccess("> Feito!");
                    savedata = true;
                } else {
                     alertOnError("> Erro!");
                }
                SLEEP(500);
                break;
            }
            case 3:{    // alterar data nascimento
                in_str.clear();
                cout << ssm->themecolor(th_input) << "Digite sua data de nascimento (dia.mes.ano) : " >> Color;
                getline(cin, in_str);
                if(in_str.empty()) break;
                if( stringToDateArray(in_date, in_str) ){
                    if( student.setAge(in_date) == true ){
                        alertOnSuccess("Feito!");
                        savedata = true;
                    } else {
                        alertOnError("Erro! Idade nao aceite.");
                    }
                }
                SLEEP(500);
                break;
            }
            case 4:{    // lista disciplinas
                BC +="LISTA DE DISCIPLINAS";
                CLEARSCREEN;
                heading(!BC);
                student.getSubjectsList(false);
                PAUSESCREEN;
                BC--;
                break;
            }
            case 5:{    // exportar agenda
                if(export_agenda(&student.course())){
                    alertOnSuccess("Agenda Exportada\n\n");
                } else {
                    alertOnError("Erro ao exportar Agenda\n\n");
                }
                if(export_evaluation(&(student.course()))){
                    alertOnSuccess("Notas Exportada\n");
                } else {
                    alertOnError("Erro ao exportar Notas\n");
                }
                PAUSESCREEN;
                break;
            }
        }
    } while (opt != ESCAPED);
    if(savedata){
        student.Save();
        cout << "salvo!";
        SLEEP(250);
    }
    return;
}

void menu_schedule(class Student* student)
{

}

void menu_evaluations(class Course* course)
{
    Discipline* in_dscp = NULL;
    int opt;

    ssm_applyTheme(ssm, th_secnd, true);

    /**  Discipline Selection */
    heading(BC());
    Cor>>ssm->theme(th_hint);
//    concor >> cc::themecolor(thc_hint);
    show_tableOfEvaluationBrief(course);
    Cor>>off;
    fillch('-');

    cout << ssm->themecolor(th_hint) << "\n* Escolha uma disciplina que queira gerir suas avaliacoes:\n\n" >> Color;

    printDiscilineListOf(*course);
    opt = optionSelector(8, (course->hasDiscipline() ? course->getNumDisciplines() : 1 ) , !course->hasDiscipline());
    //course->discipline(opt);
    if(opt != ESCAPED){
        in_dscp = course->discipline(opt);
        if(in_dscp != NULL){
            BC += in_dscp->getTitle();
            submenu_manageEvaluations(in_dscp);
            BC --;
        } else alertOnError("Erro: Disciplina Invalida");
    }

    return;
}

void menu_subjects(class Course* course)
{
    int opt, opt2, dszt, in_num;
    string in_str, in_str2, in_str3;
    string::size_type pos, pospace;
    Discipline* in_dscp = NULL;

    ssm_applyTheme(ssm, th_secnd, false);

    do{
        dszt = course->getNumDisciplines();
        if(dszt > 5)dszt =  5;
        else if(dszt == 0) dszt =  -1;
        // show
        CLEARSCREEN;
        heading(BC());
        cout.clear();
        Cor>>ssm->theme(th_hint);
        show_tableOfSubjects(course,false);
        Cor>>off;
        fillch('-');
        cout << SELECTABLE << "Gerir Minhas Disciplinas " << ssm->themecolor(th_input) << ">>" >> Color << '\n'
             << SELECTABLE << "Adicionar Disciplinas" << ssm->themecolor(th_warn) << (dszt == -1 ?"*":"") >> Color << '\n'
             << SELECTABLE << "Eliminar Disciplinas\n"
             << SELECTABLE << "Consultar Disciplinas Arquivadas\n"
             << SELECTABLE << "(X) Voltar";

        opt = optionSelector(dszt+5,5);
        switch(opt){
            case 0:{    // visualizar e gerir
                    BC << "Minhas Disciplinas";
                    CLEARSCREEN;
                    heading(!BC);
                    show_tableOfSubjects(course,true,true);
                    if(course->hasDiscipline()){
                        printcolor(" Tecle ESC para voltar ou ENTER para gerir...", ssm->themecolor(th_hint));
                        opt2 = optionSelector(4,course->getNumDisciplines(),false);
                        if(opt2 != ESCAPED){
                            in_dscp = course->discipline(opt2);
                            submenu_manageSubjects(in_dscp, *course);
                        }
                    } else {
                        SLEEP(500);
                    }
                    BC<<"Menu Disciplinas";
                    break;
                }
            case 1:{    // adicionar                                            @use of thread detach
                in_str.clear();
                in_str3.clear();
                Cor>>ssm->themecolor(th_hint);
                cout << "* nome da disciplina de ate "<< MAXTITLESTR-1 <<" letras e sem espacos.. `AM2`\n"
                     << "* podes dar uma descricao a disciplina utilizando `: `\n"
                     << "  ex... `IP` ou `IP: Introducao a Programacao`\n\n";
                !Cor;
                cout << ssm->themecolor(th_input) << "Digite o nome da Disciplina: " >> Color;
                //in_str = stringInputController(MAXTITLESTR, Black);
                getline(cin, in_str);
                if(in_str.empty()) break;
                /// improvement code for retrive desciption with shortcut of ': '
                pos = in_str.find(": ");
                if(pos != string::npos){
                    in_str3 = in_str.substr (pos+2); // holds the description value
                    while(in_str3.front() == ' ') in_str3.erase(in_str3.begin()) ;
                    while(in_str3.back() == ' ') in_str3.pop_back() ;
                    if(in_str3.length() >= MAXTITLESTR+MAXNAMESTR+2) in_str3.clear();
                    in_str = in_str.substr (0, pos);    // retain the TITLE input
                }
                while(in_str.front() == ' ') in_str.erase(in_str.begin()) ;
                while(in_str.back() == ' ') in_str.pop_back() ;
                /// end of improvement, value of description at in_str3
                if(course->getDiscipline(in_str) != NULL){
                    alertOnWarring("Erro: disciplina ja foi registrada!");
                }
                else if(!validateTitle(in_str,true)){
                    break;
                }
                else{
                    in_str2.clear();
                    cout << ssm->themecolor(th_input) << "Digite o nome do Professor: " >> Color;
                    in_str2 = stringInputController(MAXNAMESTR);
                    if(course->add_discipline(in_str,in_str2)){
                        course->getDiscipline(in_str)->setDescription(in_str3);
                        // thread saving
                        std::thread sc(&CourseSaveDataOnThread, (void*)course); sc.detach();
                        alertOnSuccess("> Disciplina Registrada!");
                    } else {
                         alertOnError("> Erro, disciplina nao registrada!");
                         SLEEP(1200);
                    }
                }
                SLEEP(500);
                break;
            }
            case 2:{    // eliminar                                             @use of thread detach
                BC += "Eliminar Disciplina";
                    CLEARSCREEN;
                    heading(!BC);
                    printcolor("* Selecione a disciplina que queira eliminar da sua\n", ssm->themecolor(th_hint));
                    printcolor("  lista de disciplinas registradas:\n\n", ssm->themecolor(th_hint));
                    printDiscilineListOf(*course);
                    if(course->hasDiscipline()){
                        cout << "   (X) Voltar";
                        opt2 = optionSelector(5,course->getNumDisciplines()+1);
                        if(opt2 != ESCAPED){
                            in_dscp = course->discipline(opt2);
                            if(course->del_disciplineRegistered(in_dscp)){
                                alertOnSuccess("> Dsciplina Removida!");
                                std::thread sc(&CourseSaveDataOnThread, (void*)course); sc.detach();
                            }else {
                                alertOnError("> Erro!!");
                            }
                            in_str.clear();
                        }
                    } else SLEEP(500);
                    BC--;
                    SLEEP(500);
                break;
            }
            case 3:{    // arquivadas
                    BC+="Disciplinas Arquivadas";
                    in_num = course->getNumDisciplinesArchived();
                    CLEARSCREEN;
                    heading(!BC);
                    printcolor("* Disciplinas arquivadas sao disciplinas ja concluidas.\n", ssm->themecolor(th_hint));
                    printcolor("  lista de disciplinas arquivadas:\n\n", ssm->themecolor(th_hint));
                    printDiscilineListOf(*course,true);
                    //printDiscilineListOf(*course,true,3*it,3*(++it));
                    //if((in_num-(it*3))>0) in_num-=(it*3);
                    //if((in_num-(it*3)) > 0)
//                            cout << "   ... Listar Outros\n";
                    if(course->hasDisciplineArchived()){cout << "   (X) Voltar";}
                    else {
                        Cor>>ssm->themecolor(th_input);
                        cout << "\n\n"
                             << "# Uma disciplina pode ser arquivada quando 100% das \n"
                             << "# avaliacoes agendadas forem totalmente concluidas;\n"
                             << "# Disciplinas marcadas como arquivadas, estarao disponiveis\n"
                             << "# apenas para consulta: Nao pode alterar seus dados e elas\n"
                             << "# nao aparecerao mais na lista de disciplinas registradas;\n"
                             << "# Aqui voce podera sempre Consultar, Eliminar e Exporta-las.\n";
                        Cor>>off;
                    }
                    opt2 = optionSelector(5,in_num+1);
                    if(opt2 != ESCAPED){
                        in_dscp = course->archived(opt2);
                        submenu_subjectArchived(course, in_dscp);
                        SLEEP(500);
                    }
                    BC--;
                break;
            }
            case 4:{    //
                break;
            }
        }

    }while(opt != ESCAPED);
}

void menu_settings(class Student* student)
{
    int opt, opt2;
    string in_str;
    bool savedata = false;
    Discipline* in_dscp = NULL;
    ssm_applyTheme(ssm, th_main, false);

    do{
        CLEARSCREEN;
        heading(BC());
        cout << ssm->themecolor(th_hint) << left <<'@'<< setw(14) << SSMANAGER_VAUTHOR >> Color;
        cout << ssm->themecolor(th_input) << internal << setw(30) << SSMANAGER_VWEBSITE >> Color;
        cout << ssm->themecolor(th_hint) << right << setw(14) << ssm->version >> Color <<'\n';
        fillch('=');
        cout << SELECTABLE << "Informacoes sobre esta Versao" << ssm->themecolor(th_input) << "*" >> Color << '\n'
             << SELECTABLE << "Escolher Tema\n"
             << SELECTABLE << "Verificar Atualizacoes\n"
             << SELECTABLE << "Dados e Configuracos\n"
             << SELECTABLE << "(X) "<<(savedata?"Salvar & ":"") <<"Voltar"
             << endl;

        opt = optionSelector(4,5);
        //opt = optionMouseSelector(6,6);

        switch(opt){
            case 0:{    // sobre esta versao
                in_str = to_string(ssm->code);
                BC += ("informacoes - SSM" + in_str);
                show_aboutThisVersion();
                BC--;
                break;
            }
            case 1:{    // escolher tema
                do{
                    CLEARSCREEN;
                    heading("ESCOLHER TEMA");
                    cout << left;
                    cout << SELECTABLE << "Tema Padrao" <<(ssm->atheme==0?"*":" ") << "\t\t\t(Aqua e Magenta)" <<'\n';
                    cout << SELECTABLE << "Tema Claro"  <<(ssm->atheme==1?"*":" ") << "\t\t\t(Branco e cinza)" <<'\n';
                    cout << SELECTABLE << "Tema Escuro" <<(ssm->atheme==2?"*":" ") << "\t\t\t(Preto)" <<'\n';
                    cout << SELECTABLE << "(X) OK!";
                    opt2 = optionSelector(2,4);
                    if(opt2 >= 0 && opt2 < 3){
                        conprint("aplicando tema.").typewrite();
                        ssm->atheme = opt2;
                        cout.put('.');
                        if(SSM_SAVE(ssm)){
                            cout.put('.');
                        }
                        ssm_applyTheme(ssm, th_main, false);
                        cout << " ok";
                    }
                    SLEEP(250);
                }while(opt2 != ESCAPED);
                cout << "concluindo...";
                SLEEP(500);
                break;
            }
            case 2:{    // verificar atualizacao
                _update_updater();
                conprint.out("Carregando...").typewrite(100);
                break;
            }
            case 3:{    // dados e configuracoes
                do{
                    CLEARSCREEN;
                    heading("DADOS E CONFIGURACOES");
                    cout << left;
                    cout << SELECTABLE << "Salvar todos os dados"<<'\n';
                    cout << SELECTABLE << "Exportar Agenda " << ssm->themecolor(th_hint) << "/> (Desktop)" >> Color <<'\n';
                    cout << SELECTABLE << "Exportar Disciplina " << ssm->themecolor(th_hint) << "/> (Docuemtns)" >> Color <<'\n';
                    cout << SELECTABLE << "Exportar Notas " << ssm->themecolor(th_hint) << "/> (Desktop)" >> Color <<'\n';
                    cout << SELECTABLE << "(X) Retornar";
                    opt2 = optionSelector(2,5);

                    switch(opt2){

                        case 0:{
                            if(ssm->atheme == 1)    Cor >> ssm->theme(th_input);
                            else                    Cor >> ssm->theme(th_success);
                            cout << "Guardando os dados";
                            SSM_SAVE(ssm);
                            cout.put('.'); SLEEP(250);
                            student->course().Save();
                            student->course().SaveDisciplines();
                            cout.put('.'); SLEEP(250);
                            student->Save();
                            cout.put('.'); SLEEP( 250);
                            cout << " pronto!";
                            Cor >> off;
                            break;
                        }
                        case 1:{
                            if(export_agenda(&(student->course()))){
                                alertOnSuccess("Agenda Exportada");
                            } else {
                                alertOnError("Erro ao exportar");
                            }
                            SLEEP(2000);
                            break;
                        }
                        case 2:{
                            CLEARSCREEN;
                            heading("ESCOLHA A DISCIPLINA PARA EXPORTAR");
                            printDiscilineListOf(student->course());
                            opt = optionSelector(2, (student->course().hasDiscipline() ? student->course().getNumDisciplines() : 1 ) , !student->course().hasDiscipline());
                            if(opt != ESCAPED){
                                in_dscp = student->course().discipline(opt);
                                if(in_dscp != NULL){
                                    if(export_subject(in_dscp,CSIDL_PERSONAL,false)){
                                        alertOnSuccess("Disciplina Exportada");
                                    } else {
                                        alertOnError("Erro na exportacao");
                                    }
                                } else alertOnError("Erro: Disciplina Invalida");
                            }
                            SLEEP(2000);
                            break;
                        }
                        case 3:{
                            if(export_evaluation(&(student->course()))){
                                alertOnSuccess("Notas Exportada");
                            } else {
                                alertOnError("Erro ao exportar");
                            }
                            SLEEP(2000);
                            break;
                        }
                    }
                }while(opt2 != ESCAPED);
                SLEEP(500);
                break;
            }
            case 4:{    // Ajuda
                conprint("MODO DE UTILIZACAO DO PROGRAMA::").onMiddle();
                break;
            }
        }

    } while( opt != ESCAPED);
    return;
}

/// SUBEMUS

void submenu_course(class Course* course)
{
    int opt, in_num, dsz, adsz;
    string in_str;
    bool savedata = false;

    BC >> "MEU CURSO";
    do{
        CLEARSCREEN;
        heading(BC());
        cout.clear();
        Cor>>ssm->theme(th_hint);
        cout << left
             <<setw(14) << "Curso:" << course->getCourse() << '\n'
             <<setw(14) << "Acronimo:" << course->getCourseAcronym() << '\n'
             <<setw(14) << "Ano Letivo:" <<course->getLective() << "* ano" << '\n'
             <<setw(14) << "Semestre: " << course->getSemester()<< "* semestre do curso"<<'\n'
             <<setw(14) << "Disciplinas:"
                        << course->getNumDisciplines() << " registradas & "
                        << course->getNumDisciplinesArchived() << " arquivadas"
        <<endl;
        Cor>>off;
        fillch('-');
        cout << SELECTABLE << "Alterar Curso\n"
             << SELECTABLE << "Alterar Acronimo do Curso\n"
             << SELECTABLE << "Alterar Semestre do Curso" << ssm->themecolor(th_input) << '*' >> Color << '\n'
             << SELECTABLE << "Definir Ano Letivo\n"
             << SELECTABLE << "Lista Geral de Disciplinas\n"
             << SELECTABLE << "(X) "<<(savedata?"Salvar & ":"") <<"Voltar";
        opt = optionSelector(8,6);

        switch(opt){
            case 0:{    // alterar nome curso
                in_str.clear();
                cout << ssm->themecolor(th_input) << "Seu Curso: " >> Color;
                in_str = stringInputController(MAXCOURSESTR, false, Black);
                if(in_str.empty()) break;
                if(course->setCourse(in_str)){
                    alertOnSuccess("> Feito!");
                    savedata = true;
                } else {
                     alertOnError("> Erro!");
                }
                SLEEP(500);
                break;
            }
            case 1:{    // alterar acronimo curso
                in_str.clear();
                cout << ssm->themecolor(th_input) << "Acronimo do Curso: " >> Color;
                in_str = stringInputController(5, true, Black);
                if(in_str.empty()) break;
                if(course->setCourseAcronym(in_str)){
                    alertOnSuccess("> Feito!");
                    savedata = true;
                } else {
                     alertOnError("> Erro! utilize apenas letras");
                }
                SLEEP(500);
                break;
            }
            case 2:{    // alterar semestre curso
                in_str.clear();
                printcolor("O `semestre do curso` deve ser alterado a cada mudanca\nde semestre do aluno\n\n",
                    ssm->themecolor(th_hint));
                cout << ssm->themecolor(th_input) << "Digite o Semestre actual do curso (num) : " >> Color;
                getline(cin, in_str);
                if(in_str.empty()) break;
                in_num = StringToNumber<int>(in_str);
                if(course->setSemester((unsigned)in_num)){
                    alertOnSuccess("> Feito!");
                    savedata = true;
                } else {
                     alertOnError("> Erro! Semestre invalido");
                }
                SLEEP(500);
                break;
            }
            case 3:{    // alterar ano curso
                in_str.clear();
                cout << ssm->themecolor(th_input) << "Digite o Ano Letivo (num): " >> Color;
                getline(cin, in_str);
                if(in_str.empty()) break;
                in_num = StringToNumber<int>(in_str);
                if(course->setLective((unsigned)in_num)){
                    alertOnSuccess("> Feito!");
                    savedata = true;
                } else {
                     alertOnError("> Erro! Ano letivo invalido");
                }
                SLEEP(500);
                break;
            }
            case 4:{    // lista geral disciplinas
                dsz = course->getNumDisciplines();
                adsz = course->getNumDisciplinesArchived();
                BC +="LISTA GERAL DE DISCIPLINAS";
                CLEARSCREEN;
                heading(!BC);
                in_num = max(course->getNumDisciplines(), course->getNumDisciplinesArchived());
                Cor<<Black>>ssm->themecolor(th_warn);
                cout << setw((getConfigScreenX()/2)+2) << " REGISTRADAS"
                     << setw((getConfigScreenX()/2)-3) << " ARQUIVADAS" << '\n';
                Cor>>off;
                for(int i = 0; i < in_num; i++){

                    cout << " ";
                    if(i < dsz){
                        cout << std::left << setw((getConfigScreenX()/2)+2) << course->discipline(i)->getTitle();
                    } else {
                        cout << std::left << setw((getConfigScreenX()/2)+2) << " ";
                    }
                    if(i < adsz){
                        cout << std::left << setw((getConfigScreenX()/2)-4) << course->archived(i)->getTitle();
                    }
                    cout.put('\n');
                }
                fillch('-');
                Cor >> ssm->themecolor(th_hint);
                cout << " Disciplinas Registradas:" << right << setw(3) << dsz << (adsz > 999 ? "\n" : "    ")
                     << " Disciplinas Arquivadas:" << right << setw(3)<< adsz << '\n';
                !Cor;
                fillch('=');
                PAUSESCREEN;
                BC--;
                break;
            }
        }

    }while(opt != ESCAPED);
    BC--;
    if(savedata){
        course->Save();
        cout << "salvo!";
        SLEEP(250);
    }
}
void submenu_subjectArchived(Course* course,class Discipline* archived)
{
    int opt, posini, opt2;
    bool savevret;
    CLEARSCREEN;
    heading((BC>>archived->getTitle())); BC--;
    cout << left << setprecision(1)
        <<setw(15) << "Disciplina:" << archived->getTitle() << " (" << archived->getDescription() << ")\n"
        <<setw(15) << "Professor:" << archived->getProfessor() << '\n'
        <<setw(15) << "Semestre:" << archived->getSemester() << "* semestre do curso" << '\n'
        <<setw(15) << "Avaliacoes: " << setw(4) <<archived->getNumOfEvals()
    <<endl;
    fillch('-');
    for(int i = 0; i < archived->getNumOfEvals(); i++){
        Evaluation* eval = archived->evals(i);
        std::cout << std::left
            << " "<< std::setw(17) << getEvalName(eval->getType())
            << " >> " << std::setw(14) << eval->getDateString()
            << "  (" << eval->getPercentageInt() << "%) " << std::setw(3);
            std::cout << "nota " << std::right << std::setw(5) << std::setprecision(2) << eval->getGrade()
        << endl;
    }
    fillch('-');
    cout << left << std::setprecision(1) << " Nota Final: "
         << setw(25) << archived->getEvalsGradeAverage()
         << " Estado: " << archived->getStatus() << '\n';
    fillch('=');
    posini = 10 + archived->getNumOfEvals();

    cout << SELECTABLE << "(X) Voltar\n";
    cout << SELECTABLE << "Exportar\n";
    cout << SELECTABLE << "Eliminar\n";
    opt = optionSelector(posini,3,false);

    switch(opt){
        case 1:
            savevret = export_subject(archived);
            if(savevret){
                alertOnSuccess("Disciplina Exportado com sucesso");
            } else {
                alertOnError("Nao foi possivel exportar a disciplina");
            }
            break;
        case 2:
            clearScreen_(posini);
            printcolor("Pretende Eliminar esta disciplina arquivada? \n", ssm->themecolor(th_warn));
            cout << SELECTABLE << "SIM  (eliminar)\n";
            cout << SELECTABLE << "SIM  (Exportar e eliminar)\n";
            cout << SELECTABLE << "NAO  (cancelar)\n";
            opt2 = optionSelector(posini+1,3);
            if(opt2 != ESCAPED){
                if(opt2 == 1){
                    if(export_subject(archived)){
                        alertOnSuccess("* Disciplina Exportado com sucesso\n");
                    }
                }
                if(course->del_disciplineArchived(archived)){
                    alertOnSuccess("* Disciplina Eliminado com sucesso!");
                } else
                    alertOnError("Erro: Nao pode ser eliminado");
            } else {
                alertOnWarring("Cancelado!");
            }
            break;
    }
    PAUSESCREEN;
    return;
}
void submenu_manageSubjects(class Discipline* discipline, Course& course)
{
    int opt, opt2, in_num;
    float in_real;
    string in_str;
    bool savedata = false, is_fullscheduled;
    discipline->getEvalsGradeAverage();
    BC >> discipline->getTitle();
    do{
        is_fullscheduled = discipline->isEvalsFullScheduled();
        CLEARSCREEN;
        heading(BC());
        cout.clear();
        Cor>>ssm->theme(th_hint);
        cout << left << setprecision(1)
             <<setw(15) << "#Disciplina:" << discipline->getTitle() << " : " << discipline->getDescription() << '\n'
             <<setw(15) << "Professor:" << discipline->getProfessor() << '\n'
             <<setw(15) << "Semestre:" << discipline->getSemester() << "* semestre do curso" << '\n'
             <<setw(15) << "Avaliacoes: " << setw(4) <<discipline->getNumOfEvals()
                        << setw(30)<<right<< discipline->getNumOfEvals()<<left << "/   feitas"<<'\n'
             <<setw(15) << ( discipline->isCompleted() ? "Nota Final:" : "Nota Actual:")
                        << setw(4)<< discipline->getEvalsGradeAverage(true)
                        << setw(30)<<right<< discipline->getFinalGradeRequired()<<left <<"/ condicao"<< '\n'
             <<setw(15) << "Estado:" << discipline->getStatus()
        <<endl;
        Cor>>off;
        fillch('-');
        cout << SELECTABLE << ssm->themecolor(th_hint) <<"#Sigla da disciplina nao pode ser alterada\n" >> Color;
        cout << SELECTABLE << "Gerir Avaliacoes " << ssm->themecolor(th_input) <<">>">> Color <<'\n'
             << SELECTABLE << "Alterar descricao da disciplina" << '\n'
             << SELECTABLE << "Alterar Nome do Professor" << '\n'
             << SELECTABLE << "Alterar Semestre da Disciplina" << '\n'
             << SELECTABLE << "Definir Requisitos para aprovacao" << '\n'
             << SELECTABLE << ssm->themecolor(th_warn) << ( is_fullscheduled ?
                            "Arquivar esta disciplina (marcar como concluida)\n" : "\r") >> Color;
        cout << SELECTABLE << "(X) "<<(savedata?"Salvar & ":"") <<"Voltar";

        cout << "\n" << "";

        opt = optionSelector(10,6+(int)(is_fullscheduled) );

        switch(opt){
            case 0:{     // Gerir Avaliacoes
                BC+="Avaliacoes";
                submenu_manageEvaluations(discipline);
                BC--;
                break;
            }
            case 1:{     // Definir descricao
                in_str.clear();
                cout << ssm->themecolor(th_input)
                        << "Degite uma nova discricao (completo):\n> " >> Color <<discipline->getTitle() <<": ";
                in_str = stringInputController(MAXTITLESTR+MAXNAMESTR-1, false, Black);
                if(in_str.empty()) break;
                if(discipline->setDescription(in_str)){
                    alertOnSuccess("> Feito!");
                    savedata = true;
                } else {
                     alertOnError("> Erro!");
                }
                SLEEP(500);
                break;
            }
            case 2:{     // Alterar professor
                in_str.clear();
                cout << ssm->themecolor(th_input) << "Degite o nome do professor: "  >> Color;
                in_str = stringInputController(MAXNAMESTR-1, false, Black);
                if(in_str.empty()) break;
                if(discipline->setProfessor(in_str)){
                    alertOnSuccess("> Feito!");
                    savedata = true;
                } else {
                     alertOnError("> Erro!");
                }
                SLEEP(500);
                break;
            }
            case 3:{     // Alterar semestre
                in_str.clear();
                cout << ssm->themecolor(th_input) << "Digite o Semestre para esta discilina (num) : " >> Color;
                getline(cin, in_str);
                if(in_str.empty()) break;
                in_num = StringToNumber<int>(in_str);
                if(discipline->setSemester((unsigned)in_num)){
                    alertOnSuccess("> Feito!");
                    savedata = true;
                } else {
                     alertOnError("> Erro! Semestre invalido");
                }
                SLEEP(500);
                break;
            }
            case 4:{     // Requisitos Aprovacao (submenu)
                in_str.clear();
                cout << ssm->themecolor(th_hint) << "Digite a nota final minima passar nesta disciplina:\n" >> Color;
                conprint(" N.Final Requerida >> ").color(ssm->themecolor(th_input));
                getline(cin, in_str);
                if(in_str.empty()) break;
                in_real = StringToDecimal(in_str);
                if(discipline->setFinalGradeRequired(in_real)){
                    alertOnSuccess("> Feito!");
                    savedata = true;
                } else {
                     alertOnError("> Erro! Requesito invalido");
                }
                SLEEP(500);
                break;
            }
            case 5:{     // Arquivar disciplina      @condicional
                ssm_applyTheme(ssm,th_warn,false);
                clearScreen_(9);
                cout.put('\n');
                onMidlle("< ARQUIVAR ESTA DISCIPLINA />");
                if(!discipline->isCompleted())
                {
                    cout<<"\n\n"
                        << " Voce precisa fazer todas as avaliacoes para poder arquivar\n"
                        << " esta disciplina. ainda ha "
                        << discipline->getNumOfEvals() - discipline->getNumOfEvalsDone()
                        <<" avaliacoes para atribuir nota."
                        <<endl;
                    ssm_applyTheme(ssm,th_secnd, false);
                    clearScreen_(15);
                    cout << "ok!\b";
                    PAUSESCREEN;
                    break;
                }
                cout<<"\n\n"
                    << " Marcar esta disciplina como concluida e move-la para lista\n"
                    << " de desciplinas arquivadas. Nao podera alterar seus dados."
                    <<endl;
                ssm_applyTheme(ssm,th_secnd, false);
                clearScreen_(15);
                printcolor("[?] Voce confirma que quer arquivar esta disciplina?", ssm->themecolor(th_hint));
                cout << "\n"
                    << "    (X) NAO\n"
                    << "    (!) SIM (Arquivar)";
                opt2 = optionSelector(16, 2);
                // alert option box
                if(opt2 != ESCAPED){
                    cout << "Cancelado!";
                    break;
                } else
                if(course.archive_discipline(discipline)){
                    alertOnError("Feito! Disciplina arquvada com sucesso");
                    savedata = true;
                    opt = ESCAPED;
                } else {
                    alertOnError("Erro, nao foi possivel arquivar.");
                }
                SLEEP(1000);
                break;
            }
        }

    }while(opt != ESCAPED);
    BC--;
    if(savedata){
        discipline->SaveData();
        cout << "salvo!";
        SLEEP(250);
    }

}
void submenu_manageEvaluations(class Discipline* discipline)
{
    if(discipline == NULL) return;
    int opt, opt2;
    string in_str;

    EvaluationType in_etype = Generic;
    int in_date[3] = {0,0,0};
    float in_per100 = 0;
    bool savedata = false;
    do{
        CLEARSCREEN;
        heading(BC());
    // show table of evaluation overview
    Cor >> ssm->themecolor(th_hint);
    discipline->printEvalsBrief();
    Cor >> off;
    fillch('=');
    // menu options
    cout << SELECTABLE << "Listar e Geir Avalaicoes agendadas " << ssm->themecolor(th_input) << ">>" >> Color << '\n'
         << SELECTABLE << "Agendar uma avaliacao"
            << ssm->themecolor(th_warn) << (!discipline->hasEvalScheduled() ? "*":"" ) >> Color << '\n'
         << SELECTABLE << "Detalhe geral" << '\n'
         << SELECTABLE << (savedata ? "Salvar & " : "" ) << "Voltar (ESC)" << '\n';

    opt = optionSelector(6, 4);

    switch(opt){
        case 0:{     // Listar e gerir
            do{
                BC>>"Avaliacoes agendadas";
                CLEARSCREEN;
                heading(!BC); BC--;
                discipline->printEvalsList();
                fillch('=');
                cout << ssm->themecolor(th_hint) << "[ESC] para voltar" >> Color;
                opt2 = optionSelector(2, (discipline->hasEvalScheduled()?discipline->getNumOfEvals():1),false);
                if(opt2 != ESCAPED){
                    if(!discipline->hasEvalScheduled()) break;
                    submenu_modifyEvaluation(discipline, opt2);
                }
            }while(opt2 != ESCAPED);
            break;
        }
        case 1:{     // Agendar
            for(int i = 0; i < 3; i++) in_date[i] = 0;
            in_per100 = 0; in_etype = Generic; in_str.clear();
            BC+=("Agendar avaliacao ("+discipline->getTitle()+")");
            type: // type
            CLEARSCREEN;
            heading(!BC);
            BC--;
            //** List Of Avaliable Evaluation of a discipline
            if(discipline->hasExamScheduled() && discipline->isEvalsFullScheduled()) {
                cout << "   ESTA DISCIPLINA POSSUI 100% DAS AVALIACOES AGENDADAS"<<endl;
                fillch('=');
                PAUSESCREEN;
                break;
            }
            printcolor("Qual avaliacao deseja agenda?\n", ssm->themecolor(th_hint));
            fillch('=',30);
            for(int i = 0; i < 5; i++) {
                cout << SELECTABLE;
                if(discipline->isEvalSchedulable((eval_t)i)){
                    cout << getEvalName((eval_t)i);
                } else {
                    cout << ssm->themecolor(th_invalid) << getEvalName((eval_t)i) >> Color;
                }
                cout.put('\n');
            }
            cout << SELECTABLE << "(X) Cancelar [ESC]\n";
            fillch('=',30);
            //** End Of List
            opt2 = optionSelector(4,6);
            if(opt2 == ESCAPED)
                break;
            else
                in_etype = (EvaluationType)opt2;

            if (!discipline->isEvalSchedulable(in_etype)){
                alertOnWarring("Avaliacao nao pode ser agendada.");
            }
            else {
                data:   // data
                if(in_etype != EvaluationType::Others){
                    printcolor(("Qual a data da avaliacao?\n"), ssm->themecolor(th_hint));
                    cin.clear();
                    cout <<ssm->themecolor(th_input) << "[dia.mes.ano] >> " >> Color;
                    getline(cin, in_str);
                    if(!in_str.empty()){
                        stringToDateArray(in_date,in_str);
                    } else if(in_str == "-1") goto type;
                }
                percentage: // percentage
                if(in_etype != Exam){
                    fillch('=',30);
                    printcolor("Qual o peso dessa avaliacao\n", ssm->themecolor(th_hint));
                    cout << ssm->themecolor(th_input) << "[max. "<<100-discipline->getEvalsStatus()<<"%] >> " >> Color;
                    cin >> in_per100;
                    if (in_per100 == 0) break;
                }
                fillch('=',30);
                if(discipline->scheduleEvaluation(in_etype, in_per100, in_date)){
                    savedata = true;
                    alertOnSuccess(getEvalName((EvaluationType)in_etype)+" Agendado com sucesso!");
                } else {
                    alertOnError(getEvalName((EvaluationType)in_etype)+" nao pode ser agendado!");
                    SLEEP(250);
                }
            }
            SLEEP(1250);
            break;
        }
        case 2:{     // Detalhes

            clearScreen_(6);
            Cor >> ssm->themecolor(th_hint);
            cout << "Nota Final:           " << discipline->getEvalsGradeAverage() << '\n';
            cout << "N.F. Requerida:       " << discipline->getFinalGradeRequired() << '\n';
            cout << "Estado de Progresso:  " << discipline->getStatusProgress() << '\n';
            cout << "Estado da Disciplina: " << discipline->getStatus() << '\n';
            cout << "Avisos:               " << "Nao temos nenhuma recomendacao!"<< '\n';
            Cor >> off;
            PAUSESCREEN;
            break;
        }
        }
    }while(opt != ESCAPED);
    if(savedata){
        discipline->SaveData();
    }
    return;
}
bool submenu_modifyEvaluation(class Discipline* discipline, int index_eval)
{
    int opt, eliminar;
    Evaluation* eval = discipline->evals(index_eval);
    int in_date[3], in_perc; //in_type;
    double in_grade;
    string user_input;
    bool savedata = false;

    clearScreen_(3+discipline->getNumOfEvals());
    cout << SELECTABLE; colour("Atribuir Nota\n")       >> (eval->isDoable() ? (eval->isDone() ? ssm->themecolor(th_success) : ssm->themecolor(th_input)) : ssm->themecolor(th_invalid));
    cout << SELECTABLE; colour("Alterar Percentagem\n") >> (eval->isExam() ? ssm->themecolor(th_invalid) : concolor::current_colors());
    cout << SELECTABLE; colour("Alterar Data\n")        >> (eval->isTypeOf(Others) ? ssm->themecolor(th_invalid) : concolor::current_colors());
    cout << SELECTABLE << "Eliminar Avaliacao\n";
    cout << SELECTABLE << "Voltar (ESC)";
    conprint(getEvalName(eval->getType()) + " -- " + eval->getTimeStatus() + ", " + getTimeLeftTo(eval->getDateTime()) ).atPos(24,3);
    opt = optionSelector(3+discipline->getNumOfEvals(),5);

    switch(opt){
        case 0: // Atribuir Nota
            if(eval->isDoable()){
                printcolor("* Digite a nota (0.."+to_string((int)BASE_GRADE)+").\n* Para marcar como nao feito entre com `...`\n", ssm->themecolor(th_hint));
                printcolor("nota >> ", ssm->themecolor(th_input));
                getline(cin,user_input);
                in_grade = StringToDecimal(user_input);
                if(user_input == "..." ){
                    eval->setDone(false);
                    alertOnWarring("Marcado como nao feito!");
                    savedata = true;
                } else
                if(user_input.empty()){
                        break;
                } else
                if((user_input != "0" && in_grade == 0.0)){
                    alertOnWarring("entrada invalida!");
                } else
                if(discipline->gradeEvaluation(index_eval, (float)in_grade)){
                    alertOnSuccess("Sucesso!");
                    savedata = true;
                } else{
                    alertOnError("Erro!");
                }
            } else {
                alertOnWarring("! Nao podes atribuir nota a essa evaliacao ainda.");
                SLEEP(1250);
            }
            SLEEP(750);
            break;
        case 1: // Alterar Percentagem
            if(eval->isExam()) break;
            user_input.clear();
            printcolor("Degite o novo peso para essa avaliacao: \n", ssm->themecolor(th_hint));
            printcolor("(max. "+to_string((100-(discipline->getEvalsStatus()-eval->getPercentageInt())))+"%) >> ", ssm->themecolor(th_input));
            getline(cin, user_input);
            in_perc = StringToNumber<int>(user_input);
            if(eval->setPercentage(in_perc)){
                alertOnSuccess("Sucesso!");
                savedata = true;
            }
            else alertOnError("Erro!");
            SLEEP(750);
            break;
        case 2: // Alterar Data
            if(eval->isTypeOf(Others)) break;
            user_input.clear();
            printcolor("Degite a nova data para a avaliacao: \n", ssm->themecolor(th_hint));
            printcolor("(dia.mes.ano) >> ", ssm->themecolor(th_input));
            getline(cin, user_input);
            stringToDateArray(in_date, user_input);
            if(user_input.empty()){
                break;
            } else
            if(eval->setDate(in_date)){
                alertOnSuccess("Sucesso!");
                savedata = true;
            }
            else alertOnError("Erro!");
            SLEEP(750);
            break;
        case 3: // Eliminar
            //conprint("Pretende eliminar esta avaliacao? \n").typewrite();
            printcolor("Pretende eliminar esta avaliacao? \n", ssm->themecolor(th_warn));
            cout << SELECTABLE << "SIM  (eliminar)\n";
            cout << SELECTABLE << "NAO  (cancelar)\n";
            eliminar = optionSelector(10+discipline->getNumOfEvals(),2);
            if(eliminar == 0){
                if(discipline->removeScheduledEvaluation(index_eval)){
                    alertOnSuccess("Eliminado com sucesso!");
                    savedata = true;
                } else alertOnError("Erro: Nao pode ser eliminado");
            } else alertOnWarring("Cancelado!");
            SLEEP(750);
            break;
    }
    if(savedata){
        discipline->SaveData();
        cout << "\nSalvo!";
    }
    return savedata;
}


/// SHOWING

void show_studentAgenda(Course* disciplines)
{
    CLEARSCREEN;
    Discipline* subject = NULL;
    Evaluation* eval = NULL;
    int i = 0, events = 0;
    // next event
    time_t time1 = 0, time2 = 0;
    Discipline* nextSbj=NULL;
    Evaluation* nextEval=NULL;
    // get today time
    stringstream sstime;
    time_t const now_c = time(NULL);
    sstime << right << setw(59-12) << put_time(localtime(&now_c), "%A, %d %b %Y");
    string myTime = sstime.str();
    //transTime >> myTime;

    heading(BC());
    Cor >> ssm->themecolor(th_hint);
    conprint("hoje: " + myTime).atPos(0,25);
    Cor >> off;
    subject = disciplines->discipline(i);
    while( (subject = disciplines->discipline(i)) != NULL ){
        for(int k = 0; k < subject->getNumOfEvals(); k++){
            eval = subject->evals(k);
            if(!eval->isDoable()){
                // check next
                time2 = eval->getDateTime();
                if(time2 < time1 || time1 == 0){
                    time1 = time2;
                    nextEval = eval;
                    nextSbj = subject;
                }
                // show data
                /// AED       | Trabalho (20%)   | Daqui ha 17 dias
                cout << std::left
                << setw(MAXTITLESTR) << subject->getTitle() << " | "
                << setw(21) << (getEvalName(eval->getType())+" ("+to_string(eval->getPercentageInt())+"%)") << " | "
                << setw(12) << getTimeLeftTo(eval->getDateTime())
                << '\n';
                events++;
            }
        }
        i++;
    }
    if(events <= 0){
        onMidlle("Agenda Vazia");
    } else if(nextSbj != NULL && nextEval != NULL){
        fillch('-');
        cout << ssm->themecolor(th_warn) << "Proximo Evento: " >> Color
             << nextSbj->getTitle() << " >> " << getEvalName(nextEval->getType())
             << " (" + to_string(nextEval->getPercentageInt()) << "%) "
             << nextEval->getDateString() << endl;
    }
    fillch('=');
    if(events == 0)
        colour("Eventos que ainda nao foram realizadas serao mostradas aqui") >> ssm->themecolor(th_hint);
    PAUSESCREEN;
    return;
}

void show_tableOfSubjects(class Course* course, bool full_elements, bool tabbed)
{
    if(!course->hasDiscipline()){
        cout << "\tNenhuma disciplina foi registrada ainda." << endl;
        if(full_elements) fillch('-');
        return;
    }
    int dsz = course->getNumDisciplines();
    string sep;
    if(tabbed) sep = "   "; else sep = " - ";

    cout << left << ssm->themecolor(th_hint)
         << setw(MAXTITLESTR+3) << " - DISCIPLINAS" << " - "
         << setw(MAXNAMESTR)    << "PROFESSOR"      << " - "
         << setw(2)             << "SS"              << " - "
         << setw(8)             << "ESTADO"
         >> Color <<
    endl;
    fillch('-');
    for(int i = 0; i < dsz; i++){
        cout << left
             << sep
             << setw(MAXTITLESTR)   << course->discipline(i)->getTitle()         << " - "
             << setw(MAXNAMESTR)    << course->discipline(i)->getProfessor()     << " - "
             << setw(2)             << course->discipline(i)->getSemester()      << " - "
             << setw(8)             << course->discipline(i)->getStatus()        <<
        '\n';
        // controlers
        if( i == 2){
            if(!full_elements && (i+3) < (dsz) ){    // i+3 due to show +2 next lines and +1 due to dsz index
                cout << setw(3+MAXTITLESTR/2) << "..."
                     << setw(1+MAXTITLESTR/2) << std::showpos << (dsz-4) << std::noshowpos
                     << setw(3+MAXNAMESTR) << "..." << setw(5) << "..." << "..."
                     << '\n';
                cout << left
                     << sep
                     << setw(MAXTITLESTR)   << course->discipline(dsz-1)->getTitle()         << " - "
                     << setw(MAXNAMESTR)    << course->discipline(dsz-1)->getProfessor()     << " - "
                     << setw(2)             << course->discipline(dsz-1)->getSemester()      << " - "
                     << setw(8)             << course->discipline(dsz-1)->getStatus()        <<
                '\n';
                break;
            }
        }
    }
    if(full_elements) fillch('-');
    return;
}
void show_tableOfEvaluationBrief(class Course* course)
{
    if(course == NULL) return;
    int evnum[4] = {0,0,0,0};

    for(int x = 0; x < 4; x++)
        evnum[x] += course->getNumOfEvalOfType((eval_t)x);

    // 1st row
    for(int i = 0; i < 4; i++)
        cout << getEvalName((EvaluationType)i, Others == (EvaluationType)i )<<'s' << "("<< evnum[i] <<") ";
    cout << "Exames(" << course->getNumOfExams() <<")\n";
    // 2nd row
    //cout << "% " << "\n";
    // 3rd row

    cout << ssm->themecolor(th_input) << "Proximo Evento: " << course->getNextEvaluationEvent() >> Color << endl;
    return;
}
void show_aboutThisVersion(void)
{
    CLEARSCREEN;
    int opt;
	time_t now = time(0);
	tm* verdate = localtime(&now);
	verdate->tm_mday = atoi(AutoVersion::DATE);
	verdate->tm_mon = atoi(AutoVersion::MONTH)-1;
	verdate->tm_year = atoi(AutoVersion::YEAR)-1900;
    errno = 0;
    heading(!BC);
    // about this version
    cout << left;
    cout << setw(20) <<"Name"           << ": " << SSMANAGER_VNAME << endl;
    SLEEP(25);
    cout << setw(20) <<"Version"        << ": " << AutoVersion::FULLVERSION_STRING << endl;
    cout << setw(20) <<"Ver. Code"   << ": " << AutoVersion::UBUNTU_VERSION_STYLE
                                                <<'-'<<AutoVersion::STATUS_SHORT<<AutoVersion::REVISION << endl;
    SLEEP(25);
    cout << setw(20) <<"Author"         << ": " << SSMANAGER_VAUTHOR << endl;
    SLEEP(25);
    cout << setw(20) <<"Github"         << ": " << SSMANAGER_VGHUB << endl;
    SLEEP(25);
    cout << setw(20) <<"Website"        << ": " << ssm->themecolor(th_input) << SSMANAGER_VWEBSITE >> Color << endl;
    Cor >> ssm->themecolor(th_hint);
    cout <<"\ncompiled at "             <<  put_time(verdate,"%b %d %Y") <<' '<<__TIME__;
    SLEEP(25);
    Cor >> off;
    // end of versioning

    /**< draw  */
    cout << right <<"\n\n\n\n\n\n\n\n\n\n";
    Cor >> ssm->themecolor(th_hint);
    drawSSM();
    Cor>>off;
    gotoxy(0, 21);
    cout << "   Voltar" << '\n'
         << "   Visitar Website" << '\n'
         << "   Author Links" << '\n';
    opt = optionSelector(21, 3, false);
    switch(opt){
        case ESCAPED:
        case 0:{    // voltar
            return;
        }
        case 1:{    // Website
            std::ifstream web1("ssm_website.url");
            if(web1.is_open()){
                web1.close();
            } else {
                std::ofstream url1("ssm_website.url", std::ios::trunc);
                url1 << "[InternetShortcut]\n";
                url1 << "URL="<< "https://" << SSMANAGER_VWEBSITE;
                url1.close();
            }
            ShellExecute(0, 0, "ssm_website.url", 0, 0 , SW_SHOW );
            cerr << "  Abrindo website:" << strerror(GetLastError());
            break;
        }
        case 2:{   // Linktree
            std::ifstream web2("ssm_linktree.url");
            if(web2.is_open()){
                web2.close();
            } else {
                std::ofstream url2("ssm_linktree.url", std::ios::trunc);
                url2 << "[InternetShortcut]\n";
                url2 << "URL="<<"https://linktr.ee/erilshackle";
                url2.close();
            }
            ShellExecute(0, 0, "ssm_linktree.url", 0, 0 , SW_SHOW );
            cerr << "  Abrindo linktree:" << strerror(GetLastError());
        }
    }
    SLEEP(1000);
    return;
}
