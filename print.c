#include "print.h"
#include "globals.h"
#include "misc.h"
#include <stdio.h>


void print_progress(char msg[], double current, double max_val)
{
	//printf("show info : %d\n", show_info);
	if (show_info)
	{
		double progress = 100 * (current / max_val);
		printf("\r%s%0.2f%%", msg, progress);

		if (current == max_val)
		{
			printf("\n");
		}
	}
}

void print_decompression_success(char control_number)
{
	if (control_number == 0)
	{
		printf("\nZdekompresowano plik!\n");
	}
	else
	{

		if (protected)
			printf("Podano nieprawidlowe haslo\n");
		else
			printf("Blad, nie udalo sie zdekompresowac pliku!\n");
	}
}

void print_short_as_bits(short x, int bits)
{
	char* code = dec_to_bin_string(x, get_bin_len(x));
	print_bin_as_bits(code, get_bin_len(x), bits);
}

void print_key(keyy_t key, int max_code_len) // output : "char - code"
{
	int i;
	printf("%d - ", key.char_id);
	for (i = 0; i < max_code_len; i++)
	{
		printf("%c", key.code[i]);
	}
	printf("\n");
}

void print_bin(char* bin, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (bin[i] == '1' || bin[i] == '0')
		{
			printf("%c ", bin[i]);
		}
	}
	printf("\n");
}

void print_bin_as_bits(char* bin, int n, int bits)
{
	if (n > bits)
	{
		printf("BLAD PRINT_BIN_AS_BITS\n");
	}
	int i;
	for (i = 0; i < (bits - n); i++)
	{
		printf("0 ");
	}
	for (i = bits - n; i <bits; i++)
	{
		printf("%c ", bin[i - (bits - n)]);
	}
	printf("\n");
}

void print_nodes(node_t* nodes, int from, int to) // wypis kontrolny, ignorowac
{
	int i;
	for (i = from; i < to; i++)
	{
		if (i == 0)
		{
			printf("Lista wystapien : \n");
		}
		printf("%c - %d (%d)\n", nodes[i].char_id, nodes[i].sum, (int)nodes[i].char_id);
	}
}

void print_node_info(node_t node)
{
	printf("char id %c - %d , ", node.char_id,node.char_id);
	printf("sum : %d , ", node.sum);
	printf("father id %d, ", node.father_id);
	printf("bit : %c ,", node.bit);
	printf("kids {%d,%d}\n", node.left, node.right);
}