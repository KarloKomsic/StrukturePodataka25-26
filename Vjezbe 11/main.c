/* Prepraviti zadatak 10 na način da se formira hash tablica država. Tablica ima
11 mjesta, a funkcija za preslikavanje ključ računa da se zbraja ASCII
vrijednost prvih pet slova države zatim računa ostatak cjelobrojnog dijeljenja
te vrijednosti s veličinom tablice. Države s istim ključem se pohranjuju u
vezanu listu sortiranu po nazivu države. Svaki čvor vezane liste sadrži stablo
gradova sortirano po broju stanovnika, zatim po nazivu grada. */

#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 11
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

int hashFunction(char *countryName);

CountryPosition createCountry(char *name);
CountryPosition insertCountrySorted(CountryPosition head,
                                    CountryPosition newCountry);
int loadCountries(CountryPosition hashTable[]);
int printHashTable(CountryPosition hashTable[]);
int freeHashTable(CountryPosition hashTable[]);

CityPosition createCity(char *name, int population);
CityPosition insertCityTree(CityPosition root, CityPosition newCity);
int loadCities(CountryPosition country, char *fileName);
int printCitiesInorder(CityPosition root);
int searchCities(CityPosition root, int minimumPopulation);
int freeCities(CityPosition root);

int main() {
  CountryPosition hashTable[TABLE_SIZE] = {NULL};
  char countryName[MAX_NAME];
  int minimumPopulation;

  printf("=======================================\n");
  loadCountries(hashTable);
  printHashTable(hashTable);
  printf("=======================================\n");

  printf("Enter a country: ");
  scanf("%s", countryName);
  printf("Enter the minimum population: ");
  scanf("%d", &minimumPopulation);

  int index = hashFunction(countryName);
  CountryPosition temp = hashTable[index];

  while (temp && strcmp(temp->name, countryName) != 0)
    temp = temp->next;

  bool countryWasFound = temp;
  if (countryWasFound) {
    printf("\nThe country was found. Now looking for the cities bigger than "
           "the minimum population:\n");
    searchCities(temp->cities, minimumPopulation);
  } else
    printf("\nThe country was not found.\n");

  freeHashTable(hashTable);
  return SUCCESS;
}

// Turns the country name into an array index of hashTable so that the countries
// are all evenly distributed in the table
// NOTE: It uses ASCII numbers OF THE FIRST 5 LETTERS OF THE COUNTRY
int hashFunction(char *countryName) {
  int sum = 0;
  int length = strlen(countryName);

  for (int i = 0; i < 5 && i < length; i++)
    sum += (int)countryName[i];

  // Modulo of 11 in this case
  return sum % TABLE_SIZE;
}

// Creates a country via memory allocation
CountryPosition createCountry(char *name) {
  CountryPosition country = malloc(sizeof(Country));
  bool countryMallocFailed = !country;
  if (countryMallocFailed) {
    printf("Error: Country memory allocation failed.\n");
    return NULL;
  }

  strcpy(country->name, name);
  country->cities = NULL;
  country->next = NULL;
  return country;
}

// Inserts country sorted into a linked list
CountryPosition insertCountrySorted(CountryPosition head,
                                    CountryPosition newCountry) {
  bool listIsEmpty = !head;
  if (listIsEmpty) {
    newCountry->next = NULL;
    return newCountry;
  }

  bool newCountryBeforeCurrentAlpha = strcmp(newCountry->name, head->name) < 0;
  if (newCountryBeforeCurrentAlpha) {
    newCountry->next = head;
    return newCountry;
  }

  head->next = insertCountrySorted(head, newCountry);
  return head;
}

// Loads countries from countries.txt file
int loadCountries(CountryPosition hashTable[]) {
  FILE *file = fopen("countries.txt", "r");
  bool fileNotFound = !file;
  if (fileNotFound) {
    printf("Error: file not found.\n");
    return FILE_ERROR;
  }

  char countryName[MAX_NAME];
  char fileName[MAX_NAME];

  while (fscanf(file, "%s %s", countryName, fileName) == 2) {
    CountryPosition country = createCountry(countryName);
    loadCities(country, fileName);

    int index = hashFunction(countryName);
    hashTable[index] = insertCountrySorted(hashTable[index], country);
  }

  fclose(file);
  return SUCCESS;
}

// Prints the entire hash table
int printHashTable(CountryPosition hashTable[]) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    printf("\n[%d]:\n", i);
    CountryPosition temp = hashTable[i];

    // Goes through all countries in one index
    while (temp) {
      printf("  %s:\n", temp->name);
      printCitiesInorder(temp->cities);
      temp = temp->next;
    }
  }
  return SUCCESS;
}

// Frees the hash table of countries including the cities
int freeHashTable(CountryPosition hashTable[]) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    CountryPosition temp = hashTable[i];
    while (temp) {
      CountryPosition toDelete = temp;
      temp = temp->next;
      freeCities(toDelete->cities);
      free(toDelete);
    }
  }
  return SUCCESS;
}

// Creates a city via memory allocation
CityPosition createCity(char *name, int population) {
  CityPosition city = malloc(sizeof(City));
  bool cityMallocFailed = !city;
  if (cityMallocFailed) {
    printf("Error: City memory allocation failed.\n");
    return NULL;
  }

  strcpy(city->name, name);
  city->population = population;
  city->left = NULL;
  city->right = NULL;
  return city;
}

// Inserts a city into a binary tree by population first and then name
CityPosition insertCityTree(CityPosition root, CityPosition newCity) {
  bool treeIsEmpty = !root;
  if (treeIsEmpty)
    return newCity;

  bool newCitySmallerThanCurrent = newCity->population < root->population;
  bool newCityEqualToCurrent = newCity->population == root->population;
  bool newCityBeforeCurrentAlpha = strcmp(newCity->name, root->name) < 0;
  bool newCityGoesBeforeCurrent =
      newCityEqualToCurrent && newCityBeforeCurrentAlpha;
  if (newCitySmallerThanCurrent || newCityGoesBeforeCurrent)
    root->left = insertCityTree(root->left, newCity);
  else
    root->right = insertCityTree(root->right, newCity);

  return root;
}

// Loads cities from a country file
int loadCities(CountryPosition country, char *fileName) {
  FILE *file = fopen(fileName, "r");
  bool fileNotFound = !file;
  if (fileNotFound) {
    printf("Error: country file could not be found.\n");
    return FILE_ERROR;
  }

  char name[MAX_NAME];
  int population;

  while (fscanf(file, "%[^,],%d\n", name, &population) == 2) {
    CityPosition city = createCity(name, population);
    country->cities = insertCityTree(country->cities, city);
  }

  fclose(file);
  return SUCCESS;
}

// Prints cities inorder
// Source of the inorder traversal:
// https://www.geeksforgeeks.org/dsa/inorder-traversal-of-binary-tree/
int printCitiesInorder(CityPosition root) {
  bool treeIsEmpty = !root;
  if (treeIsEmpty)
    return SUCCESS;

  printCitiesInorder(root->left);
  printf("    %s (%d)\n", root->name, root->population);
  printCitiesInorder(root->right);
  return SUCCESS;
}

// Search for cities bigger than the user-specified minimum
int searchCities(CityPosition root, int minimumPopulation) {
  bool treeIsEmpty = !root;
  if (treeIsEmpty)
    return SUCCESS;

  searchCities(root->left, minimumPopulation);
  bool currentCityLargerThanMinimum = root->population > minimumPopulation;
  if (currentCityLargerThanMinimum)
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
