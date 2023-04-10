#define _CRT_SECURE_NO_WARNINGS
#include "file.h"
#include "globals.h"
#include "misc.h"

unsigned char read_byte_from_file(FILE* fp)
{
	unsigned char ch;
	ch = fgetc(fp);
	if (protected)
	{
		ch ^= pass_var;
	}
	return ch;
}


FILE* open_file(int argc, char** argv, int argv_num, char* flag)
{
	FILE* fp;
	fp = argc > argv_num ? fopen(argv[argv_num], flag) : NULL;
	if (fp == NULL)
	{
		if (argc < argv_num + 1)
			printf("Nie podano nazwy pliku\n");
		else
			printf("Nie udalo sie otworzyc pliku %s\n", argv[argv_num]);

		exit(0);
	}
	else
	{
		if (argv_num == 1)
		{
			printf("Otworzono plik %s\n", argv[argv_num]);
		}
		else if (argv_num == 2)
		{
			printf("Utworzono plik %s\n", argv[argv_num]);
		}
	}
	return fp;
}

int valid_initials(FILE* fp)
{
	//sprawdzanie czy sa poprawne INICJALY
	int flag1 = 0;
	int flag2 = 0;
	unsigned char ch = fgetc(fp);
	if (ch == 'D')
	{
		ch = fgetc(fp);
		if (ch == 'K')
		{
			return 1;
		}
	}
	return 0;

}

unsigned short read_two_bytes(FILE* fp)
{
	unsigned char ch;
	unsigned short tmp = 0;
	ch = fgetc(fp);
	if(protected)
		xor_char_pass(&ch, pass_var);
	tmp += ch;
	tmp = tmp << 8;
	ch = fgetc(fp);
	if (protected)
		xor_char_pass(&ch, pass_var);
	tmp += ch;
	return tmp;
}

int read_three_bytes(FILE* fp)
{
	unsigned char ch;
	int tmp = 0;
	ch = fgetc(fp);
	if (protected)
		xor_char_pass(&ch, pass_var);
	tmp += ch;
	tmp = tmp << 8;
	ch = fgetc(fp);
	if (protected)
		xor_char_pass(&ch, pass_var);
	tmp += ch;
	tmp = tmp << 8;
	ch = fgetc(fp);
	if (protected)
		xor_char_pass(&ch, pass_var);
	tmp += ch;
	//printf("rozmiar drzewa z funkcji %d\n", tmp);
	return tmp;
}

void write_two_bytes_to_file(FILE* fp, short x)
{
	union data u;
	u.buf = x;
	if (protected)
	{
		xor_char_pass(&u.D.B, pass_var);
		xor_char_pass(&u.D.A, pass_var);
	}
	fwrite(&u.D.B, sizeof(char), sizeof(u.D.B), fp);
	fwrite(&u.D.A, sizeof(char), sizeof(u.D.A), fp);
}
void write_three_bytes_to_file(FILE* fp, int x)
{
	union big_data u;
	u.buf = x;
	if (protected)
	{
		xor_char_pass(&u.E.C, pass_var);
		xor_char_pass(&u.E.B, pass_var);
		xor_char_pass(&u.E.A, pass_var);
	}

	fwrite(&u.E.C, sizeof(char), sizeof(u.E.C), fp);
	fwrite(&u.E.B, sizeof(char), sizeof(u.E.B), fp);
	fwrite(&u.E.A, sizeof(char), sizeof(u.E.A), fp);
}


char get_control_number(FILE* fp)
{
	int cur_pos = ftell(fp);
	fseek(fp, -1, SEEK_END);
	unsigned char cn = read_byte_from_file(fp);
	fseek(fp, cur_pos, SEEK_SET);
	return cn;
}


void save_initials(FILE* fp)
{
	char initials[2] = { 'D','K' };
	fwrite(initials, sizeof(char), sizeof(initials), fp);
}


int get_file_size(FILE* fp)
{
	int cur_pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	rewind(fp);
	fseek(fp, cur_pos, SEEK_SET);
	return fsize;
}

void write_byte_to_file(FILE* fp, char x)
{
	if (protected)
	{
		x ^= pass_var;
	}
	fwrite(&x, sizeof(char), sizeof(x), fp);
}
