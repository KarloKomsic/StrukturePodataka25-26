/*
Napisati program koji prvo pročita koliko redaka ima datoteka, tj. koliko ima
studenata zapisanih u datoteci. Nakon toga potrebno je dinamički alocirati
prostor za niz struktura studenata (ime, prezime, bodovi) i učitati iz datoteke
sve zapise. Na ekran ispisati ime, prezime, apsolutni i relativni broj bodova.
Napomena: Svaki redak datoteke sadrži ime i prezime studenta, te broj bodova na
kolokviju. relatvan_br_bodova = br_bodova/max_br_bodova*100
*/

#define _CRT_SECURE_NO_WARNINGS
#define FILE_NOT_OPENED -1
#define MAX_LINE 1024
#define MAX_SIZE 50
#define MAX_BR_BODOVA 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student
{
	char ime[MAX_SIZE];
	char prezime[MAX_SIZE];
	int brojBodova;
} Student;

int brojRetke(FILE*)
{
	return 0;
}

int ucitajZapis()
{
	return 0;
}

int ispisStudenata(Student student)
{
	printf("%s\n", student.ime);
	printf("%s\n", student.prezime);
	printf("%d\n", student.brojBodova);
	return 0;
}

float relativniBrojBodova(Student student)
{
	return (student.brojBodova / MAX_BR_BODOVA)*100;
}

int main()
{
	FILE *datoteka = fopen("studenti.txt", "r");
	return 0;
}
