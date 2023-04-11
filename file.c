#define _CRT_SECURE_NO_WARNINGS
#include "file.h"


unsigned char read_byte_from_file(FILE* fp, int flag)
{
	unsigned char ch;
	size_t count = fread(&ch, sizeof(char), sizeof(ch), fp);
	if (count == 0)
	{
		print_read_error();
	}
	if (flag && protected)
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
			fprintf(stderr,"Nie podano nazwy pliku\n");
		else
			fprintf(stderr, "Nie udalo sie otworzyc pliku %s\n", argv[argv_num]);

		exit(1);
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
	int fsize = get_file_size(fp);
	if (fsize > 2)
	{
		//sprawdzanie czy sa poprawne INICJALY
		int flag1 = 0;
		int flag2 = 0;
		unsigned char ch = read_byte_from_file(fp, 0);
		if (ch == 'D')
		{
			ch = read_byte_from_file(fp, 0);
			if (ch == 'K')
			{
				return 1;
			}
		}
	}
	return 0;
}

unsigned short read_two_bytes(FILE* fp, int flag)
{
	unsigned char ch;
	unsigned short tmp = 0;
	size_t count;
	count = fread(&ch, sizeof(char), sizeof(ch), fp);
	if (count == 0)
	{
		print_read_error();
	}

	if(protected && flag)
		xor_char_pass(&ch, pass_var);
	tmp += ch;
	tmp = tmp << 8;

	count = fread(&ch, sizeof(char), sizeof(ch), fp);
	if (count == 0)
	{
		print_read_error();
	}

	if (protected && flag)
		xor_char_pass(&ch, pass_var);
	tmp += ch;
	return tmp;
}

int read_three_bytes(FILE* fp, int flag)
{
	unsigned char ch;
	int tmp = 0;
	size_t count;
	count = fread(&ch, sizeof(char), sizeof(ch), fp);
	if (count == 0)
	{
		print_read_error();
	}
	if (protected && flag)
		xor_char_pass(&ch, pass_var);
	tmp += ch;
	tmp = tmp << 8;
	count = fread(&ch, sizeof(char), sizeof(ch), fp);
	if (count == 0)
	{
		print_read_error();
	}
	if (protected && flag)
		xor_char_pass(&ch, pass_var);
	tmp += ch;
	tmp = tmp << 8;
	count = fread(&ch, sizeof(char), sizeof(ch), fp);
	if (count == 0)
	{
		print_read_error();
	}
	if (protected)
		xor_char_pass(&ch, pass_var);
	tmp += ch;
	//printf("rozmiar drzewa z funkcji %d\n", tmp);
	return tmp;
}

void write_two_bytes_to_file(FILE* fp, short x,int flag)
{
	union data u;
	size_t count;
	u.buf = x;
	if (protected && flag)
	{
		xor_char_pass(&u.D.B, pass_var);
		xor_char_pass(&u.D.A, pass_var);
	}

	count = fwrite(&u.D.B, sizeof(char), sizeof(u.D.B), fp);
	if (count == 0)
	{
		print_write_error();
	}
	count = fwrite(&u.D.A, sizeof(char), sizeof(u.D.A), fp);
	if (count == 0)
	{
		print_write_error();
	}
}
void write_three_bytes_to_file(FILE* fp, int x,int flag)
{
	union big_data u;
	size_t count;
	u.buf = x;
	if (protected && flag)
	{
		xor_char_pass(&u.E.C, pass_var);
		xor_char_pass(&u.E.B, pass_var);
		xor_char_pass(&u.E.A, pass_var);
	}

	count = fwrite(&u.E.C, sizeof(char), sizeof(u.E.C), fp);
	if (count == 0)
	{
		print_write_error();
	}
	count = fwrite(&u.E.B, sizeof(char), sizeof(u.E.B), fp);
	if (count == 0)
	{
		print_write_error();
	}
	count = fwrite(&u.E.A, sizeof(char), sizeof(u.E.A), fp);
	if (count == 0)
	{
		print_write_error();
	}
}


char get_control_number(FILE* fp)
{
	int cur_pos = ftell(fp);
	fseek(fp, -1, SEEK_END);
	unsigned char cn = read_byte_from_file(fp,1);
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

void write_byte_to_file(FILE* fp, char x, int flag)
{
	if (protected && flag)
	{
		x ^= pass_var;
	}
	size_t count = fwrite(&x, sizeof(char), sizeof(x), fp);
	if (count == 0)
	{
		print_write_error();
	}
}
