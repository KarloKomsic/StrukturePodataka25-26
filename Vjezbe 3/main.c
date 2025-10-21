/*
Definirati strukturu osoba (ime, prezime, godina rođenja) i napisati program
koji:
A. dinamički dodaje novi element na početak liste,
B. ispisuje listu,
C. dinamički dodaje novi element na kraj liste,
D. pronalazi element u listi (po prezimenu),
E. briše određeni element iz liste,
F. dinamički dodaje novi element iza određenog elementa
G. dinamički dodaje novi element ispred određenog elementa
H. sortira listu po prezimenu osoba
I. upisuje listu u datoteke
J. čita listu iz datoteke
U zadatku se ne smiju koristiti globalne varijable.
*/

#define _CRT_SECURE_NO_WARNINGS
#define PERSON_NOT_FOUND 1
#define SUCCESS 0
#define MALLOC_ERROR -2
#define FILE_NOT_OPENED -3
#define FILE_READ_ERROR 3
#define LIST_EMPTY 2
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

// A - E tasks
int insertAtBeginningOfList(Position head, char *firstName, char *lastName,
                            int yearOfBirth);
int printList(Position first);
int insertAtEndOfList(Position head, char *firstName, char *lastName,
                      int yearOfBirth);
Position searchByLastName(Position first, char *lastName);
int deletePersonFromList(Position head, char *lastName);

// F - J tasks
int insertAfterElement(Position head, char *targetLastName, char *firstName,
                       char *lastName, int yearOfBirth);
int insertBeforeElement(Position head, char *targetLastName, char *firstName,
                        char *lastName, int yearOfBirth);
int sortListByLastName(Position head);
int writeListInFile(Position first, char *fileName);
int readListFromFile(Position head, char *fileName);

// Bonus function freeList() for sake of testing the readListFromFile() and
// freeing the memory after the program ran
int freeList(Position head);

int main() {
  Person head = {
      .firstName = "", .lastName = "", .yearOfBirth = 0, .nextElement = NULL};
  int status = 0;
  Position foundPerson = NULL;

  /* In this case, first in list will be Marijana, then Roko and then afterward
     Jozo */
  status = insertAtBeginningOfList(&head, "Roko", "Dujevic", 1997);
  status = insertAtEndOfList(&head, "Jozo", "Miskovic", 1984);
  status = insertAtBeginningOfList(&head, "Marijana", "Bosnjak", 2004);

  // Printing current list with added people
  printf("---------------------------------------------------------------------"
         "----------------------------\n");
  printf("~~~~ List of people: ~~~~\n");
  printList(head.nextElement);

  // Searching by last name
  foundPerson = searchByLastName(head.nextElement, "Bosnjak");
  if (foundPerson != NULL)
    printf("\nFound a person: %s %s, %d\n", foundPerson->firstName,
           foundPerson->lastName, foundPerson->yearOfBirth);
  else
    printf("\nPerson not found.\n");

  // Insert after element
  status = insertAfterElement(&head, "Bosnjak", "Anita", "Krizina", 2000);
  printf("\n~~~~ List of people after insertAfterElement(): ~~~~\n");
  printList(head.nextElement);

  // Insert before element
  status = insertBeforeElement(&head, "Miskovic", "Goran", "Sliskovic", 1994);
  printf("\n~~~~ List of people after insertBeforeElement(): ~~~~\n");
  printList(head.nextElement);

  // Sorting the list
  status = sortListByLastName(&head);
  printf("\n~~~~ List of people after sorting the list by last name: ~~~~\n");
  printList(head.nextElement);

  // Writing into a file
  status = writeListInFile(head.nextElement, "people.txt");
  if (status != SUCCESS) {
    printf("Warning: writeListInFile failed.\n");
  }

  // Free list before before reading from a file
  status = freeList(&head);
  if (status != SUCCESS) {
    printf("Warning: freeList() returned %d\n", status);
  }

  // Read a list from a file
  status = readListFromFile(&head, "people.txt");
  if (status != SUCCESS) {
    printf("Warning: readListFromFile failed and returned error %d.\n", status);
  }
  printf("\n~~~~ List after reading it from a file: ~~~~\n");
  printList(head.nextElement);

  printf("---------------------------------------------------------------------"
         "----------------------------\n");

  // Deleting a person
  printf("Deleting person 'Miskovic'...\n");
  status = deletePersonFromList(&head, "Miskovic");
  if (status == SUCCESS)
    printf("Person succesfully deleted.\n\n");
  else
    printf("Person not found.\n\n");

  printf("~~~~ List after deleting: ~~~~\n");
  printList(head.nextElement);

  printf("---------------------------------------------------------------------"
         "----------------------------\n");

  // After everything, free the entire list due to good practice
  status = freeList(&head);
  if (status != SUCCESS) {
    printf("Warning: freeList returned %d at program end.\n", status);
  }

  return SUCCESS;
}

// Add a new person at the beginning of the list
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

  // Linking to the beginning of the list
  newPerson->nextElement = head->nextElement;
  head->nextElement = newPerson;

  return SUCCESS;
}

// Print the list
int printList(Position first) {
  // If the list is empty, return an error
  if (first == NULL) {
    printf("Lista je prazna");
    return PERSON_NOT_FOUND;
  }

  // Walk along the list, and then print the information of every member
  while (first != NULL) {
    printf("%s %s, %d\n", first->firstName, first->lastName,
           first->yearOfBirth);
    first = first->nextElement;
  }

  return SUCCESS;
}

// Add a person at the end of the list
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

  // Pointer in the address of the next added member is NULL
  newPerson->nextElement = NULL;

  // Now search for the last element in the list
  Position temp = head;
  while (temp->nextElement != NULL)
    temp = temp->nextElement;

  // When we found the last element, add the new element at the end
  temp->nextElement = newPerson;

  return SUCCESS;
}

// Searching a person by their last name
Position searchByLastName(Position first, char *lastName) {
  while (first != NULL) {
    /* If the last name of the current person is identical to the last name of
       the target person, return that person */
    if (strcmp(first->lastName, lastName) == 0)
      return first;
    first = first->nextElement;
  }

  // If the target last name isn't found, return NULL
  printf("Osoba pod tim prezimenom ne postoji.\n");
  return NULL;
}

// Delete a person using last name
int deletePersonFromList(Position head, char *lastName) {
  Position previous = head;
  Position current = head->nextElement;

  while (current != NULL) {
    /* If the last names are the same, we found the target person we want to
       delete */
    if (strcmp(current->lastName, lastName) == 0) {
      /* Relink pointers in such a way that we assign a pointer of that element
         which the currently undeleted member points to. For example: Ivan ->
         Ana -> Luka, will be Ivan -> Luka if we delete Ana */
      previous->nextElement = current->nextElement;
      free(current);

      return SUCCESS;
    }
    previous = current;
    current = current->nextElement;
  }

  // Return error if the person we are trying to delete is not found
  printf("Osoba koju probavate obrisati nije pronadena.\n");
  return PERSON_NOT_FOUND;
}

// Insert after an element
int insertAfterElement(Position head, char *targetLastName, char *firstName,
                       char *lastName, int yearOfBirth) {
  // First we search for the last name of the person we want to insert a new
  // member after
  Position target = searchByLastName(head->nextElement, targetLastName);
  if (target == NULL) {
    return PERSON_NOT_FOUND;
  }

  // If found, create a new member
  Position newPerson = (Position)malloc(sizeof(Person));
  if (!newPerson) {
    return MALLOC_ERROR;
  }

  // Assign values to the new member
  strncpy(newPerson->firstName, firstName, MAX_NAME_SIZE - 1);
  newPerson->firstName[MAX_NAME_SIZE - 1] = '\0';
  strncpy(newPerson->lastName, lastName, MAX_NAME_SIZE - 1);
  newPerson->lastName[MAX_NAME_SIZE - 1] = '\0';
  newPerson->yearOfBirth = yearOfBirth;

  // New member points at what the target member previously pointed at and the
  // target now points at the new member
  newPerson->nextElement = target->nextElement;
  target->nextElement = newPerson;

  return SUCCESS;
}

// Insert before an element
int insertBeforeElement(Position head, char *targetLastName, char *firstName,
                        char *lastName, int yearOfBirth) {
  Position previous = head;
  Position current = head->nextElement;

  while (current != NULL) {
    if (strcmp(current->lastName, targetLastName) == 0) {
      // Dynamic allocation of a new person
      Position newPerson = (Position)malloc(sizeof(Person));
      if (!newPerson) {
        return MALLOC_ERROR;
      }

      // Assigning values to the new member
      strncpy(newPerson->firstName, firstName, MAX_NAME_SIZE - 1);
      newPerson->firstName[MAX_NAME_SIZE - 1] = '\0';
      strncpy(newPerson->lastName, lastName, MAX_NAME_SIZE - 1);
      newPerson->lastName[MAX_NAME_SIZE - 1] = '\0';
      newPerson->yearOfBirth = yearOfBirth;

      // Link the new person between the "current" and "previous" members
      newPerson->nextElement = current;
      previous->nextElement = newPerson;

      return SUCCESS;
    }

    // Shift previous and current pointers by 1 member each
    previous = current;
    current = current->nextElement;
  }

  return PERSON_NOT_FOUND;
}

// Sort a list by last names, using Bubble Sort (A-Z)
int sortListByLastName(Position head) {
  // If there are 0 or 1 elements in the list, no need to sort
  if (head->nextElement == NULL || head->nextElement->nextElement == NULL)
    return SUCCESS;

  // swapped indicates if a swap happened
  int swapped;

  // We need to initiate the checking for sorting at least once, hence the
  // do-while
  do {
    swapped = 0;
    Position current = head->nextElement;

    // Go through the list
    while (current->nextElement != NULL) {
      Position next = current->nextElement;

      // Sorting part is done here using Bubble Sort
      // If current last name is alphabetically higher than the next last name
      // (e.g. Backford instead of Adams), swap them
      if (strcmp(current->lastName, next->lastName) > 0) {
        // Swap the contents of two neighbouring members
        char tempFirst[MAX_NAME_SIZE];
        char tempLast[MAX_NAME_SIZE];
        int tempYear;

        strcpy(tempFirst, current->firstName);
        strcpy(tempLast, current->lastName);
        tempYear = current->yearOfBirth;

        strcpy(current->firstName, next->firstName);
        strcpy(current->lastName, next->lastName);
        current->yearOfBirth = next->yearOfBirth;

        strcpy(next->firstName, tempFirst);
        strcpy(next->lastName, tempLast);
        next->yearOfBirth = tempYear;

        // Mark the indicator of swapped so the do-while loop can continue
        swapped = 1;
      }

      current = current->nextElement;
    }
  } while (swapped);

  return SUCCESS;
}

// Open a file and write the entire list into a file one by one
int writeListInFile(Position first, char *fileName) {
  FILE *file = fopen(fileName, "w");
  if (!file) {
    return FILE_NOT_OPENED;
  }

  // Write each member into seperate lines of the text file
  while (first != NULL) {
    fprintf(file, "%s %s %d\n", first->firstName, first->lastName,
            first->yearOfBirth);
    first = first->nextElement;
  }

  // Close the file
  fclose(file);

  return SUCCESS;
}

// Read the list from a text file
int readListFromFile(Position head, char *fileName) {
  FILE *file = fopen(fileName, "r");
  if (!file) {
    return FILE_NOT_OPENED;
  }

  // Create buffers to store each value of a member
  char firstName[MAX_NAME_SIZE];
  char lastName[MAX_NAME_SIZE];
  int year = 0;
  int readCount = 0; // Count how many lines we've read

  // Read members from file to list and insert them at the end of the list each
  // Scans for 3 values per line, if that is true only then can we read the line
  while ((readCount =
              fscanf(file, " %49s %49s %d", firstName, lastName, &year)) == 3) {
    int status = insertAtEndOfList(head, firstName, lastName, year);
    if (status != SUCCESS) {
      fclose(file);
      return status;
    }
  }

  // If we are not at the end of the file, and fscanf did not return value of 3,
  // it is a format errror
  if (!feof(file)) {
    fclose(file);
    return FILE_READ_ERROR;
  }

  fclose(file);
  return SUCCESS;
}

// Free all dynamically allocated members except head
int freeList(Position head) {
  if (head == NULL)
    return SUCCESS;

  // Assign current to the first real element, then walk through the list one by
  // one and free each member via the toDelete variable
  Position current = head->nextElement;
  while (current != NULL) {
    Position toDelete = current;
    current = current->nextElement;
    free(toDelete);
  }
  head->nextElement = NULL;

  return SUCCESS;
}
