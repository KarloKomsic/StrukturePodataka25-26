/* Napisati program koji pomoću vezanih listi (stabala) predstavlja strukturu
direktorija. Omogućiti unos novih direktorija i pod-direktorija, ispis sadržaja
direktorija i povratak u prethodni direktorij. Točnije program treba preko
menija simulirati korištenje DOS naredbi: 1- "md", 2 - "cd dir", 3 - "cd..", 4 -
"dir" i 5 – izlaz.

NOTE: Added additional function for rd (remove empty directory)
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 64
#define MAX_COMMAND 128

#define SUCCESS 0
#define DIRECTORY_ERROR -1

typedef struct directory *directoryPosition;
typedef struct directory {
  char name[MAX_NAME];
  directoryPosition sibling;      // next direcotry ON THE SAME LEVEL
  directoryPosition subDirectory; // first subdirectory
} Directory;

typedef struct stack *stackPosition;
typedef struct stack {
  directoryPosition directory;
  stackPosition next;
} Stack;

directoryPosition createDirectory(char *directoryName);
int md(directoryPosition current, char *newDirectoryName);
int cdDir(directoryPosition *current, stackPosition stackHead,
          char *subDirectoryName);
int cdBack(directoryPosition *current, stackPosition stackHead);
int dir(directoryPosition current);
int rd(directoryPosition current, char *directoryName);
int push(stackPosition stackHead, directoryPosition directory);
directoryPosition pop(stackPosition head);
int printPathRecursive(stackPosition node);
int printPath(stackPosition head, directoryPosition current);
int freeDirectoryTree(directoryPosition directory);
int freeStack(stackPosition head);

int main() {
  Directory head = {"C:", NULL, NULL};
  Stack stackHead = {NULL, NULL};
  directoryPosition current = &head;

  // Pushing the root directory into the stack first
  push(&stackHead, current);

  char command[MAX_COMMAND];

  printf("=====================================");

  while (1) {
    printPath(&stackHead, current);
    printf("> ");
    fgets(command, sizeof(command), stdin);

    if (strncmp(command, "md ", 3) == 0) {
      md(current, command + 3);
    } else if (strncmp(command, "rd ", 3) == 0) {
      rd(current, command + 3);
    } else if (strncmp(command, "cd ", 3) == 0) {
      cdDir(&current, &stackHead, command + 3);
    } else if (strcmp(command, "cd..\n") == 0) {
      cdBack(&current, &stackHead);
    } else if (strcmp(command, "dir\n") == 0) {
      dir(current);

    } else if (strcmp(command, "exit\n") == 0) {
      freeDirectoryTree(head.subDirectory); // since root is static, free from
                                            // the subdirectories
      freeStack(&stackHead);
      break;
    } else {
      printf("Unknown command.\n");
    }
  }

  printf("=====================================");
  return SUCCESS;
}

directoryPosition createDirectory(char *directoryName) {
  directoryPosition directory = malloc(sizeof(Directory));
  bool directoryMallocFailed = !directory;
  if (directoryMallocFailed) {
    printf("Error: Memory allocation for directory failed.\n");
    return NULL;
  }

  strcpy(directory->name, directoryName);
  directory->sibling = NULL;
  directory->subDirectory = NULL;
  return directory;
}

int md(directoryPosition current, char *newDirectoryName) {
  newDirectoryName[strcspn(newDirectoryName, "\n")] = 0;

  directoryPosition newDirectory = createDirectory(newDirectoryName);
  bool directoryCreationFailed = !newDirectory;
  if (directoryCreationFailed) {
    printf("Could not create new directory: %s", newDirectoryName);
    return DIRECTORY_ERROR;
  }

  bool currentDirectoryHasNoSubdirectory = !current->subDirectory;
  if (currentDirectoryHasNoSubdirectory) {
    current->subDirectory = newDirectory;
  } else { // a subdirectory already exists
    // Go through the entire list of subdirectories in the current
    // directory, and then add the new directory at the end
    directoryPosition temp = current->subDirectory;
    while (temp->sibling)
      temp = temp->sibling;
    temp->sibling = newDirectory;
  }

  return SUCCESS;
}

int cdDir(directoryPosition *current, stackPosition stackHead,
          char *subDirectoryName) {
  subDirectoryName[strcspn(subDirectoryName, "\n")] = 0;
  directoryPosition temp = (*current)->subDirectory;
  while (temp) {
    if (strcmp(temp->name, subDirectoryName) == 0) {
      *current = temp;
      push(stackHead, temp);
      return SUCCESS;
    }
    temp = temp->sibling;
  }
  printf("Directory not found.\n");
  return DIRECTORY_ERROR;
}

int cdBack(directoryPosition *current, stackPosition stackHead) {
  bool stackIsEmpty = !stackHead->next;
  bool stackOnlyHasRoot = !stackHead->next->next;
  if (stackIsEmpty || stackOnlyHasRoot) {
    // stack: [root]
    return SUCCESS;
  }

  pop(stackHead);
  *current = stackHead->next->directory;
  return SUCCESS;
}

int dir(directoryPosition current) {
  directoryPosition temp = current->subDirectory;
  bool noSubdirectoryFound = !temp;
  if (noSubdirectoryFound) {
    printf("<empty>\n");
    return SUCCESS;
  }

  // Go through each subdirectory
  while (temp) {
    printf("%s\n", temp->name);
    temp = temp->sibling;
  }
  return SUCCESS;
}

int rd(directoryPosition current, char *directoryName) {
  directoryName[strcspn(directoryName, "\n")] = 0;

  directoryPosition previous = NULL;
  directoryPosition directoryToDelete = current->subDirectory;

  while (directoryToDelete) {
    bool desiredNameMatchesCurrentDirectoryName =
        strcmp(directoryToDelete->name, directoryName) == 0;
    if (desiredNameMatchesCurrentDirectoryName) {
      bool directoryIsNotEmpty = directoryToDelete->subDirectory;
      if (directoryIsNotEmpty) {
        printf("Directory is not empty.\n");
        return DIRECTORY_ERROR;
      }

      // Getting it out of the list
      if (previous) {
        previous->sibling = directoryToDelete->sibling;
      } else {
        current->subDirectory = directoryToDelete->sibling;
      }

      free(directoryToDelete);
      return SUCCESS;
    }
    previous = directoryToDelete;
    directoryToDelete = directoryToDelete->sibling;
  }

  printf("Directory not found.\n");
  return DIRECTORY_ERROR;
}

int push(stackPosition head, directoryPosition directory) {
  stackPosition newSubdirectory = malloc(sizeof(Stack));
  newSubdirectory->directory = directory;
  newSubdirectory->next = head->next;
  head->next = newSubdirectory;
  return SUCCESS;
}

directoryPosition pop(stackPosition head) {
  bool stackIsEmpty = !head->next;
  if (stackIsEmpty)
    return NULL;

  stackPosition stackNodeToDelete = head->next;
  directoryPosition directory = stackNodeToDelete->directory;
  head->next = stackNodeToDelete->next;
  free(stackNodeToDelete);
  return directory;
}

int printPathRecursive(stackPosition node) {
  bool nodeNotFound = !node;
  if (nodeNotFound) {
    return DIRECTORY_ERROR;
  }

  printPathRecursive(node->next);
  printf("%s\\", node->directory->name);
  return SUCCESS;
}

int printPath(stackPosition head, directoryPosition current) {
  printf("\n");
  printPathRecursive(head->next);
  return SUCCESS;
}

int freeDirectoryTree(directoryPosition directory) {
  bool noDirectoryExists = !directory;
  if (noDirectoryExists)
    return SUCCESS;

  // Free the subdirectories
  freeDirectoryTree(directory->subDirectory);

  // Then free the siblings
  freeDirectoryTree(directory->sibling);

  free(directory);
  return SUCCESS;
}

int freeStack(stackPosition head) {
  while (head->next) {
    pop(head);
  }
  return SUCCESS;
}
