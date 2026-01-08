/*Napisati program koji čita datoteku drzave.txt u kojoj su zapisani nazivi
pojedinih država. Uz ime države u datoteci se nalazi i ime dodatne datoteke u
kojoj se nalaze gradovi pojedine države. Svaka datoteka koja predstavlja državu
sadrži popis gradova u formatu naziv_grada, broj_stanovnika. Potrebno je
formirati sortiranu vezanu listu država po nazivu. Svaki čvor vezane liste
sadrži stablo gradova sortirano po broju stanovnika, zatim po nazivu grada.
Nakon formiranja podataka potrebno je ispisati države i gradove te omogućiti
korisniku putem tastature pretragu gradova određene države koji imaju broj
stanovnika veći od unosa na tastaturi.*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 128
#define SUCCESS 0
#define FILE_ERROR -1

typedef struct city *CityPosition;
typedef struct city {
  char name[MAX_NAME];
  int population;
  CityPosition left;
  CityPosition right;
} City;

typedef struct country *CountryPosition;
typedef struct country {
  char name[MAX_NAME];
  CityPosition cities;
  CountryPosition next;
} Country;

CountryPosition createCountry(char *name);
CountryPosition insertCountrySorted(CountryPosition head,
                                    CountryPosition newCountry);
int loadCountries(CountryPosition *head);
int printCountries(CountryPosition head);
int freeCountries(CountryPosition head);

CityPosition createCity(char *name, int population);
CityPosition insertCityInTree(CityPosition root, CityPosition newCity);
int loadCities(CountryPosition country, char *fileName);
int printCitiesInorder(CityPosition root);
int searchCities(CityPosition root, int minimumPopulation);
int freeCities(CityPosition root);

int main() {
  CountryPosition head = NULL;
  char countryName[MAX_NAME];
  int minimumPopulation;

  printf("=======================================\n");
  loadCountries(&head);
  printCountries(head);
  printf("=======================================\n\n");

  printf("Enter a country: ");
  scanf("%s", countryName);
  printf("Enter the minimum population: ");
  scanf("%d", &minimumPopulation);

  CountryPosition temp = head;
  while (temp && strcmp(temp->name, countryName) != 0)
    temp = temp->next;

  bool countryWasFound = temp;
  if (countryWasFound) {
    printf("\nCountry found. Looking for cities with specified minimum "
           "population of %d:\n",
           minimumPopulation);
    searchCities(temp->cities, minimumPopulation);
  } else
    printf("\nThe country was not found.\n");

  printf("=======================================\n");

  freeCountries(head);
  return SUCCESS;
}

// Creates a country with memory allocation and assigns values
CountryPosition createCountry(char *name) {
  CountryPosition country = malloc(sizeof(Country));
  bool countryMallocFailed = !country;
  if (countryMallocFailed) {
    printf("Error: Country memory allocation has failed.\n");
    return NULL;
  }
  strcpy(country->name, name);
  country->cities = NULL;
  country->next = NULL;
  return country;
}

// Inserts a country into the list according to its name in alphabetical order
CountryPosition insertCountrySorted(CountryPosition head,
                                    CountryPosition newCountry) {
  bool listIsEmpty = !head;
  if (listIsEmpty) {
    newCountry->next = NULL;
    return newCountry;
  }

  if (strcmp(newCountry->name, head->name) < 0) {
    newCountry->next = head;
    return newCountry;
  }

  head->next = insertCountrySorted(head->next, newCountry);
  return head;
}

// Loads the countries from the countries.txt file along with its cities
int loadCountries(CountryPosition *head) {
  FILE *file = fopen("countries.txt", "r");
  bool fileNotOpened = !file;
  if (fileNotOpened) {
    printf("The file could not be opened.\n");
    return FILE_ERROR;
  }

  char country[MAX_NAME], fileName[MAX_NAME];

  while (fscanf(file, "%s %s", country, fileName) == 2) {
    CountryPosition countryNode = createCountry(country);
    loadCities(countryNode, fileName);
    *head = insertCountrySorted(*head, countryNode);
  }

  fclose(file);
  return SUCCESS;
}

// Prints all the countries with their cities
int printCountries(CountryPosition head) {
  while (head) {
    printf("\n%s:\n", head->name);
    printCitiesInorder(head->cities);
    head = head->next;
  }
  return SUCCESS;
}

// Frees countries and also its cities
int freeCountries(CountryPosition head) {
  bool listIsEmpty = !head;
  if (listIsEmpty)
    return SUCCESS;

  freeCountries(head->next);
  freeCities(head->cities);
  free(head);
  return SUCCESS;
}

// Creates a city via memory allocation
CityPosition createCity(char *name, int population) {
  CityPosition city = malloc(sizeof(City));
  bool cityMallocFailed = !city;
  if (cityMallocFailed) {
    printf("City memory allocation has failed.\n");
    return NULL;
  }

  strcpy(city->name, name);
  city->population = population;
  city->left = NULL;
  city->right = NULL;
  return city;
}

// Inserts a city into a binary tree first by its population and then by its name according to the alphabetical order
CityPosition insertCityInTree(CityPosition root, CityPosition newCity) {
  bool treeIsEmpty = !root;
  if (treeIsEmpty)
    return newCity;

  bool newCitySmallerThanCurrent = newCity->population < root->population;
  bool newCityEqualToCurrent = newCity->population == root->population;
  bool newCitySmallerAlphabet = strcmp(newCity->name, root->name) < 0;
  bool newCityBeforeCurrent = newCityEqualToCurrent && newCitySmallerAlphabet;
  if (newCitySmallerThanCurrent || newCityBeforeCurrent)
    root->left = insertCityInTree(root->left, newCity);
  else
    root->right = insertCityInTree(root->right, newCity);

  return root;
}

// Loads cities from a country file
int loadCities(CountryPosition country, char *fileName) {
  FILE *file = fopen(fileName, "r");
  bool fileNotFound = !file;
  if (fileNotFound) {
    printf("Country file not found.\n");
    return FILE_ERROR;
  }
  char name[MAX_NAME];
  int population;

  while (fscanf(file, "%[^,],%d\n", name, &population) == 2) {
    CityPosition city = createCity(name, population);
    country->cities = insertCityInTree(country->cities, city);
  }

  fclose(file);
  return SUCCESS;
}

// Prints all the cities of a country in the inorder style (left branch, then main/root, then right branch)
// Source: https://www.geeksforgeeks.org/dsa/inorder-traversal-of-binary-tree/
int printCitiesInorder(CityPosition root) {
  bool treeIsEmpty = !root;
  if (treeIsEmpty)
    return SUCCESS;

  printCitiesInorder(root->left);
  printf("  %s (%d)\n", root->name, root->population);
  printCitiesInorder(root->right);
  return SUCCESS;
}

// Searches for the cities bigger than the user-specified minimum population
int searchCities(CityPosition root, int minimumPopulation) {
  bool treeIsEmpty = !root;
  if (treeIsEmpty)
    return SUCCESS;

  searchCities(root->left, minimumPopulation);
  bool currentCityLarger = root->population > minimumPopulation;
  if (currentCityLarger)
    printf("%s (%d)\n", root->name, root->population);
  searchCities(root->right, minimumPopulation);

  return SUCCESS;
}

// Frees the city tree
int freeCities(CityPosition root) {
  bool treeIsEmpty = !root;
  if (treeIsEmpty)
    return SUCCESS;

  freeCities(root->left);
  freeCities(root->right);
  free(root);
  return SUCCESS;
}
