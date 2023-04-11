#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define EIGHT_BIT 256
#define TWELVE_BIT 4096
#define SIXTEEN_BIT 65536
#define MAX_PASS_LEN 50

extern char show_info;
extern char protected;
extern unsigned char pass_var;

typedef struct {
	short char_id; // znak badz ojciec
	int sum; // ilosc wystapien w tekscie
	int father_id; // ojciec
	int left;//lewe dziecko
	int right;//prawe dziecko
	char bit; // (0,1) do slownika
}node_t;

typedef struct {
	short char_id;
	char* code;
}keyy_t;

//do odtwarzania drzewa przy dekompresji
typedef struct {
	int node_id; // indeks z tablicy nodes
	int left; // lewe dziecko
	int right; // prawe dziecko
	unsigned short char_id; // znak
}branch_t;

typedef struct {
	int val;
	int ind;
}min_t;

union data {
	short buf;
	struct {
		char A;
		char B;
	}D;
};

union big_data {
	int buf;
	struct {
		char A;
		char B;
		char C;
		char D;
	}E;
};

#endif