#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NAMELEN 30
#define PATHLEN 30

typedef struct node {
  int index;
  char name[NAMELEN];
  float price;
  int quantity;
  struct node *next;
} node;

void printItems(node *head, float cash);
void menuItems(node **head);
void menuLoadItems(node **head);
void menuSaveItems(node *head);
void menuAddItem(node **head);
void menuRemoveItem(node **head);
void freeMem(node **head);
float menuSetCash();


void addItem(node **head, char *name, \
            float price, int quantity) {
  if (*head == NULL) {
    *head = (node*) malloc(sizeof(node));
    (*head)->index = 1;
    strcpy((*head)->name, name);
    (*head)->price = price;
    (*head)->quantity = quantity;
    (*head)->next = NULL;
  } else {
    int counter = 2;
    node *current = *head;
    while (current->next != NULL) {
      current = current-> next;
      counter++;
    }

    node *prev = current;
    current = (node*) malloc(sizeof(node));
    prev->next = current;
    current->index = counter;
    strcpy(current->name, name);
    current->price = price;
    current->quantity = quantity;
    current->next = NULL;
  }
}
void decreaseIndexes(node *begin_node) {
  node *current = begin_node;
  while (current != NULL) {
    current->index--;
    current = current->next;
  }
}
int removeItem(node **head, int index) {
  node *current, *temp;
  if (index < 1) return -1;
  if (index == 1) {
    temp = (*head)->next;
    free(*head);
    *head = temp;
    decreaseIndexes(*head);
    return 0;
  }

  int counter = 1;
  temp = *head;
  current = (*head)->next;
  while (current != NULL) {
    counter++;
    if (counter == index) {
      temp->next = current->next;
      free(current);
      decreaseIndexes(temp->next);
      return 0;
    }
    current = current->next;
    temp = temp->next;
  }

  return 1; //index poza zakresem
}
void saveItems(node *head, char *path) {
  FILE *output = fopen(path, "w");
  node *current = head;
  while (current != NULL) {
    fprintf(output, "%s,%f,%d\n", current->name, \
            current->price, current->quantity);
    current = current->next;
  }
  fclose(output);
}
int loadItems(node **head, char *path) {
  FILE *input = fopen(path, "r");
  if (input == NULL) return 1;
  node *current, *prev;
  char name[NAMELEN];
  float price;
  int quantity;
  
  if (head != NULL) //nadpisanie aktualnej listy
    freeMem(head);

  while (fscanf(input, "%[^,],%f,%d\n", \
            name, &price, &quantity) > 0)
    addItem(head, name, price, quantity);
  fclose(input);
  return 0;
}
void freeMem(node **head) {
  node *current = *head, *temp;
  while (current != NULL) {
    temp = current->next;
    free(current);
    current = temp;
  }
  *head = NULL;
}
void sampleList(node **head) {
  addItem(head, "sok", 3.50, 2);
  addItem(head, "ciastko", 3.14, 5);
  addItem(head, "banan", 1.23, 3);
  addItem(head, "pizza", 24.99, 1);
}

//UI
void clearScreen() {
  #if defined(__linux__)
    system("clear");
  #endif
  #if defined(_WIN64) || defined(_WIN32)
    system("cls");
  #endif
}
void goBack() {
  printf("enter - powrot\n");
  getchar();
  getchar();
}
void menuMain(node **head) {
  int choice;
  while (1) {
    clearScreen();
    printf("Lista zakupow - menu glowne\n");
    printf("1: pokaz liste\n");
    printf("2: wczytaj liste z pliku\n");
    printf("3: zapisz liste do pliku\n\n");
    printf("9: przykladowa lista\n");
    printf("0: wyjscie\n");
    scanf("%d", &choice);
    clearScreen();
    switch (choice) {
      case 1:
        menuItems(head);
        break;
      case 2:
        menuLoadItems(head);
        goBack();
        break;
      case 3:
        menuSaveItems(*head);
        goBack();
        break;
      case 9:
        sampleList(head);
        break;
      case 0:
        return;
        break;
      default:
        printf("Niewlasciwy wybor.\n");
        goBack();
        break;
    }
  }
}
void menuItems(node **head) {
  int choice;
  float cash = 0;
  while (1) {
    clearScreen();
    printItems(*head, cash);
    if (cash != 0) cash = 0; //reset
    printf("\n");
    printf("1: dodaj produkt\n");
    printf("2: usun produkt\n");
    printf("3: oblicz reszte\n\n");
    printf("0: powrot\n");

    scanf("%d", &choice);
    switch(choice) {
      case 1:
        menuAddItem(head);
        goBack();
        break;
      case 2:
        menuRemoveItem(head);
        goBack();
        break;
      case 3:
        cash = menuSetCash();
        break;
      case 0:
        return;
      default:
        printf("Niewlasciwy wybor.\n");
        goBack();
        break;
    }
  }
}

void menuAddItem(node **head) {
  char name[NAMELEN];
  float price;
  int quantity;
  printf("\nDodawanie produktu\n");
  printf("Nazwa: ");  scanf("%s", name);
  printf("Cena: ");   scanf("%f", &price);
  printf("Ilosc: ");  scanf("%d", &quantity);
  addItem(head, name, price, quantity);
  printf("Produkt dodany pomyslnie.\n");
}

void menuRemoveItem(node **head) {
  int index;
  printf("\nUsuwanie produktu\n");
  printf("Indeks: "); scanf("%d", &index);
  if(removeItem(head, index) != 0)
    printf("Podano niewlasciwy indeks.\n");
  else
    printf("Produkt usuniety pomyslnie.\n");
}

void printItems(node *head, float cash) {
  node *current = head;
  float total = 0;
  if (head == NULL) {
    printf("Lista zakupow jest pusta.\n");
    return;
  }

  while (current != NULL) {
    printf("%d: %s, %d x %.2f zl/szt.\n", current->index, \
          current->name, current->quantity, current->price);
    total += (current->price * current->quantity);
    current = current->next;
  }
  printf("suma: %.2f zl\n", total);
  if (cash > 0) {
    if (cash - total < 0)
      printf("brakuje: %.2f zl\n", total - cash);
    else
      printf("reszta: %.2f zl\n", cash - total);
  }
}

float menuSetCash() {
  float cash;
  clearScreen();
  printf("Kwota od ktorej obliczyc reszte: ");
  scanf("%f", &cash);
  return cash;
}

void menuLoadItems(node **head) {
  char path[PATHLEN];
  printf("Odczyt listy z pliku\n");
  printf("Sciezka do pliku: ");
  scanf("%s", path);
  if (loadItems(head, path) != 0)
    printf("Otwarcie pliku nieudane.\n");
  else
    printf("Lista wczytana pomyslnie.\n");
}

void menuSaveItems(node *head) {
  char path[PATHLEN];
  printf("Zapis listy do pliku\n");
  printf("Sciezka do pliku: ");
  scanf("%s", path);
  saveItems(head, path);  
}


int main(void) {
  node *head = (node*) malloc(sizeof(node));
  head = NULL;
  
  menuMain(&head);
  
  freeMem(&head);
  return 0;
}
