/* Napisati program koji omogućava rad s binarnim stablom pretraživanja. Treba
omogućiti unošenje novog elementa u stablo, ispis elemenata (inorder, preorder,
postorder i level order), brisanje i pronalaženje nekog elementa. */

#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0

typedef struct node *NodePosition;
typedef struct node {
  int value;
  NodePosition left;
  NodePosition right;
} Node;

NodePosition createNode(int value);
NodePosition insert(NodePosition root, int value);
NodePosition find(NodePosition root, int value);
NodePosition deleteNode(NodePosition root, int value);
NodePosition findMinimum(NodePosition root);
int inorder(NodePosition root);
int preorder(NodePosition root);
int postorder(NodePosition root);
int levelOrder(NodePosition root);
int printLevel(NodePosition root, int level);
int height(NodePosition root);
int freeTree(NodePosition root);

int main() {
  NodePosition root = NULL;
  int choice, value;

  printf("=====================================");

  while (1) {
    printf("\n *** BST Menu CLI ***\n");
    printf("1 - Insert\n");
    printf("2 - Find\n");
    printf("3 - Delete\n");
    printf("4 - Inorder print\n");
    printf("5 - Preorder print\n");
    printf("6 - Postorder print\n");
    printf("7 - Level Order print\n");
    printf("8 - Exit\n");
    printf("Choice: ");

    scanf("%d", &choice);

    switch (choice) {
    case 1:
      printf("Enter value to insert: ");
      scanf("%d", &value);
      root = insert(root, value);
      break;
    case 2:
      printf("Enter value to find: ");
      scanf("%d", &value);
      bool valueIsFound = find(root, value) != NULL;
      if (valueIsFound) {
        printf("Element of value %d was found.\n", value);
      } else {
        printf("Element of value %d was NOT found.\n", value);
      }
      break;
    case 3:
      printf("Enter value to delete: ");
      scanf("%d", &value);
      root = deleteNode(root, value);
      break;
    case 4:
      inorder(root);
      printf("\n");
      break;
    case 5:
      preorder(root);
      printf("\n");
      break;
    case 6:
      postorder(root);
      printf("\n");
      break;
    case 7:
      levelOrder(root);
      printf("\n");
      break;
    case 8:
      freeTree(root);
      printf("=====================================");
      return SUCCESS;
    default:
      printf("Invalid choice.\n");
    }
  }
}

NodePosition createNode(int value) {
  NodePosition node = malloc(sizeof(Node));
  bool mallocFailed = !node;
  if (mallocFailed)
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

  if (value < root->value) {
    root->left = insert(root->left, value);
  } else if (value > root->value) {
    root->right = insert(root->right, value);
  }

  return root;
}

NodePosition find(NodePosition root, int value) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    return NULL;

  if (value == root->value)
    return root;

  if (value < root->value) {
    return find(root->left, value);
  } else {
    return find(root->right, value);
  }
}

NodePosition deleteNode(NodePosition root, int value) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    return NULL;

  if (value < root->value) {
    root->left = deleteNode(root->left, value);
  } else if (value > root->value) {
    root->right = deleteNode(root->right, value);
  } else {
    bool leftChildDoesNotExist = !root->left;
    bool rightChildDoesNotExist = !root->right;
    if (leftChildDoesNotExist) {
      NodePosition temp = root->right;
      free(root);
      return temp;
    } else if (rightChildDoesNotExist) {
      NodePosition temp = root->left;
      free(root);
      return temp;
    }

    NodePosition temp = findMinimum(root->right);
    root->value = temp->value;
    root->right = deleteNode(root->right, temp->value);
  }
  return root;
}

NodePosition findMinimum(NodePosition root) {
  while (root && root->left)
    root = root->left;
  return root;
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

int preorder(NodePosition root) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    return SUCCESS;

  printf("%d ", root->value);
  preorder(root->left);
  preorder(root->right);
  return SUCCESS;
}

int postorder(NodePosition root) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    return SUCCESS;

  postorder(root->left);
  postorder(root->right);
  printf("%d ", root->value);
  return SUCCESS;
}

int levelOrder(NodePosition root) {
  int h = height(root);
  for (int level = 1; level <= h; level++)
    printLevel(root, level);
  return SUCCESS;
}

int printLevel(NodePosition root, int level) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    return SUCCESS;

  if (level == 1) {
    printf("%d ", root->value);
  } else {
    printLevel(root->left, level - 1);
    printLevel(root->right, level - 1);
  }
  return SUCCESS;
}

int height(NodePosition root) {
  bool nodeDoesNotExist = !root;
  if (nodeDoesNotExist)
    return 0;

  int left = height(root->left);
  int right = height(root->right);
  return (left > right ? left : right) + 1;
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
