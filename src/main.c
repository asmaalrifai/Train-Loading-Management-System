#include <stdio.h>
#include <stdlib.h>
#include "../include/train.h"

extern Train *create_mock_train();

void display_menu() {
    printf("=== Train Loading Management System ===\n");
    printf("1. Load train status from file\n");
    printf("2. Load material, starting from first suitable wagon from head of the Train\n");
    printf("3. Load material to specific wagon\n");
    printf("4. Unload material, starting from first suitable wagon from tail of the Train\n");
    printf("5. Unload material from specific wagon\n");
    printf("6. Display train status\n");
    printf("7. Display materials status\n");
    printf("8. Empty train\n");
    printf("9. Save train status to file\n");
    printf("10. Exit\n");
}

int main() {
    Train *train = create_mock_train();

    MaterialType materials[] = {
        {"Large Box", 200.0, 50, 5},
        {"Medium Box", 150.0, 50, 10},
        {"Small Box", 100.0, 50, 15}
    };
    int material_count = 3;

    int choice;
    while (1) {
        display_menu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 6:
            display_train_status(train);
            break;
        case 7:
            display_material_status(materials, material_count);
            break;
        case 10:
            printf("\n==========\nExiting...\n==========\n\n");
            exit(0);
        default:
            printf("\n==========\nOption not implemented yet.\n==========\n\n");
        }
    }

    return 0;
}
