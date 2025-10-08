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
#define SUCCESS 0
#define MALLOC_ERROR -2
#define FILE_READ_ERROR 3
#define MAX_LINE 1024
#define MAX_NAME_SIZE 50
#define MAX_POINTS 20

#include <stdio.h>
#include <stdlib.h>

typedef struct student {
  char ime[MAX_NAME_SIZE];
  char prezime[MAX_NAME_SIZE];
  double brojBodova;
  double relativanBrojBodova;
} Student;

int ucitajBrojRedaka(char *imeDatoteke);
int ucitajStudente(char *imeDatoteke, Student *studenti, int brojStudenata);
int izracunajBrojRelativnihBodova(Student *studenti, int brojStudenata);
int ispisStudenata(Student *studenti, int brojStudenata);

int main() {
  char *imeDatoteke = "studenti.txt";
  int brojStudenata = 0;
  Student *studenti = NULL;
  int status = 0; // Pamti povratnu vrijednost

  // Prebroj retke u datoteci
  brojStudenata = ucitajBrojRedaka(imeDatoteke);
  if (brojStudenata == 0) {
    printf("Greska: Datoteka nije pronadena ili ne postoji!\n");
    return FILE_NOT_OPENED;
  }

  // Dinamicki alociraj memoriju za sve studente
  studenti = (Student *)malloc(brojStudenata * sizeof(Student));
  if (studenti == NULL) {
    printf("Greska: Nije moguce izvesti alokaciju memorije!\n");
    return MALLOC_ERROR;
  }

  // Ucitaj studente iz datoteke
  status = ucitajStudente(imeDatoteke, studenti, brojStudenata);
  if (status != SUCCESS) {
    free(studenti);
    return status;
  }

  // Izracun broj relativnih bodova
  status = izracunajBrojRelativnihBodova(studenti, brojStudenata);
  if (status != SUCCESS) {
    free(studenti);
    return status;
  }

  // Ispisi sve studente
  status = ispisStudenata(studenti, brojStudenata);
  if (status != SUCCESS) {
    free(studenti);
    return status;
  }

  free(studenti);
  return SUCCESS;
}

int ucitajBrojRedaka(char *imeDatoteke) {
  FILE *datoteka = fopen(imeDatoteke, "r");
  // Provjera postoji li datoteka
  if (!datoteka) {
    printf("Greska: Datoteka se nije mogla otvoriti!\n");
    return FILE_NOT_OPENED;
  }

  int brojac = 0;
  char buffer[MAX_LINE] = {0};

  // Broji retke
  while (fgets(buffer, MAX_LINE, datoteka) != NULL) {
    brojac++;
  }

  fclose(datoteka);
  return brojac;
}

int ucitajStudente(char *imeDatoteke, Student *studenti, int brojStudenata) {
  FILE *datoteka = fopen(imeDatoteke, "r");
  // Provjera postoji li datoteka
  if (!datoteka) {
    printf("Greska: Datoteka se nije mogla otvoriti!\n");
    return FILE_NOT_OPENED;
  }

  for (int i = 0; i < brojStudenata; i++) {
    if (fscanf(datoteka, " %s %s %lf", studenti[i].ime, studenti[i].prezime,
               &studenti[i].brojBodova) != 3) {
      printf("Greska: Netocan format podataka u datoteci! Treba biti ime, "
             "prezime i broj bodova!");
      fclose(datoteka);
      return FILE_READ_ERROR;
    }
    // Inicijaliziraj broj relativnih bodova po svakom studentu u 0
    studenti[i].relativanBrojBodova = 0.0;
  }

  fclose(datoteka);
  return SUCCESS;
}

int izracunajBrojRelativnihBodova(Student *studenti, int brojStudenata) {
  if (studenti == NULL || brojStudenata <= 0) {
    return FILE_READ_ERROR;
  }

  for (int i = 0; i < brojStudenata; i++) {
    studenti[i].relativanBrojBodova =
        (studenti[i].brojBodova / MAX_POINTS) * 100.0;
  }

  return SUCCESS;
}

int ispisStudenata(Student *studenti, int brojStudenata) {
  if (studenti == NULL || brojStudenata <= 0) {
    return FILE_READ_ERROR;
  }

  printf("\n%-15s %-15s %-15s %-15s\n", "Ime", "Prezime", "Broj bodova",
         "Relativni broj bodova (%)");
  printf("---------------------------------------------------------------------"
         "----------------------------\n");

  for (int i = 0; i < brojStudenata; i++) {
    printf("%-15s %-15s %-15.2lf %-15.2lf\n", studenti[i].ime,
           studenti[i].prezime, studenti[i].brojBodova,
           studenti[i].relativanBrojBodova);
  }

  printf("---------------------------------------------------------------------"
         "----------------------------\n");
  printf("Maksimalni broj bodova: %d\n", MAX_POINTS);

  return SUCCESS;
}
