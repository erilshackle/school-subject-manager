#ifndef AVALS_H
#define AVALS_H

#define NOTA_MAXIMA 20.00000

#include "display.h"
#include "subjects.h"

#include <stdbool.h>
#include <math.h>

//const int max_avals = MAX_AVAL; //  imported from subjects.h

//!void menu_gerir_aval(SUBJECTS* Disciplina);
void list_avals(SUBJECTS* DCP, bool indexed_1base);

int getAvalStatus(SUBJECTS* dcp);
int getThisAvalPeso(SUBJECTS* dcp, AVAL_TYPE _type);

/// Operações
int count_aval_type(SUBJECTS* dcp, AVAL_TYPE _type);
char* getAvalType(AVAL_TYPE _tipo);
char* getTimeTo(int date[5]);
char* getTimeLeft(int date[5]);
/// Avaliação
bool set_mark_to(SUBJECTS* DCP, int aval_idx, float *mark_debug);    // enviar NULL caso não queira debugar a nota inserida
bool un_mark_to(SUBJECTS* DCP, int aval_idx);
int marcar_avaliacao(SUBJECTS* DCP, AVAL_TYPE tipo_aval, int* _data, int peso);
bool marcar_exame(SUBJECTS* DCP, int _data[5]);
bool editar_avaliacao(SUBJECTS* DCP, int aval_idx);
bool eliminar_avaliacao(SUBJECTS* DCP, int aval_idx);

/// Aux
char* returnMiniData(int *data);
int indexOfAval(SUBJECTS* disciplina, AVAL_TYPE type);
void copyAvalTo(SUBJECTS* DCP, int to_idx, int from_idx);
bool check_data(int data[5], bool report);

void _agendar_avaliacao_(SUBJECTS* Disciplina);
void _avaliamento_(SUBJECTS* Disciplina);
#endif // AVALS_H
