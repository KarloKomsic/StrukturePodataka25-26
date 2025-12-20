/* Napisati program koji pomoću vezanih listi (stabala) predstavlja strukturu
direktorija. Omogućiti unos novih direktorija i pod-direktorija, ispis sadržaja
direktorija i povratak u prethodni direktorij. Točnije program treba preko
menija simulirati korištenje DOS naredbi: 1- "md", 2 - "cd dir", 3 - "cd..", 4 -
"dir" i 5 – izlaz. */

// NOTE: Added additional function for rd (remove empty directory)

#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 64
#define MAX_COMMAND 128

#define SUCCESS 0
#define DIRECTORY_ERROR -1

typedef struct directory *DirectoryPosition;
typedef struct directory {
  char name[MAX_NAME];
  DirectoryPosition sibling;      // next direcotry ON THE SAME LEVEL
  DirectoryPosition subDirectory; // first subdirectory
} Directory;

typedef struct stack *stackPosition;
typedef struct stack {
  DirectoryPosition directory;
  stackPosition next;
} Stack;

DirectoryPosition createDirectory(char *directoryName);
int md(DirectoryPosition current, char *newDirectoryName);
int cdDir(DirectoryPosition *current, stackPosition stackHead,
          char *subDirectoryName);
int cdBack(DirectoryPosition *current, stackPosition stackHead);
int dir(DirectoryPosition current);
int rd(DirectoryPosition current, char *directoryName);
int push(stackPosition stackHead, DirectoryPosition directory);
DirectoryPosition pop(stackPosition head);
int printPathRecursive(stackPosition node);
int printPath(stackPosition head, DirectoryPosition current);
int freeDirectoryTree(DirectoryPosition directory);
int freeStack(stackPosition head);

int main() {
  Directory head = {"C:", NULL, NULL};
  Stack stackHead = {NULL, NULL};
  DirectoryPosition current = &head;

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

DirectoryPosition createDirectory(char *directoryName) {
  DirectoryPosition directory = malloc(sizeof(Directory));
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

int md(DirectoryPosition current, char *newDirectoryName) {
  newDirectoryName[strcspn(newDirectoryName, "\n")] = 0;

  DirectoryPosition newDirectory = createDirectory(newDirectoryName);
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
    DirectoryPosition temp = current->subDirectory;
    while (temp->sibling)
      temp = temp->sibling;
    temp->sibling = newDirectory;
  }

  return SUCCESS;
}

int cdDir(DirectoryPosition *current, stackPosition stackHead,
          char *subDirectoryName) {
  subDirectoryName[strcspn(subDirectoryName, "\n")] = 0;
  DirectoryPosition temp = (*current)->subDirectory;
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

int cdBack(DirectoryPosition *current, stackPosition stackHead) {
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

int dir(DirectoryPosition current) {
  DirectoryPosition temp = current->subDirectory;
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

int rd(DirectoryPosition current, char *directoryName) {
  directoryName[strcspn(directoryName, "\n")] = 0;

  DirectoryPosition previous = NULL;
  DirectoryPosition directoryToDelete = current->subDirectory;

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

int push(stackPosition head, DirectoryPosition directory) {
  stackPosition newSubdirectory = malloc(sizeof(Stack));
  newSubdirectory->directory = directory;
  newSubdirectory->next = head->next;
  head->next = newSubdirectory;
  return SUCCESS;
}

DirectoryPosition pop(stackPosition head) {
  bool stackIsEmpty = !head->next;
  if (stackIsEmpty)
    return NULL;

  stackPosition stackNodeToDelete = head->next;
  DirectoryPosition directory = stackNodeToDelete->directory;
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

int printPath(stackPosition head, DirectoryPosition current) {
  printf("\n");
  printPathRecursive(head->next);
  return SUCCESS;
}

int freeDirectoryTree(DirectoryPosition directory) {
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
