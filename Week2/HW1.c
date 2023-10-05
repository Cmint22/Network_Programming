#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Account {
    char name[50];
    char password[50];
    int status;
} Account;

typedef struct node {
    Account account;
    struct node *next;
} node;

void printMenu();
node *readFile(FILE *f);
void printList(node *head);
void saveToFile(node *head);
void registerAccount(node *head);
node *signinAccount(node *head);
void searchAccountStatus(node *head);
void signoutAccount(node *currentAccount);

int main() {
    int choice = 0;
    node *head = NULL;
    node *currentAccount = NULL;
    FILE *fptr = fopen("account.txt", "r");
    if (fptr == NULL){
        printf("Open file failed\n");
        exit(1);
    }
    head = readFile(fptr);
    fclose(fptr);
    printList(head);
    do {
        printMenu();
        scanf("%d", &choice);
        switch (choice){
            case 1:
                registerAccount(head);
                break;
            case 2:
                currentAccount = signinAccount(head);
                break;
            case 3:
                searchAccountStatus(head);
                break;
            case 4:
                signoutAccount(currentAccount);
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }while(choice != 4);
    return 0;
}

void printMenu(){
    printf("\nUSER MANAGEMENT PROGRAM\n");
    printf("-------------------------------------\n");
    printf("1. Register\n");
    printf("2. Sign in\n");
    printf("3. Search\n");
    printf("4. Sign out\n");
    printf("Your choice (1-4, other to quit): ");
}

/**
 * Function readFile: reads a file containing user accounts anf returns a linked list of node object.
 * @param f: a pointer to the file to read.
 * @return: a pointer to the head of the linked list of node structures.
**/
node *readFile(FILE *f){
    node *head = NULL;
    node *current = NULL;
    node *newNode = NULL;
    char name[50];
    char password[50];
    int status;
    while (fscanf(f, "%s %s %d", name, password, &status) != EOF){
        newNode = (node *) malloc(sizeof(node));
        strcpy(newNode->account.name, name);
        strcpy(newNode->account.password, password);
        newNode->account.status = status;
        newNode->next = NULL;
        if (head == NULL){
            head = newNode;
            current = newNode;
        } else {
            current->next = newNode;
            current = newNode;
        }
    }
    return head;
}

void saveToFile(node *head){
    FILE *fptr = fopen("account.txt", "w");
    node *current = head;
    while (current != NULL){
        fprintf(fptr, "%s %s %d\n", current->account.name, current->account.password, current->account.status);
        current = current->next;
    }
    fclose(fptr);
}

void printList(node *head){
    node *p = head;
    printf("List of accounts:\n");
    while (p != NULL){
        printf("%s %s %d\n", p->account.name, p->account.password, p->account.status);
        p = p->next;
    }
    printf("\n");
}

/**
 * Function registerAccount: take a pointer to the head of a linked list of node structures as input.
 * @param head: a pointer to the head of a linked list of node structures.
 * @return: none.
**/
void registerAccount(node *head){
    node *p = head;
    char name[50];
    char password[50];
    int status;
    printf("Username: ");
    scanf("%s", name);
    while (p != NULL){
        if (strcmp(p->account.name, name) == 0){
            printf("Account existed\n");
            return;
        }
        p = p->next;
    }
    printf("Password: ");
    scanf("%s", password);
    status = 2;
    
    node *newNode = (node *) malloc(sizeof(node));
    strcpy(newNode->account.name, name);
    strcpy(newNode->account.password, password);
    newNode->account.status = status;
    newNode->next = NULL;
    if (head == NULL){
        head = newNode;
    } else {
        p = head;
        while (p->next != NULL){
            p = p->next;
        }
        p->next = newNode;
    }
    saveToFile(head);
    printf("Successfully registration. Activation required\n");
}

/**
 * Function signinAccount: sign in an account in a linked list of accounts.
 * @param head: a pointer to the head of the linked list of nodes of type node.
 * @return: a pointer to the account node if the sign in is successful.
            NULL if the sign in is unsuccessful.
**/
node *signinAccount(node *head){
    node *p = head;
    char name[50];
    char password[50];
    int status;
    printf("Username: ");
    scanf("%s", name);
    node *temp = NULL;
    while (p != NULL){
        if (strcmp(p->account.name, name) == 0){
            temp = p;
        }
        p = p->next;
    }
    if (temp == NULL){
        printf("Cannot search account\n");
        return NULL;
    }
    if (temp->account.status == 0){
        printf("Account is blocked\n");
        return NULL;
    }
    else if (temp->account.status == 2){
        printf("Account is not activated\n");
        return NULL;
    }
    int count =0 ;
    do {
        printf("Password: ");
        scanf("%s", password);
        if (strcmp(password, temp->account.password) == 0){
            printf("Hello %s\n", temp->account.name);
            return temp;
        }
        else{
            count++;
            printf("Password is incorrect\n");
        }
    } while (count < 4);
    printf("Account is blocked\n");
    temp->account.status = 0;
    saveToFile(head);
    return NULL;
}    
    
/**
 * Function searchAccountStatus: searches for the status of an account in a linked list of accounts.
 * @param head: a pointer to the head node of a linked list of accounts.
 * @return: void
**/
void searchAccountStatus(node *head){
    node *p = head;
    char name[50];
    printf("Username: ");
    scanf("%s", name);
    while (p != NULL){
        if (strcmp(p->account.name, name) == 0){
            if (p->account.status == 0){
                printf("Account is blocked\n");
                return;
            }
            else if (p->account.status == 1){
                printf("Account is activate\n");
                return;
            }
            else if (p->account.status == 2){
                printf("Account is not activate\n");
                return;
            }
        }
        p = p->next;
    }
    printf("Cannot search account\n");
}

/**
 * Function signoutAccount: takes a pointer to a node structure as its parameter.
 * @param currentAccount: a pointer to a node object representing the account to be signed out.
 * @return: void
**/
void signoutAccount(node *currentAccount){
    if (currentAccount == NULL){
        printf("Account is not sign in\n");
        return;
    }
    printf("Goodbye %s\n", currentAccount->account.name);
    currentAccount = NULL;
}