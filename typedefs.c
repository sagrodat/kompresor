#include "typedefs.h"

typedef struct node_t {
	short char_id; // znak badz ojciec
	int sum; // ilosc wystapien w tekscie
	int father_id; // ojciec
	int left;//lewe dziecko
	int right;//prawe dziecko
	char bit; // (0,1) do slownika
}node_t;

typedef struct key_t{
	short char_id;
	char* code;
}key_t;

//do odtwarzania drzewa przy dekompresji
typedef struct branch_t {
	int node_id; // indeks z tablicy nodes
	int left; // lewe dziecko
	int right; // prawe dziecko
	unsigned short char_id; // znak
}branch_t;

typedef struct min_t {
	int val;
	int ind;
}min_t;

union data {
	short buf;
	struct D {
		char A;
		char B;
	};
};

union big_data {
	int buf;
	struct E {
		char A;
		char B;
		char C;
		char D;
	};
};