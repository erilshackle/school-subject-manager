/** \Projecto Algoritmo e Estrutura de Dados
 *
 * \Curso: Engenharia Informática e de Computadores
 * \Tema:  Gerenciador de Disciplina (Arvore Binária)
 * \Data:  23/06/2021
 * \Devs:  Erilando Carvalho & Fábio Perreira & Irany Moreno
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>

#define CLEAR_SCREEN system("cls")
#define PAUSE_SCREEN system("pause")

#include "student.h"


void menu_estudante(STUDENT* aluno);
void menu_disciplina(STUDENT* Aluno);
void menu_sobre();

void menu_listar_disciplina(SUBJECTS* *disciplina);

int main()
{
    /* Display */
    system("title GERENCIADOR DE DISCIPLINAS & mode 80,25");
    setlocale(LC_ALL, "Portuguese");

    /* Criar estrutura aluno. a variavel disciplina é o apontador para o inicio da estruttra disciplina */
    STUDENT Aluno = {
        .nome = "Meu Nome",
        .curso= "Meu Curso",
        .ID = "xxxxxx",
        .disciplina = NULL
    };
    carregar_dados(&Aluno);

/* utilizados para teste direto!

    inserir_disciplina( &(Aluno.disciplina),"AED", "Olavo", false);
    inserir_disciplina( &(Aluno.disciplina),"ACOM", "Armindo", false);
    inserir_disciplina( &(Aluno.disciplina),"MD", "Nelson", false);
    inserir_disciplina( &(Aluno.disciplina),"MV", "Jorge", false);
    inserir_disciplina( &(Aluno.disciplina),"AM2", "Adilson", false);
*/

    char op;
    /* Main Menu */
    do{
        CLEAR_SCREEN;
        system("color F0");
        time_t tempo = time(NULL);printf("\t%s", ctime(&tempo));   // Mostrar a data atual
        puts("----------------------------------------------");
        puts(" 1 | Menu do Estudante                    (E) ");
        puts("----------------------------------------------");
        puts(" 2 | Gerir Disciplinas                    (D) ");
        puts("----------------------------------------------");
        puts(" 3 | Saber mais sobre o programa          (I) ");
        puts("----------------------------------------------");
        puts(" 0 | Sair e salvar dados                      ");
        puts("----------------------------------------------\n");
        printf("Escolher -> ");
        fflush(stdin);
        scanf("%c", &op);

        switch(op){

            case 'e':case 'E':
            case '1':{
                menu_estudante(&Aluno);
                break;
            }
            case 'd':case 'D':
            case '2':{
                menu_disciplina(&Aluno);
                break;
            }
            case 'i':case 'I':
            case '3':{
                menu_sobre();
                break;
            }
        }


    }while(op!='0');
    puts("A sair...");
    salvar_dados(&Aluno);
    return 0;
}

void menu_estudante(STUDENT* aluno){
        system("color f1");
    char op, text_input[MAX_NAME];
    int  i, data_input[5], num_input;
    float real_input;
    do{
        CLEAR_SCREEN;
    printf("Nome        : %s\n", aluno->nome);
    printf("Curso       : %s\n", aluno->curso);
    printf("ID          : %s\n", aluno->ID);
    printf("Disciplinas : %d\n", _NUM_DISCIPLINAS_ATUAL);
    printf("Media final : %.2lf\n", aluno->MediaFinal);
    puts("=============================================");
    puts(" N >> Alterar Nome             ");
    puts(" I >> Alterar ID               ");
    puts(" C >> Alterar Curso            ");
    puts(" A >> Calcular Maedia Final    ");
    puts("-------------------------------");
    puts(" 1 -> Listar minhas disciplinas");
    puts(" 2 -> Ver minha agenda\n");
    puts(" 3 -> Agendar Teste");
    puts(" 4 -> Modificar info Teste");
    puts(" 5 -> Inserir Nota\n");;
    puts(" 6 -> Ver meu status");
    puts(" 7 -> exportar info para arquivo\n");
    puts(" 0 -> Voltar");
    puts("-------------------------------");
    printf("escolha: "); fflush(stdin);
    scanf("%c", &op);

    switch(op){

        case 'n':case 'N':{ // Mudar Nome do Aluno
            CLEAR_SCREEN;
            printf("nome atual: %s\n", aluno->nome);
            printf("Digite seu novo Nome: (max. %i caracteres.)\n", MAX_NAME);
            fflush(stdin);  gets(text_input);
            set_student_nome(aluno,text_input);
            break;
        }
        case 'c':case 'C':{ // Mudar o curso do Aluno
            CLEAR_SCREEN;
            printf("nome do curso: %s\n", aluno->curso);
            printf("Digite o novo nome do curso: (max. %i caracteres.)\n", MAX_NAME);
            fflush(stdin);  gets(text_input);
            set_student_curso(aluno,text_input);
            break;
        }
        case 'i':case 'I':{ // Mudar o ID do Aluno
            CLEAR_SCREEN;
            printf("ID registrado: %s\n", aluno->ID);
            printf("Digite seu novo ID: ..........\b\b\b\b\b\b\b\b\b\b");
            fflush(stdin);  gets(text_input);
            set_student_ID(aluno,text_input);
            break;
        }
        case 'a':case 'A':{
            CLEAR_SCREEN;
            calcular_Media_Final(aluno,aluno->disciplina);
            printf("Media fnal do aluno Atualizado!\n");
            printf("%.2lf\n\n", aluno->MediaFinal);

            PAUSE_SCREEN;
            break;
        }
        case '1':{    // Listar todas as disciplinas (pre-ordem in-ordem pos-ordem)
            menu_listar_disciplina( &(aluno->disciplina) );
            break;
        }
        case '2':{  // ver todos os eventos para-fazer do aluno
            CLEAR_SCREEN;
            puts("MINHA AGENDA");
            puts("- - - - - - -");
            ver_agenda(aluno->disciplina);
            PAUSE_SCREEN;
            break;
        }
        case '3':{  // Agendar um teste
            CLEAR_SCREEN;
            // buscar pela disciplina
            printf("Insira o nome da disciplina: ");    fflush(stdin);
            gets(text_input);
            SUBJECTS* disciplina = buscar_disciplina(aluno->disciplina, text_input);
            if(disciplina == NULL){
                puts("Erro! verifique o nome da disciplina.\n");
                PAUSE_SCREEN;
                break;
            }
            printf("total testes da disciplia: %d\n",disciplina->numTestes);
            puts("- - - - - - - - - - - - - - -");
            // informar dados
            printf("Dia     (int) : "); scanf("%d", &data_input[0]);
            printf("Mes     (int) : "); scanf("%d", &data_input[1]);
            printf("Ano     (int) : "); scanf("%d", &data_input[2]);
            printf("Hora    (int) : "); scanf("%d", &data_input[3]);
            printf("Minutos (int) : "); scanf("%d", &data_input[4]);
            printf("peso do teste (int) : "); scanf("%d", &num_input);
            if (agendar_teste(disciplina, disciplina->numTestes, data_input, num_input) )
                printf("Teste agendado com sucesso!\n");
            else
                printf("Falha ao agendar teste!");
            PAUSE_SCREEN;
            break;
        }

        case '4':{  // Modificar informações do teste
             // buscar pela disciplina
            CLEAR_SCREEN;
            printf("Qual disciplina? ");    fflush(stdin);
            gets(text_input);
            SUBJECTS* disciplina = buscar_disciplina(aluno->disciplina, text_input);
            if(disciplina == NULL){
                puts("Erro! verifique o nome da disciplina.\n");
                PAUSE_SCREEN;
                break;
            }
            puts("- - - - - - - - - - - - - - -");
            for(i = 0; i< disciplina->numTestes; i++)
                printf("%i >> Teste de %s\n",i+1, showDateOf(disciplina->teste[i].data) );
            puts("0 >> cancelar");
            puts("- - - - - - - - - - - - - - -");
            printf("Escolher: "); scanf("%d", &num_input);
            // se escolher 0: sair
            if(num_input == 0)break;
            // informar dados
            if(disciplina->teste[num_input-1].feito == false){
                // caso o teste não tiver sido feito
                printf("Dia     (%d) : ",disciplina->teste[num_input-1].data[0]); scanf("%d", &data_input[0]);
                printf("Mes     (%d) : ",disciplina->teste[num_input-1].data[1]); scanf("%d", &data_input[1]);
                printf("Ano     (%d) : ",disciplina->teste[num_input-1].data[2]); scanf("%d", &data_input[2]);
                printf("Hora    (%d) : ",disciplina->teste[num_input-1].data[3]); scanf("%d", &data_input[3]);
                printf("Minutos (%d) : ",disciplina->teste[num_input-1].data[4]); scanf("%d", &data_input[4]);
                printf("peso do teste (%d%%) : ",disciplina->teste[num_input].peso); scanf("%d", &num_input);

                if (agendar_teste(disciplina, disciplina->numTestes, data_input-1, num_input) )
                    printf("Teste agendado com sucesso!\n");
                else
                    printf("Falha ao agendar teste!");
            } else {
                // caso o teste já tiver sido feito
                printf("Nota do teste (%.2f) : ",disciplina->teste[num_input-1].nota); scanf("%f", &real_input);
                modificar_nota(disciplina, num_input-1, real_input);
            }
            PAUSE_SCREEN;
            break;
        }

        case '5':{  //  Inserir nota para disciplina
            // buscar pela disciplina
            CLEAR_SCREEN;
            printf("Insira o nome da disciplina: ");    fflush(stdin);
            gets(text_input);
            SUBJECTS* disciplina = buscar_disciplina(aluno->disciplina, text_input);
            if(disciplina == NULL){
                puts("Erro! verifique o nome da disciplina.\n");
                PAUSE_SCREEN;
                break;
            }
            puts("- - - - - - - - - - - - - - -");
            for(i = 0; i< disciplina->numTestes; i++)
                printf("%i >> Teste de %s (%.1f)\n",i+1, showDateOf(disciplina->teste[i].data), disciplina->teste[i].nota );
            puts("0 >> cancelar");
            puts("- - - - - - - - - - - - - - -");

            try_again:
            printf("Escolha um teste >> ");
                scanf("%d", &num_input);
            // prevenção de erros na hora da escolha
            if(num_input <= 0 || num_input > disciplina->numTestes ){
                if( num_input == 0 ) break;
                puts("escolha um numero de teste valido!\n");
                goto try_again;
            }
            printf("Entre com a nota do %i*teste: ", num_input);
                scanf("%f", &real_input);

            if (inserir_nota(disciplina, num_input-1, real_input) ) // num_input-1 para poder aceder o indice valido
                puts("Nota do teste validado com sucesso!\n");
            else
                puts("Falha ao validar a nota do teste!");
            PAUSE_SCREEN;
            break;
        }

        case '6':{  // ver meu status
            CLEAR_SCREEN;
                puts("MEU STATUS\n- - - - - - -");
                ver_status(aluno->disciplina);
                puts("- - - - - - -");
                printf("Total de testes feitos   : %d\n", soma_testes_feito(aluno->disciplina));
            // nesse passo chamamos a função dcp_media_alta() para mandar para string str_media_alta a disciplina que comtem a media mais alta
            dcp_media_alta(aluno->disciplina, 0);
                printf("Disciplina de média alta : %s\n", str_media_alta);
                printf("Media Final no momento   : %.2lf\n\n", aluno->MediaFinal);
            PAUSE_SCREEN;
            break;
        }

        case '7':{
            CLEAR_SCREEN;
            puts("EXPORTAR MINHAS INFORMAÇÕES");
            puts("- - - - - - - - - - - - - -");
            printf("escolha o nome do arquivo:\n");
            fflush(stdin); gets(text_input);
            if(exportar_info_arquivo(aluno, text_input)){
                printf("Dados exportados com sucesso para %s\n", text_input);
            }else{
                printf("Falha ao extar informações para arquivo %s\n", text_input);
            }
            PAUSE_SCREEN;
            break;
        }

    }
    }while(op!='0');
    fflush(stdin);
}

void menu_disciplina(STUDENT* Aluno){
    system("color f1");
    char op, text_input[MAX_NAME];
    float real_imput;
    SUBJECTS* *Disciplina = &(Aluno->disciplina);

    do{
       CLEAR_SCREEN;
       puts("GERIR DISCIPLINAS");
       puts("- - - - - - - - - -");
       printf("Disciplinas: %d \t raiz(%s)\n", _NUM_DISCIPLINAS_ATUAL, (*Disciplina)->titulo);

       puts("------------------------------------------------");
       puts(" 1 / L | Listar Disciplinas                  [.]");
       puts("------------------------------------------------");
       puts(" 2 / + | Adicionar Disciplinas               [A]");
       puts("------------------------------------------------");
       puts(" 3 / - | Eliminar uma Disciplina             [E]");
       puts("------------------------------------------------");
       puts(" 4 / T | Mudar Nome do Professor                ");
       puts("------------------------------------------------");
       puts(" 5 / P | Buscar por Nivel de Profundidade       ");
       puts("------------------------------------------------");
       puts(" 6 / M | Filtrar Disciplinas por media          ");
       puts("------------------------------------------------");
       puts("   0   | Voltar                                 ");
       puts("------------------------------------------------");
       printf("Escolha: ");
       fflush(stdin); scanf("%c", &op);

        switch(op){

            case '1':case 'l':case'L':case'.':{
                menu_listar_disciplina(&(*Disciplina));
                break;
            }

            case '2':case 'a':case'A':case'+':{
                char input_title[15], input_teacher[MAX_NAME];
                CLEAR_SCREEN;
                puts("Digite o nome curto da disciplia.\nex: \"AED\" em vez de \"Alg. Estr. de Dados\"");
                puts("----------------------------------------");
                printf("Degiste o nome curto da disciplina:\n");
                fflush(stdin);gets(input_title);

                printf("Degite o nome do docente da disciplina:\n");
                fflush(stdin);gets(input_teacher);

                convert_Upercase(input_title); // converter a string para maiuscula para poder haver uniformidade

                inserir_disciplina(&(Aluno->disciplina), input_title, input_teacher, false);
                break;
            }

            case '3':case 'e':case'E':case'-':{
                CLEAR_SCREEN;
                puts("Qual disciplina deseja eliminar?:");
                fflush(stdin); gets(text_input);

                convert_Upercase(text_input);

                if( eliminar_disciplina(&(Aluno->disciplina), text_input) ){
                    puts("Sucesso: Disciplina Eliminado");
                } else {
                    puts("Falha: Disciplina a eliminar não econtrada!");
                }
                break;
            }

            case '4':case 't':case 'T':{
                CLEAR_SCREEN;
                printf("Professor de qual disciplina?: ");
                fflush(stdin); gets(text_input);
                convert_Upercase(text_input);
                SUBJECTS* busca = buscar_disciplina( Aluno->disciplina , text_input );
                if( busca != NULL){
                    printf("Professor de <%s> é %s!\n\n", busca->titulo, busca->teacher);
                    printf("Entre com o novo nome: ");
                    fflush(stdin); gets(text_input);
                    strcpy(busca->teacher, text_input);
                    puts("OK!\n");
                }else{
                    printf("Disciplina <%s> não encontrada!", text_input);
                }
                PAUSE_SCREEN;
                break;
            }

            case '5':case 'p':case'P':{
                CLEAR_SCREEN;
                printf("Entre com o nome da disciplina: ");
                fflush(stdin); gets(text_input);
                convert_Upercase(text_input);
                int profundidade = Nivelprofundidade( Aluno->disciplina , text_input );
                if( profundidade == -1){
                    printf("Disciplina não existe na Arvore!\n\n");
                }else{
                    printf("O nivel de profundidade de <%s> na arvore é: %d\n\n", text_input,profundidade);
                }
                PAUSE_SCREEN;
                break;
            }

            case '6':case 'm':case'M':{
                real_imput = -1.0;  // para começar sem mostrar nada
                CLEAR_SCREEN;
                puts("FILTRAR DISCIPLINA POR MEDIA\n");
                printf("entre com uma media: ");
                scanf("%f", &real_imput);

                puts("- - - - - - - - - - - - - - - - - - - - - -");
                filtrar_disciplina(Aluno->disciplina, real_imput);
                puts("- - - - - - - - - - - - - - - - - - - - - -");
                PAUSE_SCREEN;
                break;
            }
        }
    }while(op!='0');
    fflush(stdin);
}

// MENU para Gerir arquivos dos dados (Guardar / Carregar / Resetar);
void menu_sobre(){
    CLEAR_SCREEN;
    system("color 1f");
    puts("                        GERENCIADOR DE DISCIPLINA ");
    puts("                    -------------------------------");
    puts("                          23 de Junho de 2021\n");
    puts("       Em enquadramento com a disciplina de Algoritmo e Estrutura de Dados");
    puts("   direcionada com linguagem C, este programa foi desenvolvido com proposito");
    puts("    de Ajudar alunos na gerenciação de suas notas, mesmo que por base de uma");
    puts("                            interface simples.\n");
    puts("                             Desenvolvedores");
    puts("                        ------------------------- ");
    puts("                            Erilando Carvalho");
    puts("                             Fábio Perreira");
    puts("                              Irany Moreno\n\n");
    puts("Agradecimento ao professor José Olavo Teixeita.\n\n\n");
    PAUSE_SCREEN;
    return;
}

void menu_listar_disciplina(SUBJECTS* *disciplina){
    char ordem = '1';
    do{
        CLEAR_SCREEN;
        puts(" Disciplina   | Professor       | Testes | Media");
        puts("---------------------------------------------------------------------");
        switch(ordem){  // de acordo com a ordem escolhida assim sera mostrada a lista em:
            case '1':
                listar_disciplinas_pre(*disciplina);
                puts("---------------------------- pre-ordem ------------------------------");
                break;
            case '2':
              listar_disciplinas_in(*disciplina);
              puts("----------------------------  in-ordem ------------------------------");
                break;
            case '3':
                listar_disciplinas_pos(*disciplina);
                puts("---------------------------- pos-ordem ------------------------------");
                break;
            default:
                puts("------------------- Escolha uma ordem de listagem -------------------");
        }
        puts("1 >> listar pre-ordem");
        puts("2 >> listar in-ordem");
        puts("3 >> listar pos-ordem");
        puts("0 >> voltar");
        fflush(stdin);scanf("%c", &ordem);
    }while(ordem != '0');
    return;
}
