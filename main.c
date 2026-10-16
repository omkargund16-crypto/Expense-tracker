#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "cJSON.c"

#define MAX 100

struct Expense {
    char name[40];
    float amount;
};

struct Expense e[MAX];
int n = 0;
float budget = 0;

void saveData()
{
    int i;
    FILE *f;
    cJSON *root = cJSON_CreateObject();
    cJSON *list = cJSON_CreateArray();

    cJSON_AddNumberToObject(root, "budget", budget);

    for (i = 0; i < n; i++) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "name", e[i].name);
        cJSON_AddNumberToObject(item, "amount", e[i].amount);
        cJSON_AddItemToArray(list, item);
    }

    cJSON_AddItemToObject(root, "expenses", list);

    f = fopen("expenses.json", "w");
    if (f != NULL) {
        char *text = cJSON_Print(root);
        if (text != NULL) {
            fputs(text, f);
            free(text);
            printf("\nData saved successfully!\n");
        }
        fclose(f);
    } else {
        printf("\nError saving data!\n");
    }

    cJSON_Delete(root);
}

void loadData()
{
    FILE *f = fopen("expenses.json", "r");
    char text[20000];
    cJSON *root, *list, *item, *name, *amount, *b;
    int i;

    if (f == NULL) {
        printf("Starting a new expense record.\n");
        return;
    }

    if (fgets(text, sizeof(text), f) == NULL) {
        fclose(f);
        return;
    }
    fclose(f);

    root = cJSON_Parse(text);
    if (root == NULL) {
        printf("Could not read saved JSON data.\n");
        return;
    }

    b = cJSON_GetObjectItem(root, "budget");
    list = cJSON_GetObjectItem(root, "expenses");

    if (cJSON_IsNumber(b))
        budget = (float)b->valuedouble;

    if (cJSON_IsArray(list)) {
        for (i = 0; i < cJSON_GetArraySize(list) && n < MAX; i++) {
            item = cJSON_GetArrayItem(list, i);
            name = cJSON_GetObjectItem(item, "name");
            amount = cJSON_GetObjectItem(item, "amount");

            if (cJSON_IsString(name) && cJSON_IsNumber(amount)
                && strlen(name->valuestring) < sizeof(e[n].name)) {
                strcpy(e[n].name, name->valuestring);
                e[n].amount = (float)amount->valuedouble;
                n++;
            }
        }
    }

    cJSON_Delete(root);
    printf("Saved data loaded successfully!\n");
}

int main()
{
    int choice, i;
    float total;

    loadData();

    if (budget == 0) {
        printf("Enter your monthly budget (Rs.): ");
        if (scanf("%f", &budget) != 1 || budget < 0) {
            printf("Invalid budget!\n");
            return 1;
        }
        saveData();
    }

    do {
        printf("\n===== PERSONAL EXPENSE TRACKER =====\n");
        printf("1. Add Expense\n");
        printf("2. View Expenses\n");
        printf("3. View Budget Summary\n");
        printf("4. Change Monthly Budget\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
            break;

        switch (choice) {
        case 1:
            if (n >= MAX) {
                printf("Expense limit reached!\n");
                break;
            }

            printf("Enter expense name (one word): ");
            if (scanf("%39s", e[n].name) != 1)
                break;

            printf("Enter amount (Rs.): ");
            if (scanf("%f", &e[n].amount) != 1 ||
                e[n].amount < 0) {
                printf("Invalid amount!\n");
                break;
            }

            n++;
            saveData();
            break;

        case 2:
            if (n == 0) {
                printf("No expenses recorded yet.\n");
                break;
            }

            printf("\n--- Your Expenses ---\n");
            for (i = 0; i < n; i++)
                printf("%d. %s - Rs. %.2f\n",
                       i + 1, e[i].name, e[i].amount);
            break;

        case 3:
            total = 0;
            for (i = 0; i < n; i++)
                total += e[i].amount;

            printf("\nMonthly Budget: Rs. %.2f\n", budget);
            printf("Total Spent: Rs. %.2f\n", total);
            printf("Remaining: Rs. %.2f\n", budget - total);

            if (total > budget)
                printf("Warning: You exceeded your budget!\n");
            break;

        case 4:
            printf("Enter new monthly budget: ");
            if (scanf("%f", &budget) != 1 || budget < 0) {
                printf("Invalid budget!\n");
                break;
            }
            saveData();
            break;

        case 0:
            saveData();
            printf("Thank you for using Expense Tracker!\n");
            break;

        default:
            printf("Invalid choice. Try again.\n");
        }

    } while (choice != 0);

    return 0;
}
