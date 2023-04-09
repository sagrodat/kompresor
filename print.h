#pragma once

#include "typedefs.c"
void print_decompression_success(char control_number);

void print_progress(char msg[], double current, double max_val);

void print_nodes(node_t* nodes, int from, int to);

void print_node_info(node_t node);

void print_key(key_t key, int max_code_len);

void print_bin(char* bin, int n);

void print_bin_as_bits(char* bin, int n, int bits);

void print_short_as_bits(short x, int bits);