#ifndef _MISC_H
#define _MISC_H

#include "globals.h"
#include "tree.h"
#include "print.h"
int get_unique_chars(int count[], int n);

char* add_zeros_to_left(char* arr, int n);

int get_unique_chars(int count[], int n);

int get_max_code_len(node_t* nodes, int unique_chars);

void reverse_string(char** arr, int strlen);

void fill_char_arr(char** arr, int n, char ch);

int bin_string_to_dec(char* arr, int n);

int get_bin_len(int x);

char* add_zeros_to_left(char* arr, int n);

unsigned char get_pass_var(char* pass);

char* dec_to_bin_string(unsigned char x, int n);

int get_total_code_len(node_t* nodes, keyy_t* dict, int* count, int unique_chars);

int get_total_code_len(node_t* nodes, keyy_t* dict, int* count, int unique_chars);

int get_tail(node_t* nodes, keyy_t* dict, int* count, int unique_chars, int COMPRESS_MODE);

unsigned char xor_pass(char* pass);

void xor_char_pass(unsigned char* ch, unsigned char pass_var);

#endif