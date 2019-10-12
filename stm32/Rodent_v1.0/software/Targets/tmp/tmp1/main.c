/*
Temat 3 projektu:
Lista z dynamicznie alokowaną tablicą.
Napisać program zarządzający strukturami dynamicznymi.
Niech będzie sobie pewna grupa osób kolekcjonująca monety (wymyślić kilka cech je charakteryzujących:
np. kraj pochodzenia, nazwa waluty, nominał, materiał, rozmiar, rok wybicia, lata funkcjonowania, itp).
Napisać program pozwalający „bawić się” w taką grupę. Dodawać nowe okazy, modyfikować typy, udawać wymiany, sprzedaż-kupno etc.
Przy czym monety mają być przechowywane w „klaserze” z dokładanymi i usuwanymi stronami (np. niech będzie to 6 monet na stronie).
Strony należy dokładać w miarę powiększania zbiorów i ujmować w miarę ich uszczuplania.
Program ma pozwalać w dowolnym momencie w pełni podglądać i modyfikować struktury danych.
Zaimplementować zapis i odczyt dynamicznej struktury z dysku (binarny).
Podanie przy uruchamianiu programu nazwy pliku z zapisem ma wczytać na starcie dany plik - coś w rodzaju wyboru danej kolekcji.
Wymyślić dodatkowe parametry uruchomienia i je obsłużyć - np. niech to będzie zapis archiwum monet, które dana osoba miała,
ale zostały one przekazane innemu kolekcjonerowi w ramach wymiany oraz przyporządkowana każdemu okazowi lista monet, które ten
użytkownik w tej wymianie otrzymał.
Nie wolno w programie stosować zmiennych globalnych. Program należy podzielić na pliki. Stworzyć początkowe pliki z kolekcjami i danymi.*/
#include <stdio.h>
#include <stdlib.h>
//http://www.learn-c.org/en/Linked_lists
//#include "CoinStampAlbum.h"
typedef struct node {
    char a1[20];
    char a2[20];
    struct node * next;
} node_t;

typedef struct segment_search{
	int type; //0 - |__|, 1 - | |, 2 - |‾‾, 3 - ‾‾|, 4 -   |, 5 - |   , 6 - |‾‾|
	//float time;//time driven in this segment
	struct segment_search * next;
}segment_search_t;

segment_search_t maze_array[16][16];

int UpdateMaze(int type){

}

void print_list(segment_search_t* head) {
    segment_search_t * current = head;

    while (current != NULL) {
        printf("%d\n", current->type);
        current = current->next;
    }
}

void AddItemToEnd(segment_search_t *head, int val){ //segment_search_t array[][],
    segment_search_t * current = head;

    while (current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(segment_search_t));
    current->next->type = val;
    current->next->next = NULL;
}

void AddItemToBeggining(node_t **head, char *tab1, char *tab2) { //push items
    node_t * new_node;
    new_node = malloc(sizeof(node_t));

    strcpy(new_node->a1, tab1);
    strcpy(new_node->a2, tab2);
    new_node->next = *head;
    *head = new_node;
}

int RemoveFirstItem(node_t ** head) { //popping items
    char save1[20];
    char save2[20];
    node_t * next_node = NULL;

    if (*head == NULL) {
        return 0;
    }

    next_node = (*head)->next;
    strcpy(save1, (*head)->a1);
    strcpy(save2, (*head)->a2);
    free(*head);
    *head = next_node;

    return 1;
}
void RemoveLastItem(node_t * head){
    node_t  *curr;
    curr = head->next;
    while (curr->next != NULL) {
        head = curr;
        curr = curr->next;
    }
    free(head->next);
    head->next = NULL;
}

int RemoveSpecyfic(node_t ** head, int n) { //remove by index
    int i = 0;
    char save1[20];
    char save2[20];

    node_t * current = *head;
    node_t * temp_node = NULL;

    if (n == 0) {
        return RemoveFirstItem(head);
    }

    for (i = 0; i < n-1; i++) {
        if (current->next == NULL) {
            return 0;
        }
        current = current->next;
    }

    temp_node = current->next;
    strcpy(save1, temp_node->a1);
    strcpy(save2, temp_node->a2);
    current->next = temp_node->next;
    free(temp_node);

    return 1;
}

int main()
{
    //segment_search_t * head = NULL;
     segment_search_t * head = malloc(sizeof(segment_search_t));

    if (head == NULL) { //jesli jest jest pusta to zakoncz main
        return 1;
    }
    //head = NULL;

    //head = NULL;
    //head = NULL;
    head->type = 4;
    head->next = NULL;
//    head->next = malloc(sizeof(node_t));
//    head->next->type = 2;
//    head->next->next = NULL;

    AddItemToEnd(head, 3);
    AddItemToEnd(head, 4);
    //AddItemToEnd(head, 4);
    //AddItemToBeggining(&head, "zygi", "glut");
    //RemoveFirstItem(&head);
    //RemoveLastItem(head);
    //RemoveSpecyfic(&head, 3);
    print_list(head);

    printf("herewrwr");
    return 0;
}
