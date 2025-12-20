/* Prepraviti program na način da umjesto predefiniranog cjelobrojnog polja
korištenjem funkcije rand() generira slučajne brojeve u rasponu <10, 90>.
Također, potrebno je upisati u datoteku sve brojeve u inorder prolasku nakon
korištenja funkcije iz a), zatim b) dijela zadatka. */

#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SUCCESS 0
#define FILE_ERROR -1

typedef struct node *NodePosition;
typedef struct node {
  int value;
  NodePosition left;
  NodePosition right;
} Node;

NodePosition createNode(int value);
NodePosition insert(NodePosition root, int value);
int replace(NodePosition root);
int printInorderToFile(NodePosition root, FILE *file);
int freeTree(NodePosition root);

int main() {
  NodePosition root = NULL;
  FILE *file = NULL;
  int numberOfElements = 10;

  srand(time(NULL));

  for (int index = 0; index < numberOfElements; index++) {
    // We do % 81 + 10 because minimum number can either be from 10 to 90
    int number = rand() % 81 + 10;
    root = insert(root, number);
  }

  file = fopen("output.txt", "w");
  bool fileFailedToOpen = !file;
  if (fileFailedToOpen)
    return FILE_ERROR;

  fprintf(file, "INORDER BEFORE REPLACE:\n");
  printInorderToFile(root, file);
  fprintf(file, "\n");

  replace(root);

  fprintf(file, "INORDER AFTER REPLACE:\n");
  printInorderToFile(root, file);

  fclose(file);
  printf("Succesfully printed to the file 'output.txt'.\n");
  freeTree(root);

  return SUCCESS;
}

NodePosition createNode(int value) {
  NodePosition node = malloc(sizeof(Node));
  bool nodeMallocFailed = !node;
  if (nodeMallocFailed)
    return NULL;

  node->value = value;
  node->left = NULL;
  node->right = NULL;
  return node;
}

NodePosition insert(NodePosition root, int value) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    return createNode(value);

  if (value >= root->value)
    root->left = insert(root->left, value);
  else
    root->right = insert(root->right, value);

  return root;
}

int replace(NodePosition root) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    // Nothing is there, so return 0
    return 0;

  int leftSum = replace(root->left);
  int rightSum = replace(root->right);

  int oldValue = root->value;
  root->value = leftSum + rightSum;

  return root->value + oldValue;
}

int printInorderToFile(NodePosition root, FILE *file) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    return SUCCESS;

  printInorderToFile(root->left, file);
  fprintf(file, "%d ", root->value);
  printInorderToFile(root->right, file);

  return SUCCESS;
}

int freeTree(NodePosition root) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    return SUCCESS;

  freeTree(root->left);
  freeTree(root->right);
  free(root);

  return SUCCESS;
}
