/* Napisati program koji čita datoteku racuni.txt u kojoj su zapisani nazivi
 * svih datoteka koji predstavljaju pojedini račun. Na početku svake datoteke je
 * zapisan datum u kojem vremenu je račun izdat u formatu YYYY-MM-DD. Svaki
 * sljedeći red u datoteci predstavlja artikl u formatu naziv, količine, cijena.
 * Potrebno je formirati vezanu listu računa sortiranu po datumu. Svaki čvor
 * vezane liste sadržava vezanu listu artikala sortiranu po nazivu artikla.
 * Nakon toga je potrebno omogućiti upit kojim će korisnik saznati koliko je
 * novaca sveukupno potršeno na specifični artikl u određenom vremenskom
 * razdoblju i u kojoj je količini isti kupljen.*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 128

#define SUCCESS 0
#define FILE_ERROR -1
#define MALLOC_ERROR -2

typedef struct item {
  char name[MAX_NAME];
  int quantity;
  double price;
  struct item *next;
} Item;

typedef struct receipt {
  char date[11]; // YYYY-MM-DD + '\0'
  Item *itemList;
  struct receipt *next;
} Receipt;

Receipt *loadAllReceipts(const char *listFileName, int *status);
Receipt *loadSingleReceipt(Receipt *head, const char *fileName, int *status);
Receipt *insertReceiptSorted(Receipt *head, Receipt *newReceipt);
Item *insertItemSorted(Item *head, Item *newItem);
int compareDates(const char *firstDate, const char *secondDate);
int totalQuery(Receipt *head, const char *itemName, const char *start,
               const char *end, int *totalQuantity, double *totalSpent);
Receipt *filterReceiptsByDateRange(Receipt *head, const char *start,
                                   const char *end);
int freeAll(Receipt *head);
int printReceipts(Receipt *head);
int printReceipt(Receipt *receipt);
int printItems(Item *head);

int main() {
  int status = SUCCESS;
  Receipt *receipts = loadAllReceipts("receipts.txt", &status);
  if (status != SUCCESS) {
    printf("Error loading receipts.\n");
    return FILE_ERROR;
  }

  int choice = 0;
  char item[MAX_NAME];
  char start[11], end[11];

  printf("===============================================\n");

  while (1) {
    printf("\n*** RECEIPT CLI MENU ***\n");
    printf("1) Print all receipts\n");
    printf("2) Print receipts in date range\n");
    printf("3) Query total spent on item in date range\n");
    printf("4) Exit\n");
    printf("Choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      printReceipts(receipts);
      break;
    case 2:
      printf("Start date (YYYY-MM-DD): ");
      scanf("%10s", start);
      printf("End date (YYYY-MM-DD): ");
      scanf("%10s", end);
      {
        Receipt *filtered = filterReceiptsByDateRange(receipts, start, end);
        printReceipts(filtered);
      }
      break;
    case 3:
      printf("Item name: ");
      scanf(" %[^\n]", item);
      printf("Start date (YYYY-MM-DD): ");
      scanf("%10s", start);
      printf("End date (YYYY-MM-DD): ");
      scanf("%10s", end);
      int quantity = 0;
      double spent = 0.0;
      totalQuery(receipts, item, start, end, &quantity, &spent);
      printf("Item %s bought %d times. Total spent: %.2lf EUR\n", item,
             quantity, spent);
      break;
    case 4:
      freeAll(receipts);
      printf("Thank you for using the Receipt CLI program!\n");
      printf("===============================================\n");
      return SUCCESS;
      break;
    default:
      printf("The choice is invalid. Please enter a valid option.\n");
    }
  }
}

Receipt *loadAllReceipts(const char *listFileName, int *status) {
  FILE *file = fopen(listFileName, "r");
  bool fileNotFound = !file;
  if (fileNotFound) {
    *status = FILE_ERROR;
    return NULL;
  }
  Receipt *head = NULL;
  char fileName[MAX_NAME];
  while (fscanf(file, " %127s", fileName) == 1) {
    head = loadSingleReceipt(head, fileName, status);
    if (*status != SUCCESS)
      break;
  }
  fclose(file);
  *status = SUCCESS;
  return head;
}

Receipt *loadSingleReceipt(Receipt *head, const char *fileName, int *status) {
  FILE *file = fopen(fileName, "r");
  bool fileNotFound = !file;
  if (fileNotFound) {
    *status = FILE_ERROR;
    return head;
  }

  Receipt *receipt = malloc(sizeof(Receipt));
  if (!receipt) {
    fclose(file);
    *status = MALLOC_ERROR;
    return head;
  }

  fscanf(file, " %10s", receipt->date);
  receipt->itemList = NULL;
  receipt->next = NULL;

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    char name[MAX_NAME];
    int qty;
    double price;
    if (sscanf(line, " %127[^,], %d, %lf", name, &qty, &price) == 3) {
      Item *item = malloc(sizeof(Item));
      strcpy(item->name, name);
      item->quantity = qty;
      item->price = price;
      item->next = NULL;
      receipt->itemList = insertItemSorted(receipt->itemList, item);
    }
  }

  fclose(file);
  head = insertReceiptSorted(head, receipt);
  *status = SUCCESS;
  return head;
}

Receipt *insertReceiptSorted(Receipt *head, Receipt *newReceipt) {
  if (!head || compareDates(newReceipt->date, head->date) < 0) {
    newReceipt->next = head;
    return newReceipt;
  }
  Receipt *current = head;
  while (current->next &&
         compareDates(newReceipt->date, current->next->date) > 0)
    current = current->next;
  newReceipt->next = current->next;
  current->next = newReceipt;
  return head;
}

Item *insertItemSorted(Item *head, Item *newItem) {
  if (!head || strcmp(newItem->name, head->name) < 0) {
    newItem->next = head;
    return newItem;
  }
  Item *current = head;
  while (current->next && strcmp(newItem->name, current->next->name) > 0)
    current = current->next;
  newItem->next = current->next;
  current->next = newItem;
  return head;
}

int compareDates(const char *firstDate, const char *secondDate) {
  return strcmp(firstDate, secondDate);
}

int totalQuery(Receipt *head, const char *itemName, const char *start,
               const char *end, int *totalQuantity, double *totalSpent) {
  *totalQuantity = 0;
  *totalSpent = 0;
  while (head) {
    if (compareDates(head->date, start) >= 0 &&
        compareDates(head->date, end) <= 0) {
      Item *item = head->itemList;
      while (item) {
        if (strcmp(item->name, itemName) == 0) {
          *totalQuantity += item->quantity;
          *totalSpent += item->quantity * item->price;
        }
        item = item->next;
      }
    }
    head = head->next;
  }
  return SUCCESS;
}

Receipt *filterReceiptsByDateRange(Receipt *head, const char *start,
                                   const char *end) {
  Receipt *filtered = NULL;
  while (head) {
    if (compareDates(head->date, start) >= 0 &&
        compareDates(head->date, end) <= 0) {
      Receipt *copy = malloc(sizeof(Receipt));
      strcpy(copy->date, head->date);
      copy->itemList = head->itemList; // shared pointer
      copy->next = NULL;
      filtered = insertReceiptSorted(filtered, copy);
    }
    head = head->next;
  }
  return filtered;
}

int freeAll(Receipt *head) {
  while (head) {
    Receipt *receipt = head;
    head = head->next;
    Item *item = receipt->itemList;
    while (item) {
      Item *toDelete = item;
      item = item->next;
      free(toDelete);
    }
    free(receipt);
  }
  return SUCCESS;
}

int printItems(Item *head) {
  while (head) {
    printf("  %s x%d, %.2lf EUR\n", head->name, head->quantity, head->price);
    head = head->next;
  }
  return SUCCESS;
}

int printReceipt(Receipt *receipt) {
  printf("Date: %s\n", receipt->date);
  printItems(receipt->itemList);
  printf("\n");
  return SUCCESS;
}

int printReceipts(Receipt *head) {
  while (head) {
    printReceipt(head);
    head = head->next;
  }
  return SUCCESS;
}
