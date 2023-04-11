#ifndef _TREE_H
#define _TREE_H

#include "file.h"
#include "misc.h"
#include "print.h"
#include "globals.h"

branch_t* read_tree_info(FILE* fp, int* tree_size, int COMPRESSION_MODE);

void move_occuring_nodes_left(node_t** nodes, int* right, int unique_chars, int MAX_VAL);

void prepare_nodes_arr(node_t** nodes, int count[], int n);

void move_to_child(branch_t* tree, branch_t* tmp, char* ch_bin, int* bin_ind, int* tmp_ind);

node_t* create_tree(int* count, int* right, int unique_chars, int MAX_VAL);

void prepare_new_node(node_t** nodes, int right, min_t min1, min_t min2); // funkcja wypelnia wartosci zmiennych nowego wezla bedacego ojcem juz dwoch poprzedniej stworzonych

void reverse_tree(branch_t** tree, int tree_size);

int count_fatherless(node_t* nodes, int right); // funkcja zlicza wezly ktorym nie przypisano innego wezla jako ojca (node_id = -1 przy tworzeniu nowego elementu)

min_t mini1_all(node_t* nodes, int right);

min_t mini1_fatherless(node_t* nodes, int right);

min_t mini2(node_t* nodes, int right, min_t min1);

void fill_bit_values(node_t* nodes, min_t min1, min_t min2); // pierwsze wystapienie danego ojca - 0, drugie wystapienie - 1 || potrzebne do pozniejszego stworzenia slownika

void move_occuring_nodes_left(node_t** nodes, int* right, int unique_chars, int MAX_VAL);


#endif