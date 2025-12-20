/* Zadan je niz brojeva 2, 5, 7, 8, 11, 1, 4, 2, 3, 7 koji su spremljeni u
 * čvorove binarnog stabla. Napisati funkciju insert koja dodaje element u
 * stablo tako da se pozivima te funkcije za sve element zadanog niza brojeva
 * stvori stablo. Funkcije vraća pokazivač na korijen stabla. Napisati funkciju
 * replace koja će svaki element stabla zamijeniti sumom elemenata u njegovom
 * lijevom i desnom podstablu (tj. sumom svih potomaka prije zamjene vrijednosti
 * u tim potomcima). */

#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0

typedef struct node *NodePosition;
typedef struct node {
  int value;
  NodePosition left;
  NodePosition right;
} Node;

NodePosition createNode(int value);
NodePosition insert(NodePosition root, int value);
int replace(NodePosition root);
int inorder(NodePosition root);
int freeTree(NodePosition root);

int main() {
  int array[] = {2, 5, 7, 8, 11, 1, 4, 2, 3, 7};
  int numberOfElements = sizeof(array) / sizeof(array[0]);

  NodePosition root = NULL;

  for (int index = 0; index < numberOfElements; index++) {
    root = insert(root, array[index]);
  }

  printf("INORDER BEFORE REPLACE:\n");
  inorder(root);
  printf("\n");

  replace(root);

  printf("INORDER AFTER REPLACE:\n");
  inorder(root);
  printf("\n");

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
    // If there's nothing to be found in either one of the nodes, return a 0
    return 0;

  int leftSum = replace(root->left);
  int rightSum = replace(root->right);

  int oldValue = root->value;
  root->value = leftSum + rightSum;

  return root->value + oldValue;
}

int inorder(NodePosition root) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    return SUCCESS;

  inorder(root->left);
  printf("%d ", root->value);
  inorder(root->right);

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
