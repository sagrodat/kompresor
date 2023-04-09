#include "tree.h"
#include "file.h"
#include "misc.h"
#include "print.h"
#include "globals.h"

void prepare_nodes_arr(node_t** nodes, int count[], int n) // wypelnianie poczatkowych wartosci zmiennych dla kazdego elementu node_t
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (count[i] > 0)
		{
			//printf("nodes[%d].sum = %d\n", i, count[i]);
		}
		(*nodes)[i].sum = count[i];
		(*nodes)[i].char_id = i;
		(*nodes)[i].father_id = -1;
		(*nodes)[i].left = -1;
		(*nodes)[i].right = -1;
	}

	//(*nodes)[i].char_id[1] = '\0';
}


/* funkcja przesuwajaca wszystkie elementy tablicy dla ktorych znak wystapil chociaz raz
po kolei nadpisujac NIEISTOTNE elementy (czyli takie dla ktorych znak nie wystapil)*/
void move_occuring_nodes_left(node_t** nodes, int* right, int unique_chars, int MAX_VAL)
{
	int i;
	for (i = 0; i < MAX_VAL; i++)
	{
		if (*right == unique_chars)
		{
			*right = i;
			break;
		}
		if ((*nodes)[i].sum > 0)
		{
			(*nodes)[*right].char_id = (*nodes)[i].char_id;
			(*nodes)[*right].sum = (*nodes)[i].sum;
			(*right)++;
		}
	}
}

void move_to_child(branch_t* tree, branch_t* tmp, char* ch_bin, int* bin_ind, int* tmp_ind)
{
	//printf("parametry left %d, right %d\n", tmp->left, tmp->right);
	//printf("znak (0,1) : %c\n", ch_bin[*bin_ind]);
	if (ch_bin[*bin_ind] == '0') // idziemy do lewego dziecka
	{
		(*tmp) = tree[(*tmp).left];
		(*tmp_ind) = (*tmp).node_id;
		(*bin_ind)++; // zeby w nastepnej petli byl zczytany kolejny bit 

	}
	else if (ch_bin[*bin_ind] == '1')
	{
		(*tmp) = tree[(*tmp).right];
		(*tmp_ind) = (*tmp).node_id;
		(*bin_ind)++;
	}
	//printf("moved to tree id : %d, char %c\n", *tmp_ind,(*tmp).char_id);
}


branch_t* read_tree_info(FILE* fp, int* tree_size, int COMPRESSION_MODE)
{
	int tmp;
	char is_leaf;
	//zczytaj rozmiar tablicy tree i cofnij zczytanie bo to indeks pierwszego elementu tablicy tree
	if (COMPRESSION_MODE == SIXTEEN_BIT)
	{
		tmp = read_three_bytes(fp);
		
		fseek(fp, -3, SEEK_CUR);
	}
	else
	{
		tmp = read_two_bytes(fp);
		//printf("rozmiar drzewa z funkcji %d\n", tmp);
		fseek(fp, -2, SEEK_CUR);
	}

	*tree_size = tmp + 1; // bo zczytalismy indeks
	//printf("Rezerwuje %d miejsca na drzewo\n", *tree_size);
	branch_t* tree = (branch_t*)malloc(sizeof * tree * (int)*tree_size);

	//zczytaj jej zawartosc
	int i;
	for (i = 0; i < *tree_size; i++)
	{
		print_progress("Odtwarzanie drzewa : ", i, *tree_size - 1);
		if (COMPRESSION_MODE == SIXTEEN_BIT)
		{
			tmp = read_three_bytes(fp);
		}
		else
		{ 
			tmp = read_two_bytes(fp);
			//printf("tmp : %d\n\n", tmp);

		}

		tree[i].node_id = tmp;
		is_leaf = read_byte_from_file(fp);

		if (!is_leaf)
		{
			if (COMPRESSION_MODE == SIXTEEN_BIT)
			{
				tree[i].left = read_three_bytes(fp);
				tree[i].right = read_three_bytes(fp);

			}
			else
			{
				//printf("left\n");
				tree[i].left = read_two_bytes(fp);
				//printf("right\n");
				tree[i].right = read_two_bytes(fp);
			}
		}
		else if (is_leaf)
		{
			if (COMPRESSION_MODE == EIGHT_BIT)
			{
				tree[i].char_id = read_byte_from_file(fp);
			}
			else
			{
				tree[i].char_id = read_two_bytes(fp);
				//printf("wartosc bitowa : %d\n", tree[i].char_id);
			}
			tree[i].left = -1;
			tree[i].right = -1;
		}
	}
	return tree;
}


node_t* create_tree(int* count, int* right, int unique_chars, int MAX_VAL)
{
	int i;
	node_t* nodes = (node_t*)malloc(sizeof * nodes * MAX_VAL * 2); // czemu nie dziala bez * 2 ?
	if (nodes == NULL)
	{
		printf("Nie udalo sie zarezerwowac pamieci na tablice nodes\n");
		return 0;
	}
	//wypelniamy wartosci atrybutow struktury node_t poczatkowymi wartosciami dla kazdego elementu w tabeli
	prepare_nodes_arr(&nodes, count, MAX_VAL);

	//przesuwanie elementow tablicy gdzie znak wystepuje chociaz raz na maksa w lewo (aby skrocic potem tablice jesli takie istnieja)
	*right = 0;
	move_occuring_nodes_left(&nodes, right, unique_chars, MAX_VAL);

	//dopasowanie rozmiaru tablicy do ilosci znakow ktore rzeczywiscie wystapily w szyfrowanym tekscie
	//printf("unique : %d\n", unique_chars);

	//sizeof(node_t)
	//sizeof *nodes TO JEST BARDZO WAZNE
	nodes = (node_t*)realloc(nodes, sizeof * nodes * (unique_chars + 1));

	// zmienne uzywane w petli
	int new_nodes_count = 0;
	*right = unique_chars;

	//petla dopoki wszystkie wezly poza jednym nie znajda ojca, ten bez ojca to nasz ROOT
	int fatherless = count_fatherless(nodes, *right);
	int start = fatherless;
	while (fatherless != 1)
	{
		print_progress("Tworzenie drzewa : ", (double)start - (double)fatherless + 1, (double)start - (double)1);

		//printf("\rTworzenie drzewa...");
		//znajdowanie minimum
		min_t min1, min2;
		min1 = mini1_fatherless(nodes, *right);
		min2 = mini2(nodes, *right, min1);

		//wypelnienie wartosci bit w strukturze ndoe
		fill_bit_values(nodes, min1, min2);

		//przypisanie ojca wezla
		nodes[min1.ind].father_id = *right;
		nodes[min2.ind].father_id = *right;

		//powiekszenie tablicy aby zrobic miejsce na kolejny wezel
		new_nodes_count++;
		nodes = realloc(nodes, sizeof(node_t) * (*right + 1));

		//wypelnianie informacji o kolejnym, nowym wezle (ojcu)
		prepare_new_node(&nodes, *right, min1, min2);

		//przesuwanie indeksu konczacego nasze dane dalej w prawo
		(*right)++;

		//wypisanie tablicy struktur nodes i wartosci jej atrybutow
		/*for (i = 0; i < *right; i++)
		{
			print_node_info(nodes[i]);
		}*/
		fatherless -= 1;//nadalismy ojca dwóm wezlom ale stworzylismy jeden nowy (wlasnie tego ojca)
		//printf("\n///////// KONIEC ITERACJI //////// \n\n");
	}
	//printf("\n");
	//printf("right %d\n", right);

	return nodes;
}

void prepare_new_node(node_t** nodes, int right, min_t min1, min_t min2) // funkcja wypelnia wartosci zmiennych nowego wezla bedacego ojcem juz dwoch poprzedniej stworzonych
{
	(*nodes)[right].char_id = '0' + right;
	(*nodes)[right].sum = (*nodes)[min1.ind].sum + (*nodes)[min2.ind].sum;
	(*nodes)[right].father_id = -1;
	(*nodes)[right].bit = -1;
	(*nodes)[right].left = min1.ind;
	(*nodes)[right].right = min2.ind;

	//(*nodes)[right].char_id[1] = '\0';
}

void reverse_tree(branch_t** tree, int tree_size)
{
	int i;
	for (i = 0; i < (int)((tree_size) / 2); i++)
	{
		branch_t tmp = (*tree)[i];
		(*tree)[i] = (*tree)[tree_size - (i + 1)];
		(*tree)[tree_size - (i + 1)] = tmp;
	}
}


int count_fatherless(node_t* nodes, int right) // funkcja zlicza wezly ktorym nie przypisano innego wezla jako ojca (node_id = -1 przy tworzeniu nowego elementu)
{
	int i;
	int fatherless_counter = 0;
	for (i = 0; i < right; i++)
	{
		if (nodes[i].father_id == -1)
		{
			fatherless_counter += 1;
		}
	}
	return fatherless_counter;

}

min_t mini1_all(node_t* nodes, int right)
{
	int i;
	min_t min1;
	min1.val = TMP_MAX;

	for (i = 0; i < right; i++)
	{
		if (nodes[i].sum < min1.val)
		{
			min1.val = nodes[i].sum;
			min1.ind = i;
		}
	}
	return min1;
}

min_t mini1_fatherless(node_t* nodes, int right) //funkcja znajdujaca index oraz wartosc najmniejszej wartosci w tablicy w zakresie [0,right)
{
	int i;
	min_t min1;
	min1.val = TMP_MAX;

	for (i = 0; i < right; i++)
	{
		if (nodes[i].sum < min1.val && nodes[i].father_id == -1)
		{
			min1.val = nodes[i].sum;
			min1.ind = i;
		}
	}
	return min1;
}

min_t mini2(node_t* nodes, int right, min_t min1) // funkcja znajdujaca index oraz wartosc drugiej najmniejszej wartosci w tablicy w zakresie [0,right)
{
	//min1.val to wartosc minimalna w calej tablicy
	int i;
	min_t min2;
	min2.val = TMP_MAX;

	for (i = 0; i < right; i++)
	{
		if (nodes[i].sum < min2.val && i != min1.ind && nodes[i].father_id == -1)
		{
			min2.val = nodes[i].sum;
			min2.ind = i;
		}
	}
	return min2;
}

void fill_bit_values(node_t* nodes, min_t min1, min_t min2) // pierwsze wystapienie danego ojca - 0, drugie wystapienie - 1 || potrzebne do pozniejszego stworzenia slownika
{
	nodes[min1.ind].bit = '0';
	nodes[min2.ind].bit = '1';
}
