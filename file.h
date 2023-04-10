#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "typedefs.c"
#include "globals.h"
#include "misc.h"

unsigned char read_byte_from_file(FILE* fp);

FILE* open_file(int argc, char** argv, int argv_num, char* flag);

void write_byte_to_file(FILE* fp, char x);


int valid_initials(FILE* fp);

unsigned short read_two_bytes(FILE* fp);

int read_three_bytes(FILE* fp);

void write_two_bytes_to_file(FILE* fp, short x);

void write_three_bytes_to_file(FILE* fp, int x);

void save_initials(FILE* fp);

char get_control_number(FILE* fp);

int get_file_size(FILE* fp);