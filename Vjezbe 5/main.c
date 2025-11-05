/* Napisati program koji iz datoteke čita postfiks izraz i zatim korištenjem
 * stoga računa rezultat. Stog je potrebno realizirati preko vezane liste. */

#define _CRT_SECURE_NO_WARNINGS
#define SUCCESS 0
#define FILE_NOT_FOUND -1
#define MALLOC_ERROR -2
#define EMPTY_STACK -3
#define DIVISION_BY_ZERO -4

#define MAX_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stackElement *Position;
typedef struct stackElement {
  double element;
  Position next;
} StackElement;

int push(Position head, double element);
int pop(Position head, double *result);
int performOperation(Position head, char operator);
int evaluatePostfixFromFile(Position head, char *fileName);
int freeStack(Position head);

int main() {
  StackElement head = {.element = 0, .next = NULL};
  double result = 0;
  int status;

  // Read postfix expression from the file
  status = evaluatePostfixFromFile(&head, "postfix.txt");
  if (status != SUCCESS) {
    printf("Error while reading the file.\n");
    return status;
  }

  // NOTE: The commented code is only correct for a file with one postfix
  // expression

  // The result should be the only element on the stack
  /*if (head.next && !head.next->next) {
    result = head.next->element;
    printf("Result of the expression in the file: %.2lf\n", result);
    printf("======================================================\n");
  } else {
    printf("Error: the expression is not valid (more or less elements in the "
           "stack than just one)\n");
  } */

  freeStack(&head);

  return SUCCESS;
}

// Add element to the top of the stack
int push(Position head, double element) {
  Position newElement = (Position)malloc(sizeof(StackElement));
  if (!newElement)
    return MALLOC_ERROR;

  newElement->element = element;
  newElement->next = head->next;
  head->next = newElement;

  return SUCCESS;
}

// Take the element off the top of the stack
int pop(Position head, double *result) {
  // If the pointer to the next element is NULL, it's an empty stack
  if (head->next == NULL)
    return EMPTY_STACK;

  // Unlink the top element, and then free the first element
  Position temp = head->next;
  *result = temp->element;
  head->next = temp->next;
  free(temp);

  return SUCCESS;
}

// Perform operation between two elements of a stack
int performOperation(Position head, char operator) {
  double operand1 = 0, operand2 = 0;
  int status;

  // Take the first element off - right operand
  status = pop(head, &operand1);
  if (status != SUCCESS)
    return status;

  // Take the second element off - left operand
  status = pop(head, &operand2);
  if (status != SUCCESS)
    return status;

  double result = 0;

  // Perform the operation depending on the operator itself
  // Order of operands is important. Operand1 goes on the right side and then
  // operand2 on the left side
  // Operation: operand2 operator operand1
  switch (operator) {
  case '+':
    result = operand2 + operand1;
    break;
  case '-':
    result = operand2 - operand1;
    break;
  case '*':
    result = operand2 * operand1;
    break;
  case '/':
    if (operand1 == 0) {
      printf("Error: Attempted to divide by 0.\n");
      return DIVISION_BY_ZERO;
    }
    result = operand2 / operand1;
    break;
  default:
    printf("Invalid operator: %c\n", operator);
    return EMPTY_STACK;
  }

  return push(head, result);
}

// Read the file and calculate the result
int evaluatePostfixFromFile(Position head, char *fileName) {
  FILE *file = fopen(fileName, "r");
  if (!file) {
    printf("Error: could not open the file.\n");
    return FILE_NOT_FOUND;
  }

  char buffer[1024];

  char token[64];
  int offset = 0;
  double number;
  int lineExists = 0; // flag to detect if the file has any lines at all

  printf("======================================================\n");

  // Read one line aka one postfix expression
  /* while (sscanf(buffer + offset, " %63s", token) == 1) {
    offset += strlen(token) + 1;

    if (sscanf(token, "%lf", &number) == 1) {
      push(head, number);
    } else if (strlen(token) == 1 && strchr("+-/*", token[0])) {
      performOperation(head, token[0]);
    } else {
      printf("Unknown symbol: %s\n", token);
      return EMPTY_STACK;
    }
  } */

  // Challenge: Read each line (one postfix expression per line)
  while (fgets(buffer, sizeof(buffer), file)) {
    lineExists = 1; // we have at least one valid line

    // First, clear the stack before each new expression if there is any
    freeStack(head);

    printf("Loaded postfix expression: %s\n", buffer);
    offset = 0;

    // Process each token in the expression
    while (sscanf(buffer + offset, " %63s", token) == 1) {
      offset += strlen(token) + 1;

      if (sscanf(token, "%lf", &number) == 1) {
        push(head, number);
      } else if (strlen(token) == 1 && strchr("+-/*", token[0])) {
        performOperation(head, token[0]);
      } else {
        printf("Unknown symbol: %s\n", token);
        fclose(file);
        return EMPTY_STACK;
      }
    }

    if (head->next && !head->next->next) {
      printf("Result: %.2lf\n", head->next->element);
    } else {
      printf("Error: Invalid postfix expression.\n");
    }

    printf("-------------------------------------------------\n");
  }

  if (!lineExists) {
    printf("Error: the file is empty.\n");
    fclose(file);
    return EMPTY_STACK;
  }

  printf("======================================================\n");

  fclose(file);
  return SUCCESS;
}

// Free the stack
int freeStack(Position head) {
  Position current = head->next;
  while (current) {
    Position toDelete = current;
    current = current->next;
    free(toDelete);
  }
  head->next = NULL;

  return SUCCESS;
}
