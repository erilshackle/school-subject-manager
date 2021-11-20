/** \project  Subject Manager (Binary Tree) projecto final da disciplina de Algorimo e Estrutura de Dados
 *
 * \name        : School-Subject-Manager (Gestor de Disciplinas escolares)
 * \author      : Erilando T.S. Carvalho (2001)
 * \version     : 2.0
 * \date        : 5-07-2021
 * \code        : Windows OS
 * \link        : https://subjects-manager.netlify.app/
 *
 */#define VERSION "v2.1"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
/**< My Includes */
#include "student.h"
#include "display.h"
#include <avals.h>

#define DATAFOLDER_RUN system("if not exist ssm_data (mkdir ssm_data)");
#define DATAFOLDER_DEL system("if exist ssm_data (rd ssm_data /f)");

#define LINK "https://drive.google.com/file/d/16t9MB-SGSPF8ygmCdRjO-Ndjpyb7Zhax/view?usp=sharing"

/**< MENUS */
void menu_principal(STUDENT *Aluno);
void menu_estudante(STUDENT* Aluno);
void menu_disciplinas(STUDENT* Aluno);
void menu_settings();
void menu_about();
/**< SUBMENUS */
void overview_disciplinas(SUBJECTS* *Disciplinas);
void detalhar_disciplina(SUBJECTS* Disciplina);
void submenu_filtrar_disciplina(SUBJECTS* Disciplinas, char mode, float media);
void my_theme(char _theme[5][2]);

/**< COMPLEMENTARES */
void heading(char title[], char subtitle[], char desciption[]){
    CLEAR_SCREEN;
    puts(title);puts("- - - - - - - - - - - - - - - - - - - -");
    puts(subtitle); puts(desciption);   putchar('\n');
    return;
}
void typeWrite(const char* text, int speed){
    int i = 0;
    while(i < strlen(text)) {
        putchar(text[i++]);
        fflush(stdin);
        Sleep(speed);
    }
  return;
}


/**< ENTRIES */
bool entry_string(char* var_, int lengh, char what_is[]){
    int i; char _input[lengh+1];
    printf("%s << ", what_is);
    for(i =0;i < lengh-1; i++)putchar('.');
    for(i =0;i < lengh-1; i++)putchar('\b');
    fflush(stdin); gets(_input);
    if( strlen(_input) > lengh ) return false;
    if (strcmp(_input,"")==0) return false;
    strcpy(var_, _input);
    return true;
}

int main(void)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    DATAFOLDER_RUN;
    /**< carregar display configs */
        ConsoleDisplay(&console,70,25);
        configLoad();
    /**< carregar info e dados */
        STUDENT Aluno = inicializar_student();
        carregar_dados(&Aluno);
        get_student_mediaFinal(&Aluno, (Aluno.disciplina));
        WAIT(0.5);
    /**< Menu Programa*/
        menu_principal(&Aluno);
    /**< salvar e sair */
        typeWrite("A salvar e sair", 25);
        salvar_dados(&Aluno);               putchar('.');   // guardar os dados de alunos e disciplinas
        configSave();                       putchar('.');   // guardar as configurações do programa
        free_disciplinas(Aluno.disciplina); putchar('.');   // limpar memoria allocada das disciplinas
    WAIT(0.5);
    return 0;
}
/**< MENUS */
void menu_principal(STUDENT*Aluno){
    bool status = true;
    char op, showTime[24];
    struct tm* my_time;
    time_t tempo;
    do{
        CLEAR_SCREEN;
        system(CONFIG.color[0]);
        //Obter Hora e data de hoje do sistema
        tempo = time(NULL);	 my_time = localtime(&tempo);
        strftime(showTime,sizeof(showTime), "%a %d %b - %H:%M", my_time);  // armazenar data de agora

        MarkIt(" MENU PRINCIPAL", Black,BWhite);
        printf("\t\t %s \t\t ", showTime);   MarkIt(VERSION,LGreen,Black);
        puts("\n--------------------------------------------------------------------");
        puts(" 1 | Menu Estudante\t\t\t\t\t\t[E] ");
        puts("--------------------------------------------------------------------");
        puts(" 2 | Gerir Disciplinas\t\t\t\t\t\t[D] ");
        puts("--------------------------------------------------------------------");
        puts(" 3 | Configurações\t\t\t\t\t\t[C] ");
        puts("--------------------------------------------------------------------");
        puts(" 4 | Saber mais sobre o programa\t\t\t\t[I] ");
        puts("--------------------------------------------------------------------");
        puts(" 5 | Slavar meus dados\t\t\t\t\t\t[S] ");
        puts("--------------------------------------------------------------------");
        puts(" 0 | Salvar tudo & Sair. \t\t\t\t   [?]info");
        puts("--------------------------------------------------------------------\n");
        printf("Escolha >> "); fflush(stdin);
        op = getchar();

        switch(op){

            case 'e':case 'E':case '1':{
                menu_estudante(Aluno);
                break;
            }
            case 'd':case 'D':case '2':{
                menu_disciplinas(Aluno);
                break;
            }
            case 'c':case 'C':case '3':{
                menu_settings();
                break;
            }
            case 'i':case 'I':case '4':{
                menu_about();
                break;
            }
            case 's':case 'S':case '5':{
                textcolor(LAqua);
                typeWrite("> Salvando dados", 25);
                get_student_mediaFinal(Aluno, Aluno->disciplina );  putchar('.');
                configSave();                                       putchar('.');
                status = salvar_dados(Aluno);                       putchar('.');
                if(status == true)
                    MarkIt(" OK\r", LGreen, BackgroundColor);
                else
                    MarkIt(" Error\r", LRed, BackgroundColor);
                WAIT(1);
                break;
            }
            case'?':{
                textbackground(Yellow);
                textcolor(BWhite);
                puts("\nQualquer altereacao feita, quer em disciplinas, avaliacoes, etc...  ");
                puts("Para guardar as alteracoes voce precisa sair por este menu principal");
                textcolor(LGreen);  printf("[ 0 | Salvar tudo & Sair]. ");  textcolor(BWhite);
                printf("Para quando voltar voce econtrar todos   \nos seus dados, voce tem que");
                textcolor(LRed);printf(" sempre ");
                textcolor(BWhite);    MarkIt("fazer isso.",BWhite,Yellow);
                textbackground(Blue);
                PAUSE_SCREEN;
                break;
            }
        }
    }while(op!='0');
    return;
}

void menu_estudante(STUDENT* Aluno){
    system(CONFIG.color[1]);
    // variaveis
    char op, input_filter_type;
    char input_text[MAX_NAME];
    float input_real;
    int input_num;
    COLORS media_color;
    if(Aluno->media_final < 10)         media_color = LRed;
    else if (Aluno->media_final < 17.5) media_color = LYellow;
    else if (Aluno->media_final < 19)   media_color = LBlue;
    else                                media_color = LGreen;
    SUBJECTS* input_dcp = NULL;
    //menu
    do{CLEAR_SCREEN;    srand( time(NULL) );
        printf("Nome        : %s (%d anos)\n", Aluno->nome, Aluno->idade);
        printf("Curso       : %s (%i* ano)\n", Aluno->curso, Aluno->letivo);
        printf("ID          : %s\n", Aluno->ID);
        printf("Disciplinas : %d\n", _NUM_DISCIPLINAS_ATUAL);
        printf("Media final : %-4.1f ", Aluno->media_final);
        if (Aluno->media_final < NOTA_MAXIMA / 2.0)  MarkIt(":(", media_color, BackgroundColor);
        else    MarkIt(":)", media_color, BackgroundColor);
        printf(" \t [A](Atualizar Media Final)\n");
        puts("====================================================================");
        puts(" N << Alterar meu Nome");
        puts(" C << Alterar meu Curso");
        puts(" I << Alterar meu ID");
        puts(" D << Definir Data Nascimento");
        puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        puts(" 1 << Visualizar minhas disciplinas");
        puts(" 2 << Ver minha agenda\n");
        puts(" 3 << Agendamento de Avaliação");
        puts(" 4 << Atribuir Notas a avaliação\n");
        puts(" 5 << Filtar disciplinas por média");
        puts(" 6 << exportar infos do estudante para arquivo .csv\n");
        puts(" 0 << Voltar");
        puts("--------------------------------------------------------------------");
        printf("Escolha >> "); fflush(stdin);
        op = getchar();

        switch(op){
            case 'n':case 'N':{
                heading("ALTERAR MEU NOME", "* Entre com seu novo nome.","* Deixe vazio para permacecer com o atual.");
                set_student_nome(Aluno);
                puts("> OK!");
                break;
            }
            case 'a':case'A':{
                get_student_mediaFinal(Aluno, Aluno->disciplina);
                printf("Media final = %-4.1lf    ", Aluno->media_final);
                if(Aluno->media_final < NOTA_MAXIMA / 2.0){            MarkIt("Insuficiente\r", LRed, BackgroundColor);media_color = LRed;}
                else if (Aluno->media_final < NOTA_MAXIMA / 1.14){    MarkIt("Bom\r", LYellow, BackgroundColor);       media_color = LYellow;}
                else if (Aluno->media_final < NOTA_MAXIMA / 1.05){      MarkIt("Muito Bom\r", LBlue, BackgroundColor);  media_color = LBlue;}
                else{                                   MarkIt("Excelente\r", LGreen, BackgroundColor); media_color = LGreen;}
                WAIT(1.5);
                break;
            }
            case 'c':case 'C':{
                heading("ALTERAR MEU CURSO","* Entre com o novo nome do curso.","* Deixe vazio para permacecer com o atual.");
                set_student_curso(Aluno);
                puts("> OK!");
                break;
            }
            case 'i':case 'I':{
                heading("DEFINIR MEU ID ESTUDANTE", "* Entre com seu código de ID.","");
                set_student_ID(Aluno);
                puts("> OK!");
                break;
            }
            case 'd':case 'D':{
                CLEAR_SCREEN;
                heading("DEFINIR IDADE","* Informe todas os dados de forma valida.","* Entre com numeros inteiros e positivos");
                if( set_student_idade(Aluno) ){
                    puts("> OK!");
                } else{
                    puts("> ERRO!");
                }
                break;
            }
            case '1':{
                CLEAR_SCREEN;
                puts(" Disciplina   | N* Aval |  Ultimo evento feito | Média | Estado");
                puts("---------------------------------------------------------------------");
                visualizar_disciplinas(Aluno->disciplina);
                puts("---------------------------------------------------------------------");
                typeWrite("visto...",15);
                PAUSE_SCREEN;
                break;
            }
            case '2':{
                CLEAR_SCREEN;
                //textcolor(Black);
                puts("MINHA AGENDA\n");
                //resetTextcolor(1);
                agenda(Aluno->disciplina);
                puts("visto...");
                PAUSE_SCREEN;
                break;
            }
            case '3':{
                CLEAR_SCREEN;
                heading("GERIR AGENDAMENTO",
                        "* Aqui voce pode agendar seus testes, projecto, exame, etc.",
                        "* Entre com o nome da disciplina a agendar para prosseguir."
                );
                get_student_subjects(Aluno, true);
                if(entry_string(input_text,DCP_TITLE,"Disciplina")){
                    convert_Upercase(input_text);
                    input_dcp = buscar_disciplina(Aluno->disciplina, input_text);
                    if(input_dcp != NULL){
                        _agendar_avaliacao_( input_dcp );
                    }else{
                        printf("\nDisciplina %s não encontrado!", input_text);
                        WAIT(1);
                    }
                }
                break;
            }
            case '4':{
                CLEAR_SCREEN;
                heading("ATRIBUIR NOTA",
                        "* Aqui voce atribui/modifica notas das avaliacoes agendadas antes.",
                        "* Entre com o nome da disciplina da avalicao que quer atribuir a nota."
                );
                get_student_subjects(Aluno, true);
                if(entry_string(input_text, DCP_TITLE, "Disciplina")){
                    convert_Upercase(input_text);
                    input_dcp = buscar_disciplina(Aluno->disciplina, input_text);
                    if(input_dcp != NULL){
                        _avaliamento_( input_dcp );
                    }else{
                        printf("\nDisciplina %s não registrado!", input_text);
                        WAIT(1);
                    }
                }
                break;
            }
            case '5':{
                CLEAR_SCREEN;
                heading("FILTAR DISCIPLINAS MEDIA",
                        "* Filtre disciplinas que têm medias < ou >= do qual voce escolher.",
                        "* Use < ou > e depois uma media entre 0 e 20 para fazer a filtragem");
                input_num = rand()%2;
                printf("* Exemplo para filtrar médias %s %i\n", input_num == 0? "inferiores:  <" : "superiores:  >",
                        5+rand()%13
                );
                puts("* Para voltar entre com 00\n");
                /// verificar entrada
                do{
                    printf("filtrar media: "); fflush(stdin);
                    scanf("%c%f", &input_filter_type, &input_real);
                }while( (input_real < 0.00 || input_real > 20.00) );
                if(input_real == 0.00) break;   // voltar
                submenu_filtrar_disciplina(Aluno->disciplina, input_filter_type, input_real);
                typeWrite("visto...",20);
                PAUSE_SCREEN;
                break;
            }
            case '6':{
                CLEAR_SCREEN;
                heading("EXPORTAR INFOS ESTUDANTE",
                        "* Exportar todos so meus dados relevantes",
                        "* As informações serão salvas num formato de arquivo texto"
                        );
                /// eril_discente.txt
                sscanf(Aluno->nome,"%s", input_text);
                strcat(input_text,"_discente.txt");
                // esperar o utilizador confirmar
                printf("precione <ENTER> para continuar...");
                fflush(stdin);
                if(PRESS_ENTER) exportar_student_info(Aluno,input_text);
                else typeWrite("CANCELADO",50);
                WAIT(0.5);
                printf("\7");
            }
            //case '\n': return;
        }

    }while( op!='0');
    fflush(stdin);
    return;
}

void menu_disciplinas(STUDENT *Aluno){
    system(CONFIG.color[2]);
    //variaveis
    char op, curso_acronym[8] = "";
    char dcp_input[DCP_TITLE], input_proff[MAX_NAME], input_text[MAX_NAME];
    int i, acronym = 0;
    unsigned int input_posi;
    //float real_input;
    SUBJECTS* aux_dcp = NULL;
    // acronimo de curso
    for(i = 0; i < strlen(Aluno->curso); i++)
    if(Aluno->curso[i] >=65 && Aluno->curso[i] <=90 && acronym < 7) curso_acronym[acronym++] = Aluno->curso[i];
    //menu
    do{CLEAR_SCREEN;
        MarkIt(" MENU DISCIPLINAS\n", Black,BWhite);
        puts("- - - - - - - - - -");
        printf("Disciplinas: %d \t\t raiz(%s) \t%10s:%s ", _NUM_DISCIPLINAS_ATUAL, Aluno->disciplina->titulo, "Curso", curso_acronym);

        puts("\n===================================================================");
        puts(" 1 / L | Listar todas as Disciplinas");
        puts("-------------------------------------------------------------------");
        puts(" 2 / + | Adicionar uma Disciplina");
        puts(" 3 / ! | Arquivar Disciplina");
        puts(" 4 / - | Eliminar uma Disciplina");
        puts("-------------------------------------------------------------------");
        puts(" 5 / P | Alterar Nome do Professor de uma disciplina");
        puts(" 6 / S | Alterar o semestre de uma disciplina");
        puts("-------------------------------------------------------------------");
        puts(" 7 / X | Exportar infos das disciplinas para arquivo .csv");
        puts("-------------------------------------------------------------------");
        puts("   0   | Voltar ao menu principal");
        puts("-------------------------------------------------------------------");
        printf("Escolha << "); fflush(stdin);
        op = getchar();

        switch(op){
            case 'l': case 'L': case '1':{
                overview_disciplinas(&Aluno->disciplina);
                break;
            }
            case '+': case '2':{
                CLEAR_SCREEN;
                heading("ADICIONAR DISCIPLINA","* Utilizar acronimo da disciplina\n\tex: \"AED\" em vez de \"Alg. Estr. de Dados\"","* Deixe vazio para cancelar");
                // iserir titulo
                if( entry_string(dcp_input, DCP_TITLE, "Disciplina") ) {
                    // iserir professor
                    fflush(stdin);
                    while( entry_string(input_proff, MAX_NAME, "Professor")==false) {}

                    // iserir semestre
                    printf("Semestre (int) << n\b");
                    scanf("%u",&input_posi);

                    convert_Upercase(dcp_input);    // converter para uppercase para ter uniforminade
                    if( (inserir_disciplina(&(Aluno->disciplina), dcp_input, input_proff, input_posi, false))==true )
                        puts("REGISTRADO\a");
                    WAIT(1);
                }
                break;
            }
            case '!': case '3':{
                CLEAR_SCREEN;
                heading("ARQUIVAR DISCIPLINA",
                        "Eliminar uma disciplina e manter suas informações relevantes!",
                        "Esta funcionalidade ainda não está pronta....");
//                get_student_subjects(Aluno, true)
//                entry_string(input_text,DCP_TITLE,"DIsciplina");
//
//                arquivar_disciplina(&Aluno);
//                WAIT_PRESSKEY;
                break;
            }
            case '-': case '4':{
                CLEAR_SCREEN;
                heading("ELIMINAR DISCIPLINA", "* Entre com o titulo da disciplina a eliminar\n","! Para eliminar todos entre com [delete all] \n! Deixe vazio para cancelar");

                get_student_subjects(Aluno, true);
                entry_string(dcp_input,DCP_TITLE,"Disciplina");
                if(strcmp(dcp_input,"")==0) break;
                convert_Upercase(dcp_input);
                if(strcmp(dcp_input, "DELETE ALL")==0){
                    eliminar_todas_disciplinas(&(Aluno->disciplina));
                    puts("OK: TUDO FOI ELIMINADO!\a");
                } else {
                if( eliminar_disciplina(&(Aluno->disciplina), dcp_input) ) puts("OK: ELIMINADO!\a");
                }
                WAIT(1);
                break;
            }
            case 'p': case 'P': case '5':{
                CLEAR_SCREEN;
                heading("ALTERAR NOME DO PROFESSOR","1* Entre com o nome da disciplina","2* Digite o novo nome para o docente");
                get_student_subjects(Aluno, true);
                entry_string(dcp_input,DCP_TITLE,"Disciplna");
                convert_Upercase(dcp_input);
                if ( (aux_dcp = buscar_disciplina(Aluno->disciplina, dcp_input)) != NULL){
                    printf("< %s >\n\n", aux_dcp->proff);
                    entry_string(input_proff, sizeof(Aluno->disciplina->proff)/ sizeof(char), "Professor");
                    if(strcmp(input_proff,"")==0) break; fflush(stdin);
                    strcpy(aux_dcp->proff,input_proff);  puts("OK");
                }
                WAIT(1);
                break;
            }
            case 's': case 'S': case '6':{
                CLEAR_SCREEN;
                heading("ALTERAR SEMESTRE","1* Entre com o nome da disciplina a modificar o semestre","2* Digite o semestre da disciplina, considerando todo os anos");
                get_student_subjects(Aluno, true);
                entry_string(dcp_input,DCP_TITLE,"Disciplna");
                convert_Upercase(dcp_input);
                if ( (aux_dcp = buscar_disciplina(Aluno->disciplina, dcp_input)) != NULL){
                    printf("< %u*semestre >\n\n", aux_dcp->semestre);
                    printf("Semestre << "); scanf("%u",&input_posi);
                    if(input_posi<=0 || input_posi >255) break;
                    aux_dcp->semestre = input_posi; puts("OK");
                }
                WAIT(1);
                break;
            }
            case 'x':case 'X':case '7':{
                CLEAR_SCREEN;
                heading("EXPORTAR INFOS DISCIPLINAS","* Exportar todos os dados relevantes das disciplinas","* Os dados serão salvos num arquivo de formato excel");
                sscanf(Aluno->nome,"%s", input_text);
                strcat(input_text,"_disciplinas.csv");
                printf("precione <ENTER> para continuar...");
                fflush(stdin);
                if(PRESS_ENTER) exportar_subject_info(Aluno->disciplina,input_text);
                else typeWrite("CANCELADO",50);
                WAIT(0.5);
                printf("\7");
            }
            case '\n': return;
        }

    }while(op!='0');
    fflush(stdin);
    return;
}

void menu_settings(){
    // variaveis
    char op, showTime[25];
    char _theme[5][2], text_input[20];
    // hora e data
    time_t tempo = time(NULL);    struct tm* my_time = localtime(&tempo);
    strftime(showTime,sizeof(showTime), "%a %d %b - %H:%M", my_time);  // armazenar data de agora
    //menu
    system(CONFIG.color[3]);
    do{CLEAR_SCREEN;
        MarkIt("MENU CONFIGURAÇÕES\n", Black, BWhite);
        puts("- - - - - - - - - - -");
        printf(".:. some settings can be changed by using crefully %s\n",CONFIG_INI);
        puts("=====================================================================");
        MarkIt("  L | Tema Light                                                    \n", Blue, BWhite);
        MarkIt("  C | Tema Colorful                                                 \n", LPurple, Aqua);
        MarkIt("  D | Tema Dark                                                     \n", White, Black);
        MarkIt("  # | Criar meu Proprio Tema\n", 9+rand()%6, BackgroundColor);
        puts("--------------------------------------------------------------------");
        puts("  % | Escolher como os numeros são apresentados");
        puts("--------------------------------------------------------------------");
        puts("  + | Baixar uma nova versão do programa, se disponível");
        //puts("  ? | Saber mais sobre esta versão do programa");
        puts("--------------------------------------------------------------------");
        puts("  ! | Apagar todos os dados (Hard reset)");
        puts("  . | Carregar as configurações anteriores");
        puts("--------------------------------------------------------------------");
        puts("  0  | Salvar e voltar ao Menu principal");
        puts("---------------------------------------------------------------------");

        printf("Escolha >> "); fflush(stdin);
        op = getchar();

        switch(op){
            case 'l':case 'L':  // Tema Light
                strcpy(_theme[0],"f0");strcpy(_theme[1],"f1");strcpy(_theme[2],"1f");
                strcpy(_theme[3],"f3");strcpy(_theme[4],"8f");  createTheme(_theme);
                break;
            case 'd':case 'D':  // Tema Dark
                strcpy(_theme[0],"0f");strcpy(_theme[1],"07");strcpy(_theme[2],"08");
                strcpy(_theme[3],"0e");strcpy(_theme[4],"70");  createTheme(_theme);
                break;
            case 'c':case 'C':  // Tema Colorful
                strcpy(_theme[0],"3f");strcpy(_theme[1],"5f");strcpy(_theme[2],"4f");
                strcpy(_theme[3],"8b");strcpy(_theme[4],"cf");  createTheme(_theme);
                break;
            case '#':{
                my_theme(_theme);
                createTheme(_theme);
                break;
            }
            case '%':{
                CLEAR_SCREEN;
                heading("ALTERAR EXIIÇÃO DE NUMEROS","* Definir como os numeros flutuantes são lidos e exibidos",
                        "* Utilize: <comma> <virgula> <period> <ponto> <locale> `default`"
                );
                puts("Por exemplo: `period`, `virgula`, `default`");
                printf("\n<atual: %s> <%.2f>\n\n",CONFIG.numeric, -123.45);
                printf("LC_NUMERIC << "); fflush(stdin);
                scanf("%s", text_input);
                if ( strcmp(text_input,"0")==0 )    break;
                else if( strcmp(text_input,"ponto")==0   || strcmp(text_input,".")==0) strcpy(CONFIG.numeric,"period");
                else if( strcmp(text_input,"virgula")==0 || strcmp(text_input,",")==0) strcpy(CONFIG.numeric,"comma");
                else strcpy(CONFIG.numeric,text_input);
                break;
            }
            case '+':{
                softwareUpdate(LINK);
                break;
            }
            case '.':{
                configLoad();
                system(CONFIG.color[3]);
                printf("LOADED!\b");
                WAIT(0.5);
                break;
            }
            case '!':{
                if( erase_all_data() ){
                    puts("A resetar e sair...");
                    WAIT(1);
                    exit(EXIT_SUCCESS);
                }
                puts("Operação Cancelada!");
                op = '0';
            }
        }

    }while(op!='0');
    fflush(stdin);
    configSave();
    return;
}

void menu_about(){
    CLEAR_SCREEN;
    short int hand= 20;
    system(CONFIG.color[4]);
    printf("               ");MarkIt(" GERENCIADOR DE DISCIPLINA ESCOLAR\n", BWhite,Aqua);
    puts("                 ------------------------------");
    puts("                       23 de Junho de 2021\n");
    puts("  Enquadrado com a disciplina de Algoritmo e Estrutura de Dados,");
    puts("    para o projeto final do mesmo, desenvolvido em linguagem C,");
    puts("     este programa teve inicio com proposito de ajudar alunos");
    puts("       na gestão de suas notas, eventos,ou seja, disciplinas");
    puts("         mesmo que por base de uma interface simples.\n");
    puts("                         DESENVOLVEDORES");
    puts("                  *---------------------------*");
    puts("                   |_ Erilando T.S Carvalho _|");
    puts("                     |_   Fábio Perreira  _|");
    puts("                       |_    Irany M.   _|\n\n");
    typeWrite("          Agradecimento ao professor José Olavo Teixeira!\n",hand);
    printf("                                 ");
    PAUSE_SCREEN;
    return;
}
/**< SUBMENUS */
void overview_disciplinas(SUBJECTS* *Disciplinas){
    char ord = 'r';
    do{CLEAR_SCREEN;
        puts(" Disciplina   | Lv | SS | Professor              | Avaliacao");
        puts("---------------------------------------------------------------------");
        switch(ord){
            case '\n':{
                return;
            }
            case '1':case 'a':case 'A':
                if(*Disciplinas == NULL)puts("\t\t\t    Lista Vazia");
                else listar_disciplinas_asc(*Disciplinas,*Disciplinas);
                puts("---[A~Z]-------------------- ascendente -----------------------------");
                break;
            case '2':case 'd':case 'D':
                if(*Disciplinas == NULL)puts("\t\t\t    Lista Vazia");
                else listar_disciplinas_desc(*Disciplinas,*Disciplinas);
                puts("---[Z~A]-------------------- descendente ----------------------------");
                break;
            case '3':case 'r':case 'R':
                if(*Disciplinas == NULL)puts("\t\t\t    Lista Vazia");
                else listar_disciplinas_root(*Disciplinas,*Disciplinas);
                puts("---[R<>]--------------------- preordem ------------------------------");
                break;
            case '?':case 'l':case 'L':
                puts("Disciplina -> Nome/Título formato acronimo da disciplina");
                puts("Lv         -> Nivel de profundidade da disciplina na estrutura");
                puts("SS         -> Semestre pertencente a referida disciplina");
                puts("Professor  -> Nome do professor da disciplina");
                puts("Avaliacao  -> Refere-se a agenda da disciplina, se 100% entao ela mostra o estado aprovado/reprovado.");
                puts("---[???]--------------------- Legenda ------------------------------");
                break;
            default:
                printf("\t\t\tVISUALIZAR DISCIPLINAS");
        }
        puts("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        puts("[1|A] Ascendente  | [2|D] Descendente  | [3|R] Preordem  | [0] Voltar");
        puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        printf("Ordem >> ");fflush(stdin);
        ord = getchar();
    }while(ord != '0');
    return;
}

void submenu_filtrar_disciplina(SUBJECTS* Disciplinas, char mode, float media){
    CLEAR_SCREEN;
    switch(mode){
    case '<':case '-':
            printf("DISCIPLINAS COM MEDIA INFERIORES A %.1f:\n\n", media);
            puts(" DISCIPLINA      |    ESTADO    | MEDIA\n------------------------------------------------");
            filtrar_disciplina(Disciplinas,media,false);
            break;
        case '>':case '+':
        default:
            printf("DISCIPLINAS COM MEDIA SUPERIORES A %.1f:\n\n", media);
            puts(" DISCIPLINA      |    ESTADO    | MEDIA\n------------------------------------------------");
            filtrar_disciplina(Disciplinas,media,true);
    }
    puts("------------------------------------------------");
    return;
}

void my_theme(char _theme[5][2]){
    int c;
    char color[3];
    CLEAR_SCREEN;
    SetColor(BWhite,Blue);
    heading("PERSONALIZAR MEU TEMA",
            "* crie seu tema para o programa, para cada menu, do seu jeito",
            "* Seguindo a tabela, escolha duas cores na ordem: [Fundo][Textos]"
    );
    SetColor(BWhite,0);  printf("\t  %X   ",0);      // para mostrar cor preto que não aparece 0
    for(c = 1; c < 0x10; c++){
        SetColor(Black,c);
        printf("\t  %X   ",c);
    }
    SetColor(BWhite,Blue);
    putchar('\n');
    puts("Escolha ma combinação de cores diferentes para Fundo e Texto.");
    puts("Exemplo (fundo azul e texto branco): 1F");
    puts("00 << Voltar.\n");
    // escolher cores
    printf("Cores para menu Principal    >> FT\b\b");  fflush(stdin);   scanf("%s", color);
        strcpy(_theme[0], color);
    printf("Cores para menu Estudante    >> FT\b\b");  fflush(stdin);   scanf("%s", color);
        strcpy(_theme[1], color);
    printf("Cores para menu Disciplina   >> FT\b\b");  fflush(stdin);   scanf("%s", color);
        strcpy(_theme[2], color);
    printf("Cores para menu Configuracao >> FT\b\b");  fflush(stdin);   scanf("%s", color);
        strcpy(_theme[3], color);
    printf("Cores para menu About us     >> FT\b\b");  fflush(stdin);   scanf("%s", color);
        strcpy(_theme[4], color);
    printf("\nOK!:  ");
    WAIT_PRESSKEY;
    return;
}
