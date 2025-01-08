// main.c
#include <stdio.h>
#include <stdlib.h>
#include "../include/train.h"

extern Train *create_mock_train();

void display_menu()
{
    printf("=== WELCOME TO ASMA AND IREM TRAIN ===\n");
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

int main()
{
    Train *train = create_mock_train();

    MaterialType materials[] = {
        {"Large Box", 200.0, 50, 0},
        {"Medium Box", 150.0, 50, 0},
        {"Small Box", 100.0, 50, 0}};
    int material_count = 3;

    int choice = 0;
    char input[50]; // Buffer for user input

    load_train_status_from_file(train, "train_status.txt");
    while (1)
    {
        display_menu();
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin); // Read input as string

        // Validate and convert input to integer
        if (sscanf(input, "%d", &choice) != 1)
        {
            printf("\n==========\nInvalid input. Please enter a number.\n==========\n\n");
            continue;
        }

        if (choice < 1 || choice > 10)
        {
            printf("\n==========\nOption unavailable.\n==========\n\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            load_train_status_from_file(train, "train_status.txt");
            break;
        case 2:
            load_specified_material_to_train_main(train, materials, material_count);
            break;
        case 3:
            load_material_to_wagon_main(train, materials, material_count);
            break;
        case 4:
            unload_material_from_tail(train);
            break;
        case 5:
            unload_material_from_wagon_main(train, materials, material_count);
            break;
        case 6:
            display_train_status(train);
            break;
        case 7:
            display_material_status(materials, material_count, train);
            break;
        case 8:
            empty_train_or_wagon(train);
            break;
        case 9:
            save_train_status_to_file(train, "train_status.txt");
            break;
        case 10:
            //  save_train_status_to_file(train, "train_status.txt");
            printf("\n==========\nExiting...\n==========\n\n");
            exit(0);
        default:
            printf("\n==========\nOption unavailable.\n==========\n\n");
        }
    }

    return 0;
}

//  save_train_status_to_file(train, "train_status.txt");