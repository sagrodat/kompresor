#include "print.h"
#include "globals.h"
#include "misc.h"
#include <stdio.h>

void print_help()
{
	printf("-------HELP-------\n");
	printf("Program sluzy do kompresji.\n");
	printf("Do podstawowego wywolania programu potrzebne jest podanie pliku wejsciowego i wyjsciowego.\n");
	printf("Program rozpoznaje czy plik jest skompresowany.\n");
	printf("> nie jest skompresowany - program kompresuje plik (domyslnie kompresja osmiobitowa)\n");
	printf("> jest skompresowany - program dekompresuje plik\n");
	printf("Jest mozliwosc wyboru poziomu kompresji.\n");
	printf("Program posiada ponizsze funkcjonalnosci:\n");
	printf("-e kompresja osmiobitowa\n");
	printf("-t kompresja dwunastobitowa\n");
	printf("-s kompresja szesnastobitowa\n");
	printf("-p szyfrowanie; po wpisaniu flagi wpisujemy haslo\n");
	printf("-i wyswietlanie dodatkowych informacji odnosnie przebiegu programu\n");
	printf("-h, h, ?, -?, help, -help, --help instrukcja obslugi programu\n");
	printf("FLAGA DOTYCZACA KOMPRESJI ZAWSZE POWINNA BYC PODAWANA JAKO OSTATNIA!\n");
	printf("Przyklady uzycia :\n");
	printf("(1)\n");
	printf("in.txt out.txt -p haslo [kompresja 8 bitowa z haslem]\n");
	printf("out.txt in.txt -i [dekompresja z wyswietleniem dodatkowych informacji]\n");
	printf("(2)\n");
	printf("photo.png photo_compressed.bin -i -t [kompresja 12 bitowa z wyswietleniem dodatkowych informacji]\n");

	exit(0);
}


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

void print_decompression_success(char control_number,FILE *out)
{
	if (control_number == 0)
	{
		fseek(out, 0, SEEK_END);
		int fsize = ftell(out);
		if (fsize == 0 && protected)
		{
			printf("Podano nieprawidlowe haslo\n");
		}
		else
		{
			printf("\nZdekompresowano plik!\n");
		}
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