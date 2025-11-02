/* Napisati program za zbrajanje i množenje polinoma (koristeći vezane liste).
Koeficijenti i eksponenti se čitaju iz datoteke. Napomena: Eksponenti u datoteci
nisu nužno sortirani. */

#define _CRT_SECURE_NO_WARNINGS
#define SUCCESS 0
#define FILE_NOT_FOUND -2
#define MALLOC_ERROR -1
#define MAX_BUFFER_SIZE 1024

#include <stdio.h>
#include <stdlib.h>

typedef struct polynomial *Position;
typedef struct polynomial {
  int coefficient;
  int exponent;
  Position next;
} Polynomial;

int insertSortedMember(Position head, int coefficient, int exponent);
int readPolynomial(Position head, FILE *file);
int printPolynomial(Position first);
int addPolynomials(Position firstPolynomial, Position secondPolynomial,
                   Position result);
int multiplyPolynomials(Position firstPolynomial, Position secondPolynomial,
                        Position result);
int freePolynomials(Position head);

int main() {
  Polynomial head1 = {.coefficient = 0, .exponent = 0, .next = NULL};
  Polynomial head2 = {.coefficient = 0, .exponent = 0, .next = NULL};
  Polynomial sum = {.coefficient = 0, .exponent = 0, .next = NULL};
  Polynomial product = {.coefficient = 0, .exponent = 0, .next = NULL};
  int status = 0;

  FILE *file = fopen("polynomials.txt", "r");
  if (!file) {
    printf("Error: the file could not be opened.\n");
    return FILE_NOT_FOUND;
  }

  // Read the two polynomials from the file
  readPolynomial(&head1, file);
  readPolynomial(&head2, file);
  fclose(file);

  printf("\n-------------------------------------------------------------\n");
  printf("First polynomial: ");
  printPolynomial(head1.next);
  printf("Second polynomial: ");
  printPolynomial(head2.next);
  printf("\n");

  // Sum of two polynomial
  status = addPolynomials(head1.next, head2.next, &sum);
  if (status != SUCCESS) {
    printf("Error in addition.\n");
    return status;
  }

  printf("Sum: ");
  printPolynomial(sum.next);

  // Multiplying two polynomials
  status = multiplyPolynomials(head1.next, head2.next, &product);
  if (status != SUCCESS) {
    printf("Error in multiplication.\n");
    return status;
  }

  printf("\nProduct: ");
  printPolynomial(product.next);
  printf("\n-------------------------------------------------------------\n");

  // Freeing memory
  freePolynomials(&head1);
  freePolynomials(&head2);
  freePolynomials(&sum);
  freePolynomials(&product);

  return SUCCESS;
}

// Insert a member of a polynomial sorted by the exponent
int insertSortedMember(Position head, int coefficient, int exponent) {
  Position newPolynomial = NULL;
  Position previous = head;
  Position current = head->next;

  while (current != NULL && current->exponent > exponent) {
    previous = current;
    current = current->next;
  }

  // If the same exponent already exists, add the coefficients and free the
  // excess member
  if (current != NULL && current->exponent == exponent) {
    current->coefficient += coefficient;
    if (current->coefficient == 0) {
      previous->next = current->next;
      free(current);
    }
    return SUCCESS;
  }

  newPolynomial = (Position)malloc(sizeof(Polynomial));
  if (!newPolynomial)
    return MALLOC_ERROR;

  newPolynomial->coefficient = coefficient;
  newPolynomial->exponent = exponent;
  newPolynomial->next = current;
  previous->next = newPolynomial;

  return SUCCESS;
}

// Read one polynomial from the file
int readPolynomial(Position head, FILE *file) {
  int coefficient, exponent;
  char buffer[MAX_BUFFER_SIZE];
  char *pointerToCharacter;
  int n = 0; // Number of read characters

  if (!fgets(buffer, sizeof(buffer), file))
    return SUCCESS;

  pointerToCharacter = buffer;
  while (sscanf(pointerToCharacter, "%d %d%n", &coefficient, &exponent, &n) ==
         2) {
    insertSortedMember(head, coefficient, exponent);
    pointerToCharacter += n; // Move pointer by amount of read chars
  }

  return SUCCESS;
}

// Print polynomial
int printPolynomial(Position first) {
  // If the pointer to the first real member of the list is not found, the list
  // is empty
  if (!first) {
    printf("0\n");
    return SUCCESS;
  }

  // Walk along the list and print the polynomials in form of c*x^e + ... where
  // c is coefficient and e is exponent
  while (first != NULL) {
    printf("%d*x^%d", first->coefficient, first->exponent);
    if (first->next != NULL)
      printf(" + ");
    first = first->next;
  }
  printf("\n");

  return SUCCESS;
}

// Addition of polynomials
int addPolynomials(Position firstPolynomial, Position secondPolynomial,
                   Position result) {
  while (firstPolynomial != NULL) {
    insertSortedMember(result, firstPolynomial->coefficient,
                       firstPolynomial->exponent);
    firstPolynomial = firstPolynomial->next;
  }

  while (secondPolynomial != NULL) {
    insertSortedMember(result, secondPolynomial->coefficient,
                       secondPolynomial->exponent);
    secondPolynomial = secondPolynomial->next;
  }

  return SUCCESS;
}

// Multiplication of polynomials
int multiplyPolynomials(Position firstPolynomial, Position secondPolynomial,
                        Position result) {
  // Go through every member of the first polynomial
  for (Position i = firstPolynomial; i != NULL; i = i->next) {
    // Go through every member of the second polynomial
    for (Position j = secondPolynomial; j != NULL; j = j->next) {
      int status = insertSortedMember(result, i->coefficient * j->coefficient,
                                      i->exponent + j->exponent);
      if (status != SUCCESS)
        return status;
    }
  }

  return SUCCESS;
}

// Free memory
int freePolynomials(Position head) {
  Position current = head->next;
  while (current != NULL) {
    Position toDelete = current;
    current = current->next;
    free(toDelete);
  }
  head->next = NULL;

  return SUCCESS;
}
