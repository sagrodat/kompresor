#ifndef _FILE_H
#define _FILE_H

#include "globals.h"
#include "misc.h"

unsigned char read_byte_from_file(FILE* fp, int flag);

FILE* open_file(int argc, char** argv, int argv_num, char* flag);

void write_byte_to_file(FILE* fp, char x, int flag);

int valid_initials(FILE* fp);

unsigned short read_two_bytes(FILE* fp, int flag);

int read_three_bytes(FILE* fp, int flag);

void write_two_bytes_to_file(FILE* fp, short x, int flag);

void write_three_bytes_to_file(FILE* fp, int x, int flag);

void save_initials(FILE* fp);

char get_control_number(FILE* fp);

int get_file_size(FILE* fp);

#endif