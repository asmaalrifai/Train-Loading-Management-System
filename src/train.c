// train.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/train.h"
#include "../include/wagon.h"
#include "../include/material.h"
#include "../include/file_ops.h"
#include "../include/utils.h"

Train *create_mock_train()
{
    Train *train = (Train *)malloc(sizeof(Train));
    strcpy(train->train_id, "T123");
    train->wagon_count = 2;

    Wagon *wagon1 = (Wagon *)malloc(sizeof(Wagon));
    wagon1->wagon_id = 1;
    wagon1->max_weight = 1000.0;
    wagon1->current_weight = 0.0;
    wagon1->loaded_materials = NULL;
    wagon1->next = NULL;
    wagon1->prev = NULL;

    Wagon *wagon2 = (Wagon *)malloc(sizeof(Wagon));
    wagon2->wagon_id = 2;
    wagon2->max_weight = 1000.0;
    wagon2->current_weight = 0.0;
    wagon2->loaded_materials = NULL;
    wagon2->next = NULL;
    wagon2->prev = wagon1;

    wagon1->next = wagon2;
    train->first_wagon = wagon1;

    return train;
}

void display_train_status(Train *train)
{
    if (train == NULL || train->first_wagon == NULL)
    {
        printf("\n==========\nNo wagons in the train.\n==========\n\n");
        return;
    }

    printf("\n==========\nTrain ID: %s\nTotal Wagons: %d\n==========\n", train->train_id, train->wagon_count);

    Wagon *current_wagon = train->first_wagon;
    while (current_wagon != NULL)
    {
        printf("Wagon ID: %d\n", current_wagon->wagon_id);
        printf("  Max Weight: %.2f kg\n", current_wagon->max_weight);
        printf("  Current Weight: %.2f kg\n", current_wagon->current_weight);

        if (current_wagon->loaded_materials == NULL)
        {
            printf("  No materials loaded.\n");
        }
        else
        {
            printf("  Loaded Materials:\n");

            // Debugging statement to confirm traversal
            printf("Traversing Loaded Materials for Wagon %d\n", current_wagon->wagon_id);

            // Traverse the LoadedMaterial list
            LoadedMaterial *current_material = current_wagon->loaded_materials;
            while (current_material != NULL)
            {
                printf("    - %s: %.2f kg\n",
                       current_material->type->name,
                       current_material->type->weight);
                current_material = current_material->next; // Move to the next material
            }
        }
        printf("\n");
        current_wagon = current_wagon->next;
    }
}

void load_material_to_train(Train *train, MaterialType *material)
{
    if (train == NULL || material == NULL)
    {
        printf("\n==========\nError: Train or material data is missing.\n==========\n\n");
        return;
    }

    int remaining_quantity = material->quantity - material->loaded;
    if (remaining_quantity <= 0)
    {
        printf("\n==========\nNo more %s available to load.\n==========\n\n", material->name);
        return;
    }

    Wagon *current_wagon = train->first_wagon;

    while (remaining_quantity > 0)
    {
        // Create a new wagon if necessary
        if (current_wagon == NULL)
        {
            current_wagon = create_new_wagon(train);
        }

        while (remaining_quantity > 0 && check_wagon_space(current_wagon, material))
        {
            insert_material_into_wagon(current_wagon, material);
            current_wagon->current_weight += material->weight;
            material->loaded += 1;
            remaining_quantity -= 1;
        }

        current_wagon = current_wagon->next;
    }

    printf("\n==========\nMaterial loading completed.\n==========\n\n");
}

void load_specified_material_to_train(Train *train, MaterialType *material, int quantity)
{
    if (train == NULL || material == NULL)
    {
        printf("\n==========\nError: Train or material data is missing.\n==========\n\n");
        return;
    }

    if (!check_material_availability(material, quantity))
    {
        printf("\n==========\nInvalid quantity. Available quantity: %d\n==========\n\n", material->quantity - material->loaded);
        return;
    }

    int remaining_quantity = quantity;
    Wagon *current_wagon = train->first_wagon;

    while (remaining_quantity > 0)
    {
        if (current_wagon == NULL)
        {
            current_wagon = create_new_wagon(train);
        }

        while (remaining_quantity > 0 && check_wagon_space(current_wagon, material))
        {
            insert_material_into_wagon(current_wagon, material);
            current_wagon->current_weight += material->weight;
            material->loaded += 1;
            remaining_quantity -= 1;
        }

        current_wagon = current_wagon->next;
    }

    printf("\n==========\nMaterial loading completed.\n==========\n\n");
}

void load_specified_material_to_train_main(Train *train, MaterialType *materials, int material_count)
{
    char input[50];
    int material_choice;
    int quantity;

    // Display material options
    printf("Select material to load:\n");
    for (int i = 0; i < material_count; i++)
    {
        printf("%d. %s\n", i + 1, materials[i].name);
    }

    // Validate material choice
    while (1)
    {
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &material_choice) != 1 || material_choice < 1 || material_choice > material_count)
        {
            printf("\n==========\nInvalid material choice. Please try again.\n==========\n\n");
            continue;
        }
        break;
    }

    // Validate quantity to load
    while (1)
    {
        printf("Enter the number of materials to load: ");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &quantity) != 1 || quantity <= 0)
        {
            printf("\n==========\nInvalid quantity. Please enter a positive number.\n==========\n\n");
            continue;
        }
        break;
    }

    load_specified_material_to_train(train, &materials[material_choice - 1], quantity);
}

// 4
void unload_material_from_tail(Train *train)
{
    if (train == NULL || train->first_wagon == NULL)
    {
        printf("\n==========\nError: Train or wagons are missing.\n==========\n\n");
        return;
    }

    char input[50];
    int material_choice;
    int quantity_to_unload;

    // Display material options
    printf("\nSelect material to unload:\n");
    printf("1. Large Box\n");
    printf("2. Medium Box\n");
    printf("3. Small Box\n");

    // Validate material choice
    while (1)
    {
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &material_choice) != 1 || material_choice < 1 || material_choice > 3)
        {
            printf("\n==========\nInvalid choice. Please try again.\n==========\n\n");
            continue;
        }
        break;
    }

    MaterialType *selected_material = NULL;
    if (material_choice == 1)
    {
        selected_material = &(MaterialType){"Large Box", 200.0, 0, 0};
    }
    else if (material_choice == 2)
    {
        selected_material = &(MaterialType){"Medium Box", 150.0, 0, 0};
    }
    else if (material_choice == 3)
    {
        selected_material = &(MaterialType){"Small Box", 100.0, 0, 0};
    }

    // Validate quantity to unload
    while (1)
    {
        printf("Enter the number of materials to unload: ");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &quantity_to_unload) != 1 || quantity_to_unload <= 0)
        {
            printf("\n==========\nInvalid quantity. Please enter a positive number.\n==========\n\n");
            continue;
        }
        break;
    }

    Wagon *current_wagon = train->first_wagon;

    // Move to the last wagon
    while (current_wagon->next != NULL)
    {
        current_wagon = current_wagon->next;
    }

    // Traverse wagons from tail to head
    while (current_wagon != NULL && quantity_to_unload > 0)
    {
        LoadedMaterial *current_material = current_wagon->loaded_materials;

        while (current_material != NULL && quantity_to_unload > 0)
        {
            if (strcmp(current_material->type->name, selected_material->name) == 0)
            {
                // Unload one material
                current_wagon->current_weight -= current_material->type->weight;
                quantity_to_unload--;

                // Remove the material node
                if (current_material->prev != NULL)
                {
                    current_material->prev->next = current_material->next;
                }
                else
                {
                    current_wagon->loaded_materials = current_material->next;
                }
                if (current_material->next != NULL)
                {
                    current_material->next->prev = current_material->prev;
                }

                LoadedMaterial *to_free = current_material;
                current_material = current_material->next;
                free(to_free);

                printf("\n==========\nUnloaded 1 %s from Wagon %d.\n==========\n\n", selected_material->name, current_wagon->wagon_id);
            }
            else
            {
                current_material = current_material->next;
            }
        }

        // Remove empty wagons
        if (current_wagon->loaded_materials == NULL && current_wagon->current_weight == 0)
        {
            Wagon *to_free = current_wagon;
            if (current_wagon->prev != NULL)
            {
                current_wagon->prev->next = current_wagon->next;
            }
            else
            {
                train->first_wagon = current_wagon->next;
            }
            if (current_wagon->next != NULL)
            {
                current_wagon->next->prev = current_wagon->prev;
            }
            current_wagon = current_wagon->prev;
            free(to_free);
            train->wagon_count--;
        }
        else
        {
            current_wagon = current_wagon->prev;
        }
    }

    if (quantity_to_unload > 0)
    {
        printf("\n==========\nNot enough materials to unload. %d remaining.\n==========\n\n", quantity_to_unload);
    }
    else
    {
        printf("\n==========\nUnloading completed.\n==========\n\n");
    }
    // Call the function to delete empty wagons and renumber
    delete_empty_wagons(train);
}

void empty_train_or_wagon(Train *train)
{
    if (train == NULL || train->first_wagon == NULL)
    {
        printf("\n==========\nTrain is already empty.\n==========\n\n");
        return;
    }

    printf("\nDo you want to empty a specific wagon or the entire train?\n");
    printf("1. Specific Wagon\n");
    printf("2. Entire Train\n");
    printf("Enter your choice: ");
    int choice;
    if (scanf("%d", &choice) != 1)
    {
        printf("\n==========\nInvalid input. Operation canceled.\n==========\n\n");
        clear_stdin();
        return;
    }
    clear_stdin(); // Clear buffer after reading choice

    if (choice == 1)
    {
        printf("Enter the Wagon ID to empty: ");
        int wagon_id;
        if (scanf("%d", &wagon_id) != 1)
        {
            printf("\n==========\nInvalid input. Operation canceled.\n==========\n\n");
            clear_stdin();
            return;
        }
        clear_stdin();

        // Find the wagon
        Wagon *current_wagon = train->first_wagon;
        while (current_wagon != NULL && current_wagon->wagon_id != wagon_id)
        {
            current_wagon = current_wagon->next;
        }

        if (current_wagon == NULL)
        {
            printf("\n==========\nError: Wagon ID %d does not exist.\n==========\n\n", wagon_id);
            return;
        }

        printf("\nAre you sure you want to empty Wagon %d? This action cannot be undone. (y/n): ", wagon_id);
        char confirm;
        if (scanf(" %c", &confirm) != 1)
        {
            printf("\n==========\nInvalid input. Operation canceled.\n==========\n\n");
            clear_stdin();
            return;
        }
        clear_stdin();

        if (confirm == 'y' || confirm == 'Y')
        {
            empty_specific_wagon(current_wagon);
        }
        else
        {
            printf("\n==========\nOperation canceled.\n==========\n\n");
        }
    }
    else if (choice == 2)
    {
        printf("\nAre you sure you want to delete all data? This action cannot be undone. (y/n): ");
        char confirm;
        if (scanf(" %c", &confirm) != 1)
        {
            printf("\n==========\nInvalid input. Operation canceled.\n==========\n\n");
            clear_stdin();
            return;
        }
        clear_stdin();

        if (confirm == 'y' || confirm == 'Y')
        {
            // Empty the entire train
            Wagon *current_wagon = train->first_wagon;

            while (current_wagon != NULL)
            {
                LoadedMaterial *current_material = current_wagon->loaded_materials;
                while (current_material != NULL)
                {
                    LoadedMaterial *to_free = current_material;
                    current_material = current_material->next;
                    free(to_free);
                }

                Wagon *to_free = current_wagon;
                current_wagon = current_wagon->next;
                free(to_free);
            }

            train->first_wagon = NULL;
            train->wagon_count = 0;

            printf("\n==========\nTrain has been emptied.\n==========\n\n");
        }
        else
        {
            printf("\n==========\nOperation canceled.\n==========\n\n");
        }
    }
    else
    {
        printf("\n==========\nInvalid choice. Operation canceled.\n==========\n\n");
    }

    delete_empty_wagons(train);
}
