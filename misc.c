#include "misc.h"

int get_unique_chars(int count[], int n) // funkcja zliczajaca ilosc unikalnych znakow wystepujacych w pliku wejsciowym
{
	int i;
	int unique_chars = 0;
	for (i = 0; i < n; i++)
	{
		print_progress("Zliczanie unikalnych ciagow bitow : ", i, n - 1);

		if (count[i] > 0)
		{
			unique_chars++;
		}
	}
	
	return unique_chars;
}


char* add_zeros_to_left(char* arr, int n)
{
	char* new_arr = malloc(sizeof(char) * 8);
	if (new_arr == NULL)
	{
		malloc_error("tablice znakow w funkcji pomocniczej add_zeros_to_lef()");
	}
	int i;
	for (i = 0; i < 8 - n; i++)
	{
		new_arr[i] = '0';
	}
	int old_ind = 0;
	for (i = 8 - n; i < 8; i++)
	{
		new_arr[i] = arr[old_ind];
		old_ind++;
	}
	return new_arr;
}


int get_max_code_len(node_t* nodes, int unique_chars)
{
	min_t least_occuring = mini1_all(nodes, unique_chars);
	int max_code_len = 1;
	int i = least_occuring.ind;
	while (nodes[i].father_id != -1)
	{
		max_code_len++;
		i = nodes[i].father_id;
	}
	return max_code_len;
}

void reverse_string(char** arr, int strlen)
{
	int i;
	for (i = 0; i < (int)((strlen) / 2); i++)
	{
		char tmp = (*arr)[i];
		(*arr)[i] = (*arr)[strlen - (i + 1)];
		(*arr)[strlen - (i + 1)] = tmp;
	}
}


void fill_char_arr(char** arr, int n, char ch)
{
	int i;
	for (i = 0; i < n; i++)
	{
		(*arr)[i] = ch;
	}
}

int bin_string_to_dec(char* arr, int n)
{
	int sum = 0;
	int exp = 0;
	int i;
	for (i = n - 1; i > -1; i--)
	{
		if (arr[i] == '1')
		{
			sum += (int)pow(2, exp);
		}
		exp++;
	}
	return sum;
}

int get_bin_len(int x)
{
	int len = 0;
	while (x > 0)
	{
		x /= 2;
		len++;
	}
	return len;
}

char* dec_to_bin_string(unsigned char x, int n)
{

	char* binary = malloc(sizeof(char) * n);
	if (binary == NULL)
	{
		malloc_error("tablice znakow tworzaca zapis binarny liczby dziesietnej (funkcja dec_to_bin_string())");
	}
	int index = 0;
	while (x > 0)
	{
		if (x % 2 == 1)
		{
			binary[index] = '1';
		}
		else
		{
			binary[index] = '0';
		}
		index++;
		x /= 2;
	}
	
	reverse_string(&binary, n);
	//na koncu
	int i;
	return binary;
}

unsigned char get_pass_var(char* pass)
{
	if (pass == NULL)
	{
		fprintf(stderr,"Blad przy wczytywaniu hasla\n");
		exit(1);
	}
	else
	{
		unsigned char var = 0;
		int i;

		for (i = 0; i < strlen(pass); i++)
		{
			var ^= pass[i];
		}
		return var;
	}
	
}

int get_total_code_len(node_t* nodes, keyy_t* dict, int* count, int unique_chars)
{
	long long total_code_len = 0;
	int i;
	for (i = 0; i < unique_chars; i++)
	{
		short key = nodes[i].char_id;
		//printf("node_id %d, dlugosc kodu %d\n", nodes[i].char_id, strlen(dict[key].code));
		total_code_len += count[(unsigned short)nodes[i].char_id] * strlen(dict[(unsigned short)key].code);
	}
	//printf("Calkowita dlugosc kodu : %d\n", total_code_len);
	return total_code_len;
}

int get_tail(node_t* nodes, keyy_t* dict, int* count, int unique_chars, int COMPRESS_MODE)
{
	int total_code_len = get_total_code_len(nodes, dict, count, unique_chars);
	char tail = total_code_len % 8;
	if (tail == 0)
	{
		tail = 8; // ostatni znak jest pelny kodu a nie pusty !!
	}
	return tail;
}


unsigned char xor_pass(char* pass)
{
	unsigned char var = 0;
	int i;
	for (i = 0; i < strlen(pass); i++)
	{
		if (pass[i] != 10)
		{
			//printf("%d ^ %d\n", var, pass[i]);
			var = var ^ pass[i];
		}
		
	}
	return var;
}

void xor_char_pass(unsigned char* ch, unsigned char pass_var)
{
	(*ch) = (*ch) ^ (pass_var);
}