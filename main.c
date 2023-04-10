#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "misc.h"
#include "print.h"
#include "file.h"
#include "typedefs.c"
#include "tree.h"
#include "getopt.h"
#include "globals.h"

int* count_occurences(FILE* fp, int fsize, int dict_size)
{
	int* count = malloc(sizeof(int) * dict_size);
	short ch;
	int i;
	for (i = 0; i < dict_size; i++)
	{
		count[i] = 0;
	}
	if (dict_size == EIGHT_BIT)
	{

		for (i = 0; i < fsize; i++)
		{
			print_progress("Zliczanie wystapien ciagow bitow : ", i, fsize - 1);
			ch = fgetc(fp);
			count[(unsigned char)ch] += 1;
		}
	}
	else if (dict_size == TWELVE_BIT)
	{
		union data u;
		u.buf = 0;
		int read_bits = 0;
		int iter = fsize * 2; // bo wczytujemy po 4 bity
		double loading_bar;
		for (i = 0; i < iter; i++)
		{
			print_progress("Zliczanie wystapien ciagow bitow : ", i, iter - 1);

			if (i % 2 == 0)
			{
				ch = fgetc(fp);
				//printf("%c\n", ch);
			}
			if (i % 2 == 0) //zapisujemy lewa polowe znaku
			{
				u.buf += (ch & 0xf0) >> 4;
				read_bits += 4;
				if (read_bits < 12 && i != iter - 1)
				{
					u.buf = u.buf << 4;
				}
			}
			else if (i % 2 == 1) // zapisujemy prawa polowe znaku
			{

				u.buf += (ch & 0x0f);
				read_bits += 4;
				if (read_bits < 12 && i != iter - 1)
				{
					u.buf = u.buf << 4;
				}
			}
			if (read_bits == 12 || i == iter - 1)
			{
				count[(unsigned short)u.buf] += 1;

				//char* code = dec_to_bin_string(u.buf, get_bin_len(u.buf));
				//printf("%d\n", u.buf);
				//print_bin_as_bits(code, get_bin_len(u.buf), read_bits);
				u.buf = 0;
				read_bits = 0;
			}

		}
	}
	else if (dict_size == SIXTEEN_BIT)
	{
		int j;
		short key;
		for (j = 0; j < (fsize / 2); j++)
		{
			print_progress("Zliczanie wystapien ciagow bitow : ", j, fsize / 2 - 1);

			key = read_two_bytes(fp);
			//printf("key %u\n", (unsigned int)key);
			count[(unsigned short)key]++;
		}
		if (fsize % 2 == 1)
		{
			key = fgetc(fp);
			count[(unsigned short)key] += 1;
		}
	}
	/*for (i = 0; i < dict_size; i++)
	{
		if (count[i] > 0)
			printf("count[%d] = %d\n", i, count[i]);
	}*/
	return count;
}
keyy_t* create_dict(node_t* nodes, int unique_chars, int MAX_VAL)
{
	int i;
	keyy_t* dict = malloc(sizeof * dict * MAX_VAL);
	//znajdywanie wielkosci najdluzszego kodu aby ogolnei zarezerwowac mniejsza ilosc miejsca na kody
	//sprawdzamy jak dlugi jest kod dla znaku ktory wystepuje najmniej razy, to bedzie max dlugosc kodu
	// 
	int max_code_len = get_max_code_len(nodes, unique_chars);
	//printf("MAX CODE LEN %d\n", max_code_len);

	//printf("%d MAX CODE LEN\n", max_code_len);

	//printf("SLOWNIK \n");
	for (i = 0; i < unique_chars; i++)
	{
		print_progress("Tworzenie slownika : ", i, unique_chars - 1);

		int index = 0; // index w tabeli z kodem
		short key = nodes[i].char_id;
		//printf("key : %d\n", key);
		dict[(unsigned short)key].code = malloc(sizeof(char) * max_code_len);

		//poczatkowo wypelnianie kodu pustymi znakami
		fill_char_arr(&(dict[(unsigned short)key].code), max_code_len, '\0');

		//znak dla ktorego znajdujemy kod
		dict[(unsigned short)key].char_id = nodes[i].char_id;

		//tworzymy kod dla danego znaku
		int j = i; // j, index ktorym przechodzimy po ojcach (idziemy do korzenia)
		while (nodes[j].father_id != -1)
		{
			//print kontrolny
			//print_node_info(nodes[j]);

			dict[(unsigned short)key].code[index] = nodes[j].bit;
			j = nodes[j].father_id;
			index++;
		}
		//printf("%d %s\n", (unsigned short)key, dict[(unsigned short)key].code);

		//odwracanie klucza (szlismy od lisci do korzenia a kod jest na odwrot)
		reverse_string(&dict[(unsigned short)key].code, index);

		//print klucza
		//print_key(dict[key], max_code_len);
	}
	return dict;

}
void write_byte_to_file(FILE * fp, char x)
{
	if (protected)
	{
		x ^= pass_var;
	}
	fwrite(&x, sizeof(char), sizeof(x), fp);
}
void save_header(FILE* fp, node_t* nodes, char tail, int fsize, int right, int COMPRESSION_MODE)
{
	save_initials(fp);

	fwrite(&protected, sizeof(char), sizeof(protected), fp);

	char compression_mode_indicator = (char)(log2f(COMPRESSION_MODE));
	write_byte_to_file(fp,compression_mode_indicator);
	write_byte_to_file(fp, tail);

	if (COMPRESSION_MODE == EIGHT_BIT)
	{
		//printf("ZAPISYWANIE NAGLOWKA \n");


		//SLOWNIK
		int i;
		for (i = right - 1; i >= 0; i--)
		{
			write_two_bytes_to_file(fp, i);

			char is_leaf = nodes[i].left == -1 ? 1 : 0; // sprawdzamy czy ma jakiekolwiek dziecko
			write_byte_to_file(fp, is_leaf);

			if (is_leaf)//znak odpowiadajacy lisciowi
			{
				char ch = nodes[i].char_id;
				write_byte_to_file(fp, ch);
			}
			else if (!is_leaf)//kolejne dzieci
			{
				write_two_bytes_to_file(fp, nodes[i].left);
				write_two_bytes_to_file(fp, nodes[i].right);
			}
		}
	}
	else if (COMPRESSION_MODE == TWELVE_BIT)
	{

		int i;
		for (i = right - 1; i >= 0; i--)
		{
			write_two_bytes_to_file(fp, i);

			char is_leaf = nodes[i].left == -1 ? 1 : 0;
			write_byte_to_file(fp, is_leaf);

			if (is_leaf)
			{
				write_two_bytes_to_file(fp, nodes[i].char_id);
			}
			else if (!is_leaf)
			{
				write_two_bytes_to_file(fp, nodes[i].left);
				write_two_bytes_to_file(fp, nodes[i].right);
			}
		}
	}
	else if (COMPRESSION_MODE == SIXTEEN_BIT)
	{
		int i;
		for (i = right - 1; i >= 0; i--)
		{
			write_three_bytes_to_file(fp, i);

			char is_leaf = nodes[i].left == -1 ? 1 : 0;
			write_byte_to_file(fp, is_leaf);

			if (is_leaf)
			{
				write_two_bytes_to_file(fp, nodes[i].char_id);
			}
			else if (!is_leaf)
			{
				write_three_bytes_to_file(fp, nodes[i].left);
				write_three_bytes_to_file(fp, nodes[i].right);
			}
		}
	}
}
void save_code(FILE* in, FILE* out, keyy_t* dict, int fsize, int unique_chars, int COMPRESSION_MODE)
{
	if (COMPRESSION_MODE == EIGHT_BIT)
	{
		int i;
		// zapisywanie bitow kodu w bajtach jako znaki
		union data u;
		u.buf = 0;
		int counter = 0;

		char control_number = 0;

		fseek(in, 0, SEEK_SET); // czytamy od poczatku plik zeby go zakodowac

		//dla kazdego znaku
		//printf("fsize %d\n", fsize);
		//printf("%d uNIQUE \n", unique_chars);
		for (i = 0; i < fsize; i++)
		{
			print_progress("Zapisywanie kodu : ", i, fsize - 1);

			//wczytaj znak i znajdz kod char
			char ch = fgetc(in);
			//printf("Znak dla ktorego szukamy %c\n", ch);
			//printf("i : %d\n", i);
			////printowanie czy xor dziala
			//char *control_number_bin = dec_to_bin_string(control_number, get_bin_len(control_number));
			//char* ch_bin = dec_to_bin_string((int)ch, get_bin_len((int)ch));
			//printf("control\n");
			//print_byte_in_bin(control_number_bin, strlen(control_number_bin));
			//printf("char\n");
			//print_byte_in_bin(ch_bin, strlen(ch_bin));

			//xorujemy znak kontrolny
			control_number = control_number ^ ch;
			//printf("i : %d", i);

			//znajdujemy kod odpowiadajacy znakowi
			//printf("ch %d, unsigned char ch %d\n", ch, (unsigned char)ch);
			char* code = dict[(unsigned char)ch].code;
			//printf("znak %c, kod %s\n", ch, dict[ch].code);


			//zamieniamy kod z stringa np "10101001" na jego wartosc dziesietna

			int code_len = (int)strlen(code);
			//printf("znak : %c, dlugosc kodu : %d\n", ch, code_len);
			int val = bin_string_to_dec(code, code_len);

			int j;
			//TEST (NOWA METODA DZIALA ALE JEST O WIELE WOLNIEJSZA, DO USPRAWNENIA)
			for (j = 0; j < code_len; j++)
			{
				u.buf = u.buf << 1;
				//printf("DODAJEMY DO SHORTA %d\n", (code[code_len - (j + 1)] - '0'));
				u.buf += (code[j] - '0');
				counter += 1;

				if (counter >= 8)
				{

					//przesun w lewo, zapisz lewy bajt, wyzeruj, przesun w prawo
					u.buf = u.buf << (16 - counter);
					write_byte_to_file(out, u.D.B);
					//printf("zapisuje %d - ", (u.B));
					//print_bin(dec_to_bin_string(u.B, get_bin_len(u.B)), get_bin_len(u.B));

					u.D.B = 0;
					u.buf = u.buf >> (16 - counter);
					counter = 0;

				}
			}
		}
		//printf("%c - %d\n", u.A, u.A);

		if (counter != 0)
		{
			//zapisz resztke 
			write_byte_to_file(out, u.D.A);
		}

		//printf("zapisuje %d\n", (u.A));
		//zapisz kontrolna sume
		write_byte_to_file(out, control_number);
		//printf("kontrol save %d\n", control_number);
		fclose(out);
	}
	else if (COMPRESSION_MODE == TWELVE_BIT)
	{
		double loading_bar;
		union data u, v;
		u.buf = 0;
		v.buf = 0;
		char control_number = 0;

		//printf("\ncontrol_nubmer : %d\n", control_number);
		fseek(in, 0, SEEK_SET);

		char ch;
		int i;

		int read_bits = 0;
		int iter = fsize * 2; // bo wczytujemy po 4 bity
		int counter = 0;
		for (i = 0; i < iter; i++)
		{
			print_progress("Zapisywanie kodu : ", i, iter - 1);

			if (i % 2 == 0)
			{
				ch = fgetc(in);
				control_number = control_number ^ ch;

				//	printf("\ncontrol_nubmer : %d\n", control_number);
					//printf("KONTROL %c\n", control_number);
			}
			if (i % 2 == 0) //zapisujemy lewa polowe znaku
			{
				u.buf += (ch & 0xf0) >> 4;
				read_bits += 4;
				if (read_bits < 12 && i != iter - 1)
				{
					u.buf = u.buf << 4;
				}
			}
			else if (i % 2 == 1) // zapisujemy prawa polowe znaku
			{

				u.buf += (ch & 0x0f);
				read_bits += 4;
				if (read_bits < 12 && i != iter - 1)
				{
					u.buf = u.buf << 4;
				}
			}
			if (read_bits == 12 || i == iter - 1)
			{
				char* code = dict[(unsigned short)u.buf].code;
				//print_bin(code, strlen(code));


				int code_len = (int)strlen(code);
				int j;
				for (j = 0; j < code_len; j++)
				{

					v.buf = v.buf << 1;
					v.buf += (code[j] - '0');
					//printf("%c ", code[j]);
					counter += 1;
					if (counter >= 8)
					{

						//	printf("\n");
						v.buf = v.buf << (16 - counter);
						write_byte_to_file(out, v.D.B);
						//printf("saved %d\n", v.B);
						v.D.B = 0;
						v.buf = v.buf >> (16 - counter);
						counter = 0;

					}
				}
				//printf("\n");
				u.buf = 0;
				read_bits = 0;
			}

		}
		if (counter != 0)
		{
			//printf("ogonek jaki zostal : %d\n", v.A);
			write_byte_to_file(out , v.D.A);
		}
		//printf("kontrol %d\n", control_number);
		write_byte_to_file(out, control_number);
		fclose(out);

	}
	else if (COMPRESSION_MODE == SIXTEEN_BIT)
	{
		int i;
		char control_number = 0;
		union data key;
		key.buf = 0;
		union data u;
		u.buf = 0;

		fseek(in, 0, SEEK_SET);

		int counter = 0;
		int iter = fsize % 2 == 0 ? fsize / 2 : (fsize / 2) + 1;
		for (i = 0; i < iter; i++)
		{
			print_progress("Zapisywanie kodu : ", i, iter - 1);

			//wczytaj znak i znajdz kod char
			if (i == iter - 1 && iter == (fsize / 2) + 1)
			{
				key.buf = fgetc(in);
				control_number = control_number ^ key.buf;
			}
			else
			{
				//key.buf = read_two_bytes(in);
				key.D.B = fgetc(in);
				key.D.A = fgetc(in);
				//printf("Wczytano %d %d\n", (unsigned char)key.D.B, (unsigned char)key.D.A);
				control_number = control_number ^ key.D.A;
				control_number = control_number ^ key.D.B;
			}

			//printf("Znak dla ktorego szukamy %c\n", ch);
			//printf("i : %d", i);

			//znajdujemy kod odpowiadajacy znakowi
			char* code = dict[(unsigned short)key.buf].code;
			//printf("key %d\n", (unsigned short)key.buf);
			//printf("code %s\n", dict[(unsigned short)key.buf].code);
			//print_bin_as_bits(code, strlen(code), 16);

			//zamieniamy kod z stringa np "10101001" na jego wartosc dziesietna
			//printf("KOD %s\n", code);
			int code_len = (int)strlen(code);
			//printf("znak : %c, dlugosc kodu : %d\n", ch, code_len);
			int val = bin_string_to_dec(code, code_len);

			int j;
			for (j = 0; j < code_len; j++)
			{
				u.buf = u.buf << 1;
				//printf("DODAJEMY DO SHORTA %d\n", (code[code_len - (j + 1)] - '0'));
				u.buf += (code[j] - '0');
				counter += 1;

				if (counter >= 8)
				{

					//przesun w lewo, zapisz lewy bajt, wyzeruj, przesun w prawo
					u.buf = u.buf << (16 - counter);
					write_byte_to_file(out, u.D.B);

					//printf("zapisuje %u - ", (unsigned int)(u.B));
					//print_bin(dec_to_bin_string(u.B, get_bin_len(u.B)), get_bin_len(u.B));

					u.D.B = 0;
					u.buf = u.buf >> (16 - counter);
					counter = 0;

				}
			}

		}
		//printf("%c - %d\n", u.A, u.A);
		//zapisz resztke 
		if (counter != 0)
		{
			//zapisz resztke 
			write_byte_to_file(out, u.D.A);
			//	printf("zapisuje %d - \n", (u.A));
			//print_bin(dec_to_bin_string(u.A, get_bin_len(u.A)), get_bin_len(u.A));
		}
		//zapisz kontrolna sume
		write_byte_to_file(out, control_number);
		//printf("kontrol save %d\n", control_number);
		fclose(out);
	}


}
unsigned char get_pass_var(char* pass)
{
	unsigned char var = 0;
	int i;
	for (i = 0; i < strlen(pass); i++)
	{
		var ^= pass[i];
	}
	return var;
}
void compress(FILE* in, FILE* out, int COMPRESSION_MODE, char* pass)
{
	fseek(in, 0, SEEK_SET); // do poczatku bo sprawdzalismy czy inicjaly w poprzednim ifie w mainie

	///////////////////////////////// ZLICZANIE WYSTAPIEN ///////////////////////////////////////

	int* count = NULL;
	int fsize = get_file_size(in);
	count = count_occurences(in, fsize, COMPRESSION_MODE);

	/////////////////////////////////// DRZEWO HUFFMANA /////////////////////////////////////////

	node_t* nodes = NULL;
	int right = 0;
	int unique_chars = get_unique_chars(count, COMPRESSION_MODE);
	if (unique_chars < 1)
	{
		fprintf(stderr, "Pusty plik! Nie mozna skompresowac\n");
		exit(0);
	}
	nodes = create_tree(count, &right, unique_chars, COMPRESSION_MODE);

	///////////////////////////////////     S£OWNIK     /////////////////////////////////////////

	keyy_t* dict = NULL;
	dict = create_dict(nodes, unique_chars, COMPRESSION_MODE);

	///////////////////////////////////      ZAPISYWANIE SKOMPRESOWANEGO TEKSTU      /////////////////////////////////////
	char tail = get_tail(nodes, dict, count, unique_chars, COMPRESSION_MODE);

	if (show_info)
		printf("Ilosc znakow w pliku : %d\n", get_file_size(in));


	/// HASLO ///
	if (protected)
	{
		pass_var = get_pass_var(pass);
	}
	
	save_header(out, nodes, tail, fsize, right, COMPRESSION_MODE);
	save_code(in, out, dict, fsize, unique_chars, COMPRESSION_MODE);
	printf("Zakonczono kompresje!\n");
}


branch_t* read_header(FILE* fp, int* tree_size, int* tail, unsigned char* control_number, int* DECOMPRESSION_MODE)
{
	branch_t* tree = NULL;

	protected = fgetc(fp);
	char pass[50];
	if (protected)
	{
		printf("Zaszyfrowany plik, prosze podac haslo \n");
		scanf("%s", &pass);
	}
	pass_var = get_pass_var(pass);
	unsigned char compression_mode_indicator = read_byte_from_file(fp);
	*DECOMPRESSION_MODE = (int)pow(2, compression_mode_indicator);
	//printf("COMPRESSION MODE  %d\n", *DECOMPRESSION_MODE);
	if (*DECOMPRESSION_MODE == EIGHT_BIT)
	{
		*tail = read_byte_from_file(fp); // wczytanie ogonka ( 3 znak)

		//zczytanie control_number z konca
		*control_number = get_control_number(fp);

		tree = read_tree_info(fp, tree_size, *DECOMPRESSION_MODE);
	}
	else if (*DECOMPRESSION_MODE == TWELVE_BIT)
	{
		*tail = read_byte_from_file(fp);

		*control_number = get_control_number(fp);

		tree = read_tree_info(fp, tree_size, *DECOMPRESSION_MODE);
	}
	else if (*DECOMPRESSION_MODE == SIXTEEN_BIT)
	{
		*tail = read_byte_from_file(fp);

		*control_number = get_control_number(fp);

		tree = read_tree_info(fp, tree_size, *DECOMPRESSION_MODE);
	}

	//obroc tablice aby indeksy dzieci sie zgadzaly z indeksami w tablicy
	reverse_tree(&tree, *tree_size);
	return tree;
}
void read_code(FILE* fp, FILE* out, branch_t* tree, int tree_size, int tail, char control_number, int DECOMPRESSION_MODE)
{
	//printf("COMPRESSION MODE %d\n", DECOMPRESSION_MODE);
	if (DECOMPRESSION_MODE == EIGHT_BIT)
	{
		unsigned char ch;

		//printf("FILE SIZE %d\n", get_file_size(fp));
		//printf("WHERE WE AT %d\n", ftell(fp));
		int chars_to_read = get_file_size(fp) - ftell(fp) - 1;

		int i;

		int tmp_ind = tree_size - 1; // do zapamietania gdzie jestesmy w drzewie (ZACZYNAMY OD ROOTA ktory jest ostatnim indeksem)


		for (i = 0; i < chars_to_read; i++)
		{
			print_progress("Czytanie kodow huffmana : ", i, chars_to_read - 1);

			ch = read_byte_from_file(fp);
			//zmiana znaku ch na binarny zapis
			char* ch_bin = dec_to_bin_string((int)ch, get_bin_len((unsigned char)ch));
			//uzupelnianie zerami z lewej
			ch_bin = add_zeros_to_left(ch_bin, get_bin_len((unsigned char)ch));
			int j;
			//	for (j = 0; j < 8; j++)
			//	{
				//	printf("%c ", ch_bin[j]);
				//}
				//printf("\n");

			branch_t tmp = tree[tmp_ind];  // zmienna do przechodzenia po drzewie

			int bin_ind = 0; // do zapamietania gdzie jestesmy w danym bajcie

			//printf("\"");

			while (bin_ind < 8)
			{

				//jesli czytamy ostatni bajt to pomijamy (8-ogonek) bitow zeby dobrze przeczytac dobrze ogonek
				if (i == chars_to_read - 1 && bin_ind == 0) // &&bin_ind == 0 aby przesunac sie tylko RAZ gdy ZACZYNAMY czytac ten ostani bajt
				{
					bin_ind += (8 - tail);
					//printf("przesuniecie ze wzlegu na ogon %d\n", tail);
					//printf("z bin[0] na bin[%d]\n", bin_ind);
				}

				//printf("bin[%d] = %c\n",bin_ind, ch_bin[bin_ind]);
			//	printf("tree id %d\n", tmp_ind);
				//printf("kids {%d,%d}\n\n", tmp.left, tmp.right);


				if (tmp.left != -1) // jezeli wezel na ktorym aktualnie znajdujemy sie w drzewie ma dzieci
				{
					move_to_child(tree, &tmp, ch_bin, &bin_ind, &tmp_ind);
				}
				if (tmp.left == -1) //jesli przeszlismy do wezla bez dziecka (bedacego lisciem) to wypiszmy znak
				{
					//wypisz znak
					//printf("%c", tmp.char_id);
					unsigned char save = tmp.char_id;
					fwrite(&save, sizeof(char), sizeof(save), out);
					control_number = control_number ^ save;
					//printf("\nkontrol %d\n", control_number);
					//cofnijmy sie do korzenia, zeby w nastepnej iteracji zaczac chodzic po drzewie od niego (ROOT)
					tmp_ind = tree_size - 1;
					tmp = tree[tmp_ind];
				}
				//printf("KONIEC ITERACJI\n\n");
			}
			//printf("\"\n");
		}

	}
	else if (DECOMPRESSION_MODE == TWELVE_BIT) // ending still kinda fucked
	{
		//printf("FILE SIZE %d\n", get_file_size(fp));
		//printf("WHERE WE AT %d\n", ftell(fp));
		int chars_to_read = get_file_size(fp) - ftell(fp) - 1;

		//printf("CHARS TO READ %d\n", chars_to_read);
		int tmp_ind = tree_size - 1;
		branch_t tmp = tree[tmp_ind];
		int i;
		int cnt = 0;
		char ch; // TU COS JEST ZLE
		union data u;
		u.buf = 0;

		//printf("\ncontrol_nubmer : %d\n", control_number);


		for (i = 0; i < chars_to_read; i++)
		{
			ch = read_byte_from_file(fp);
			//zmiana znaku ch na binarny zapis
			char* ch_bin = dec_to_bin_string((unsigned char)ch, get_bin_len((unsigned char)ch));
			//uzupelnianie zerami z lewej
			ch_bin = add_zeros_to_left(ch_bin, get_bin_len((unsigned char)ch));
			//printf("\n%d - ", (unsigned char)ch);
			//print_bin(ch_bin, 8);



			//printf("zczytano : ");
			int bin_ind = 0;
			while (bin_ind < 8)
			{
				//jesli czytamy ostatni bajt to pomijamy (8-ogonek) bitow zeby dobrze przeczytac dobrze ogonek
				if (i == chars_to_read - 1 && bin_ind == 0) // &&bin_ind == 0 aby przesunac sie tylko RAZ gdy ZACZYNAMY czytac ten ostani bajt
				{
					bin_ind += (8 - tail);
					//printf("po przesunieciu : %d\n", bin_ind);
				}
				//printf("bin[%d] = %c\n",bin_ind, ch_bin[bin_ind]);
				//printf("tree id %d\n", tmp_ind);
				//printf("kids {%d,%d}\n", tmp.left, tmp.right);

				if (tmp.left != -1) // jezeli wezel na ktorym aktualnie znajdujemy sie w drzewie ma dzieci
				{
				//	printf("bin przed : %d\n", bin_ind);
					move_to_child(tree, &tmp, ch_bin, &bin_ind, &tmp_ind);
				//	printf("bin po : %d\n", bin_ind);
				}
				//jesli przeszlismy do wezla bez dziecka (bedacego lisciem) to wypiszmy znak
				if (tmp.left == -1)
				{
					//	printf("\n");
						//wypisz znak
					//	printf("klucz %d\n", tmp.char_id);
						//print_short_as_bits(tmp.char_id, 16);
					//	printf("cnt %d\n", cnt);
					if (cnt % 2 == 0)
					{
						if (i == chars_to_read - 1 && bin_ind == 8)
						{
							//	printf("I AM IN 1 %d %d\n",i,chars_to_read);
								//printf("start operacji i%%2 == 0\n");
							u.buf += tmp.char_id;
							//print_short_as_bits(u.buf, 16);
							//printf("%c", u.A);
							fwrite(&u.D.A, sizeof(char), sizeof(u.D.A), out);
							control_number = control_number ^ u.D.A;
							//	printf("\ncontrol_nubmer : %d\n", control_number);
							cnt++;
						}
						else
						{
							//printf("I AM IN 2 %d %d\n", i, chars_to_read);
							u.buf += tmp.char_id;
							//print_short_as_bits(u.buf, 16);
							u.buf = u.buf << 4;
							//print_short_as_bits(u.buf, 16);
						 //   printf("%c", u.B);
							fwrite(&u.D.B, sizeof(char), sizeof(u.D.B), out);
							control_number = control_number ^ u.D.B;
							///		printf("\ncontrol_nubmer : %d\n", control_number);
							u.D.B = 0;
							u.buf = u.buf >> 4;
							//print_short_as_bits(u.buf, 16);
							cnt++;
						}

					}
					else if (cnt % 2 == 1)
					{
						if (i == chars_to_read - 1 && bin_ind == 8)
						{
							//printf("I AM IN 3 %d %d\n", i, chars_to_read);
							if (tmp.char_id < 32)
							{
								u.buf = u.buf << 4;
								u.buf += tmp.char_id;
								//	printf("%c", u.A);
								fwrite(&u.D.A, sizeof(char), sizeof(u.D.A), out);
								control_number = control_number ^ u.D.A;
							}
							else
							{
								u.buf = u.buf << 12;
								u.buf += tmp.char_id;
								//	printf("%c", u.B);
								fwrite(&u.D.B, sizeof(char), sizeof(u.D.B), out);
								//	printf("%c", u.A);
								fwrite(&u.D.A, sizeof(char), sizeof(u.D.A), out);


								control_number = control_number ^ u.D.B;
								control_number = control_number ^ u.D.A;
							}

							//print_short_as_bits(u.buf, 16);
							//printf("%c", u.B);
							//	printf("\ncontrol_nubmer : %d\n", control_number);
							//	printf("\ncontrol_nubmer : %d\n", control_number);
							u.buf = 0;
							cnt++;
						}
						else
						{
							//printf("I AM IN 4 %d %d\n", i, chars_to_read);
							u.buf = u.buf << 12;
							//print_short_as_bits(u.buf, 16);
							u.buf += tmp.char_id;
							//print_short_as_bits(u.buf, 16);
							//printf("%c", u.B);
							fwrite(&u.D.B, sizeof(char), sizeof(u.D.B), out);
							control_number = control_number ^ u.D.B;
							//	printf("\ncontrol_nubmer : %d\n", control_number);
							//printf("%c", u.A);
							fwrite(&u.D.A, sizeof(char), sizeof(u.D.A), out);
							control_number = control_number ^ u.D.A;
							//	printf("\ncontrol_nubmer : %d\n", control_number);
							u.buf = 0;
							cnt++;
						}

					}
					//cofnijmy sie do korzenia, zeby w nastepnej iteracji zaczac chodzic po drzewie od niego (ROOT)
					tmp_ind = tree_size - 1;
					tmp = tree[tmp_ind];
				}
				//printf("KONIEC ITERACJI\n\n");
			}
			//printf("\"\n");
		}
	}
	else if (DECOMPRESSION_MODE == SIXTEEN_BIT)
	{
		//printf("FILE SIZE %d\n", get_file_size(fp));
		//printf("WHERE WE AT %d\n", ftell(fp));
		int chars_to_read = get_file_size(fp) - ftell(fp) - 1;

		//printf("CHARS TO READ %d\n", chars_to_read);
		int tmp_ind = tree_size - 1;
		branch_t tmp = tree[tmp_ind];

		int i;
		unsigned char ch;
		for (i = 0; i < chars_to_read; i++)
		{
			print_progress("Czytanie kodow huffmana : ", i, chars_to_read - 1);
			ch = read_byte_from_file(fp);
			char* ch_bin = dec_to_bin_string((unsigned char)ch, get_bin_len((unsigned char)ch));
			ch_bin = add_zeros_to_left(ch_bin, get_bin_len((unsigned char)ch));

			int bin_ind = 0;

			while (bin_ind < 8)
			{
				//jesli czytamy ostatni bajt to pomijamy (8-ogonek) bitow zeby dobrze przeczytac dobrze ogonek
				if (i == chars_to_read - 1 && bin_ind == 0) // &&bin_ind == 0 aby przesunac sie tylko RAZ gdy ZACZYNAMY czytac ten ostani bajt
				{
					bin_ind += (8 - tail);
				}

				if (tmp.left != -1) // jezeli wezel na ktorym aktualnie znajdujemy sie w drzewie ma dzieci
				{
					move_to_child(tree, &tmp, ch_bin, &bin_ind, &tmp_ind);
				}
				if (tmp.left == -1) //jesli przeszlismy do wezla bez dziecka (bedacego lisciem) to wypiszmy znak
				{
					//wypisz znak
					union data u;
					u.buf = tmp.char_id;
					if (tmp.char_id > 255)
					{
						//printf("%c%c", u.B, u.A);
						fwrite(&u.D.B, sizeof(char), sizeof(u.D.B), out);
						fwrite(&u.D.A, sizeof(char), sizeof(u.D.A), out);
						control_number = control_number ^ u.D.B;
						control_number = control_number ^ u.D.A;
					}
					else
					{
						
						
						//printf("%c", u.A);
						//albo jest 8 bitowy znak na lisciu (gdy plik wejsciowy ma nieparzysta ilosc znakowy) 
						if (i == chars_to_read - 1) // bo bedzie on zapisany w ostatnim znaku skompresowanego pliku
						{
							fwrite(&u.D.A, sizeof(char), sizeof(u.D.A), out);
							control_number = control_number ^ u.D.A;
						}
						else //albo lewy fragment shorta jest zerowy czyli znak na lisciu to NULL
						{
							fwrite(&u.D.B, sizeof(char), sizeof(u.D.B), out);
							fwrite(&u.D.A, sizeof(char), sizeof(u.D.A), out);
							control_number = control_number ^ u.D.B;
							control_number = control_number ^ u.D.A;
						}
					
					}


					//printf("\nkontrol %d\n", control_number);
					tmp_ind = tree_size - 1;
					tmp = tree[tmp_ind];
				}
				//printf("KONIEC ITERACJI\n\n");
			}
			//printf("\"\n");
		}
	}
	print_decompression_success(control_number);
}
void decompress(FILE* in, FILE* out)
{
	int fsize = get_file_size(out);
	branch_t* tree = NULL;
	int tree_size, tail;
	unsigned char control_number;
	int DECOMPRESSION_MODE;

	tree = read_header(in, &tree_size, &tail, &control_number, &DECOMPRESSION_MODE); // zwraca juz gotowe odbudowane drzewo

	//printf("rozmiar drzewa %d\n", tree_size);
	//wypisanie odbudowanego drzewa
	/*int i;
	for (i = 0; i < tree_size; i++)
	{
		printf("node_id %d, left %d, right %d, char_id %c\n", tree[i].node_id, tree[i].left, tree[i].right, tree[i].char_id);
	}*/
	//czytanie kodu!!! (pierwszy znak wczytujemy tutaj przed petla (nie wiem czemu))
	read_code(in, out, tree, tree_size, tail, control_number, DECOMPRESSION_MODE);

}


int main(int argc, char** argv)
{


	if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "h") || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "help") || !strcmp(argv[1], "?") || !strcmp(argv[1], "-?"))
	{
		print_help();
	}


	FILE* in = open_file(argc, argv, 1, "rb"); // rb bo ten plik musi istniec
	FILE* out = open_file(argc, argv, 2, "wb");
	int COMPRESSION_MODE; // domyslnie kompresujemy osemkowo
	char compressed = 0;
	show_info = 0;
	protected = 0;

	int opt;
	char* pass = NULL;

	if (valid_initials(in))
	{
		while ((opt = getopt(argc, argv, ":p:iets")) != -1)
		{
			switch (opt)
			{
			case 'i': // info
				show_info = 1;
				break;
			default:
				break;
			}
		}

		decompress(in, out);
		return 0;
	}

	//parametery ktore trzeba znac przed kompresja/dekompresja
	while ((opt = getopt(argc, argv, ":p:iets")) != -1)
	{
		switch (opt)
		{
		case 'p': // password
			pass = optarg;
			protected = 1;
			break;


		case 'i': // info
			show_info = 1;
			break;


		case 'e': //eight
			COMPRESSION_MODE = EIGHT_BIT;
			if (show_info)
				printf("Kompresja %d bitowa\n", (int)log2f(COMPRESSION_MODE));
			compress(in, out, COMPRESSION_MODE, pass);
			compressed = 1;
			break;

		case 't': // twelve
			COMPRESSION_MODE = TWELVE_BIT;
			if (show_info)
				printf("Kompresja %d bitowa\n", (int)log2f(COMPRESSION_MODE));
			compress(in, out, COMPRESSION_MODE, pass);
			compressed = 1;
			break;

		case 's': // sixteen
			COMPRESSION_MODE = SIXTEEN_BIT;
			if (show_info)
				printf("Kompresja %d bitowa\n", (int)log2f(COMPRESSION_MODE));
			compress(in, out, COMPRESSION_MODE, pass);
			compressed = 1;
			break;

		case ':':
			printf("Jeden z parametrow potrzebuje argumentu\n");
			break;

		case '?':
			printf("Nieznany parametr: %c\n", optopt);
			break;

		default:
			break;
		}
	}
	if (!compressed) // skompresuj bez parametru
	{
		if (show_info)
			printf("Kompresja %d bitowa\n", (int)log2f(EIGHT_BIT));
		compress(in, out, EIGHT_BIT, pass);
	}

	return 0;
}

