/* Napisati program koji čita datoteku drzave.txt u kojoj su zapisani nazivi
pojedinih država. Uz ime države u datoteci se nalazi i ime dodatne datoteke u
kojoj se nalaze gradovi pojedine države. Svaka datoteka koja predstavlja državu
sadrži popis gradova u formatu naziv_grada, broj_stanovnika. Potrebno je
formirati stablo država sortirano po nazivu. Svaki čvor stabla sadrži vezanu
listu gradova sortiranu po broju stanovnika, zatim po nazivu grada.
Nakon formiranja podataka potrebno je ispisati države i gradove te omogućiti
korisniku putem tastature pretragu gradova određene države koji imaju broj
stanovnika veći od unosa na tastaturi.*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 64
#define SUCCESS 0
#define FILE_ERROR -1

typedef struct city *CityPosition;
typedef struct city {
  char name[MAX_NAME];
  int population;
  CityPosition next;
} City;

typedef struct country *CountryPosition;
typedef struct country {
  char name[MAX_NAME];
  CityPosition cities;
  CountryPosition left;
  CountryPosition right;
} Country;

CountryPosition insertCountryInTree(CountryPosition root,
                                    CountryPosition newCountry);
CountryPosition createCountry(char *name);
int loadData(CountryPosition *root);
int printCountries(CountryPosition root);
int freeTree(CountryPosition root);

CityPosition insertCitySorted(CityPosition head, CityPosition newCity);
CityPosition createCity(char *name, int population);
int loadCities(CountryPosition country, char *fileName);
int searchCities(CityPosition head, int minimumPopulation);

int main() {
  CountryPosition root = NULL;
  char countryName[MAX_NAME];
  int minimumPopulation;

  printf("=================================\n");
  loadData(&root);
  printCountries(root);
  printf("=================================\n");

  printf("Enter a country: ");
  scanf("%s", countryName);
  printf("Enter the minimum population: ");
  scanf("%d", &minimumPopulation);

  CountryPosition temp = root;
  while (temp && strcmp(temp->name, countryName) != 0) {
    bool countryGoesBeforeCurrent = strcmp(countryName, temp->name) < 0;
    if (countryGoesBeforeCurrent)
      temp = temp->left;
    else
      temp = temp->right;
  }

  bool countryWasFound = temp;
  if (countryWasFound) {
    printf("\nCountry found. Looking for cities with specified minimum "
           "population of %d:\n",
           minimumPopulation);
    searchCities(temp->cities, minimumPopulation);
  } else
    printf("The country was not found.\n");

  printf("=================================\n");

  freeTree(root);
  return SUCCESS;
}

// Inserts a country in a binary tree according to its name
CountryPosition insertCountryInTree(CountryPosition root,
                                    CountryPosition newCountry) {
  bool treeIsEmpty = !root;
  if (treeIsEmpty)
    return newCountry;

  bool newCountryNameBeforeCurrent = strcmp(newCountry->name, root->name) < 0;
  if (newCountryNameBeforeCurrent)
    root->left = insertCountryInTree(root->left, newCountry);
  else
    root->right = insertCountryInTree(root->right, newCountry);

  return root;
}

// Creates a country with memory allocation
CountryPosition createCountry(char *name) {
  CountryPosition country = malloc(sizeof(Country));
  bool countryMallocFailed = !country;
  if (countryMallocFailed) {
    printf("Error: country memory allocation failed.\n");
    return NULL;
  }

  strcpy(country->name, name);
  country->cities = NULL;
  country->left = NULL;
  country->right = NULL;
  return country;
}

// Loads the countries.txt file and creates the countries from it, loading the
// cities along the way
int loadData(CountryPosition *root) {
  FILE *file = fopen("countries.txt", "r");
  bool fileNotFound = !file;
  if (fileNotFound) {
    printf("Error: the countries file is not found.\n");
    return FILE_ERROR;
  }
  char name[MAX_NAME];
  char fileName[MAX_NAME];

  while (fscanf(file, "%s %s", name, fileName) == 2) {
    CountryPosition country = createCountry(name);
    loadCities(country, fileName);
    *root = insertCountryInTree(*root, country);
  }

  fclose(file);
  return SUCCESS;
}

// Prints the countries along with its cities
int printCountries(CountryPosition root) {
  bool treeIsEmpty = !root;
  if (treeIsEmpty)
    return SUCCESS;

  printCountries(root->left);
  printf("\n%s:\n", root->name);
  CityPosition city = root->cities;
  while (city) {
    printf("  %s (%d)\n", city->name, city->population);
    city = city->next;
  }
  printCountries(root->right);
  return SUCCESS;
}

// Frees the country memory including cities
int freeTree(CountryPosition root) {
  bool treeIsEmpty = !root;
  if (treeIsEmpty)
    return SUCCESS;

  freeTree(root->left);
  freeTree(root->right);

  CityPosition city = root->cities;
  while (city) {
    CityPosition temp = city;
    city = city->next;
    free(temp);
  }
  free(root);
  return SUCCESS;
}

// Inserts a city into the list first according to population, and if it's
// equal, by alphabetical order
CityPosition insertCitySorted(CityPosition head, CityPosition newCity) {
  bool listIsEmpty = !head;
  if (listIsEmpty) {
    newCity->next = NULL;
    return newCity;
  }

  bool newCitySmallerThanCurrent = newCity->population < head->population;
  bool newCityEqualToCurrent = newCity->population == head->population;
  bool newCityBeforeCurrentAlpha = strcmp(newCity->name, head->name) < 0;
  if (newCitySmallerThanCurrent ||
      (newCityEqualToCurrent && newCityBeforeCurrentAlpha)) {
    newCity->next = head;
    return newCity;
  }

  head->next = insertCitySorted(head->next, newCity);
  return head;
}

// Creates a city by allocating memory to it
CityPosition createCity(char *name, int population) {
  CityPosition city = malloc(sizeof(City));
  bool cityMallocFailed = !city;
  if (cityMallocFailed) {
    printf("Error: City memory allocation has failed.\n");
    return NULL;
  }

  strcpy(city->name, name);
  city->population = population;
  city->next = NULL;
  return city;
}

// Loads cities from a country file
int loadCities(CountryPosition country, char *fileName) {
  FILE *file = fopen(fileName, "r");
  char name[MAX_NAME];
  int population;

  while (fscanf(file, "%[^,],%d\n", name, &population) == 2) {
    country->cities =
        insertCitySorted(country->cities, createCity(name, population));
  }

  fclose(file);
  return SUCCESS;
}

// Searches for cities bigger than the user-specified minimum population
int searchCities(CityPosition head, int minimumPopulation) {
  while (head) {
    bool currentCityGreaterThanMin = head->population > minimumPopulation;
    if (currentCityGreaterThanMin)
      printf("%s (%d)\n", head->name, head->population);
    head = head->next;
  }
  return SUCCESS;
}
