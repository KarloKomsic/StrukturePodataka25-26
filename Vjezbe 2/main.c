/*
Definirati strukturu osoba (ime, prezime, godina rođenja) i napisati program
koji:
A. dinamički dodaje novi element na početak liste,
B. ispisuje listu,
C. dinamički dodaje novi element na kraj liste,
D. pronalazi element u listi (po prezimenu),
E. briše određeni element iz liste,
U zadatku se ne smiju koristiti globalne varijable.
*/

#define _CRT_SECURE_NO_WARNINGS
#define PERSON_NOT_FOUND 1
#define SUCCESS 0
#define MALLOC_ERROR -2
#define MAX_NAME_SIZE 50

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct person *Position;
typedef struct person {
  char firstName[MAX_NAME_SIZE];
  char lastName[MAX_NAME_SIZE];
  int yearOfBirth;
  Position nextElement;
} Person;

int insertAtBeginningOfList(Position head, char *firstName, char *lastName,
                            int yearOfBirth);
int printList(Position first);
int insertAtEndOfList(Position head, char *firstName, char *lastName,
                      int yearOfBirth);
Position searchByLastName(Position first, char *lastName);
int deletePersonFromList(Position head, char *lastName);

int main() {
  Person head = {
      .firstName = "", .lastName = "", .yearOfBirth = 0, .nextElement = NULL};
  int status = 0;
  Position foundPerson = NULL;

  // U ovom slucaju, prva u listi ce biti Marijana, pa onda Roko, i zatim Jozo
  status = insertAtBeginningOfList(&head, "Roko", "Dujevic", 1997);
  status = insertAtEndOfList(&head, "Jozo", "Miskovic", 1984);
  status = insertAtBeginningOfList(&head, "Marijana", "Bosnjak", 2004);

  // Printanje trenutne liste sa dodane 3 osobe
  printf("---------------------------------------------------------------------"
         "----------------------------\n");
  printf("~~~~ Lista osoba: ~~~~\n");
  printList(head.nextElement);

  // Pretrazivanje po prezimenu
  foundPerson = searchByLastName(head.nextElement, "Bosnjak");
  if (foundPerson != NULL)
    printf("\nPronadena osoba: %s %s, %d\n", foundPerson->firstName,
           foundPerson->lastName, foundPerson->yearOfBirth);
  else
    printf("\nOsoba nije pronadena.\n");

  printf("---------------------------------------------------------------------"
         "----------------------------\n");

  // Brisanje osobe
  printf("Brisanje osobe 'Miskovic'...\n");
  status = deletePersonFromList(&head, "Miskovic");
  if (status == SUCCESS)
    printf("Osoba uspjesno obrisana.\n\n");
  else
    printf("Osoba nije pronadena.\n\n");

  printf("~~~~ Lista nakon brisanja: ~~~~\n");
  printList(head.nextElement);

  return SUCCESS;
}

// Dodaje novu osobu NA POCETKU liste
int insertAtBeginningOfList(Position head, char *firstName, char *lastName,
                            int yearOfBirth) {
  Position newPerson = (Position)malloc(sizeof(Person));
  if (!newPerson) {
    printf("Greska: Alokacija memorije nije bila moguca.\n");
    return MALLOC_ERROR;
  }

  strcpy(newPerson->firstName, firstName);
  strcpy(newPerson->lastName, lastName);
  newPerson->yearOfBirth = yearOfBirth;

  // Uvezivanje na pocetak liste
  newPerson->nextElement = head->nextElement;
  head->nextElement = newPerson;

  return SUCCESS;
}

// Ispisuje listu
int printList(Position first) {
  // Ako je lista prazna, vrati error
  if (first == NULL) {
    printf("Lista je prazna");
    return PERSON_NOT_FOUND;
  }

  // Seta po listi, i zatim printa podatke od svakog clana
  while (first != NULL) {
    printf("%s %s, %d\n", first->firstName, first->lastName,
           first->yearOfBirth);
    first = first->nextElement;
  }

  return SUCCESS;
}

// Dodaje osobu NA KRAJU liste
int insertAtEndOfList(Position head, char *firstName, char *lastName,
                      int yearOfBirth) {
  Position newPerson = (Position)malloc(sizeof(Person));
  if (!newPerson) {
    printf("Greska: Alokacija memorije nije bila moguca.\n");
    return MALLOC_ERROR;
  }

  strcpy(newPerson->firstName, firstName);
  strcpy(newPerson->lastName, lastName);
  newPerson->yearOfBirth = yearOfBirth;

  // Pokazivac na adresu iduceg clana dodanog elementa je kraj
  newPerson->nextElement = NULL;

  // Sad trazimo zadnji element u listi
  Position temp = head;
  while (temp->nextElement != NULL)
    temp = temp->nextElement;

  // Kada smo nasli zadnji element, dodajemo novi element na kraj
  temp->nextElement = newPerson;

  return SUCCESS;
}

// Pronalazenje osobe po prezimenu
Position searchByLastName(Position first, char *lastName) {
  while (first != NULL) {
    /* Ako je prezime trenutne osobe identicna trazenom prezimenu,
     vrati tog clana */
    if (strcmp(first->lastName, lastName) == 0)
      return first;
    first = first->nextElement;
  }

  // Ako nije pronadeno trazeno prezime, vrati NULL
  printf("Osoba pod tim prezimenom ne postoji.\n");
  return NULL;
}

// Brisanje osobe po prezimenu
int deletePersonFromList(Position head, char *lastName) {
  Position previous = head;
  Position current = head->nextElement;

  while (current != NULL) {
    // Ako su prezimena ista, nasli smo osobu koju zelimo izbrisati
    if (strcmp(current->lastName, lastName) == 0) {
      /* Prespajanje pokazivaca tako da prethodnoj osobi dadnemo pokazivac na
      onaj element na kojeg trenutno neobrisani clan pokazuje.
      Primjer: Ivan -> Ana -> Luka, ce biti Ivan -> Luka ako maknemo Anu */
      previous->nextElement = current->nextElement;
      free(current);

      return SUCCESS;
    }
    previous = current;
    current = current->nextElement;
  }

  // Vrati error ako osoba koju probavamo izbrisati nije pronadena
  printf("Osoba koju probavate obrisati nije pronadena.\n");
  return PERSON_NOT_FOUND;
}
