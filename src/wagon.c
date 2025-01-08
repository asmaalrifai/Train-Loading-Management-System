//wagon.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/wagon.h"
#include "../include/train.h"
#include "../include/material.h"
#include "../include/file_ops.h"
#include "../include/utils.h"



struct Wagon *create_new_wagon(struct Train *train)

{
    Wagon *new_wagon = (Wagon *)malloc(sizeof(Wagon));
    if (!new_wagon)
    {
        printf("\n==========\nError: Memory allocation failed for new wagon.\n==========\n\n");
        exit(1);
    }
    new_wagon->wagon_id = train->wagon_count + 1;
    new_wagon->max_weight = 1000.0;
    new_wagon->current_weight = 0.0;
    new_wagon->loaded_materials = NULL;
    new_wagon->next = NULL;
    new_wagon->prev = NULL;

    if (train->first_wagon == NULL)
    {
        train->first_wagon = new_wagon;
    }
    else
    {
        Wagon *last_wagon = train->first_wagon;
        while (last_wagon->next != NULL)
        {
            last_wagon = last_wagon->next;
        }
        last_wagon->next = new_wagon;
        new_wagon->prev = last_wagon;
    }

    train->wagon_count++;
    return new_wagon;
}

void insert_material_into_wagon(Wagon *wagon, MaterialType *material)
{
    LoadedMaterial *new_material = (LoadedMaterial *)malloc(sizeof(LoadedMaterial));
    if (!new_material)
    {
        printf("\n==========\nError: Memory allocation failed for new material.\n==========\n\n");
        exit(1);
    }
    new_material->type = material;
    new_material->next = NULL;
    new_material->prev = NULL;

    if (wagon->loaded_materials == NULL)
    {
        wagon->loaded_materials = new_material;
    }
    else
    {
        LoadedMaterial *current = wagon->loaded_materials;

        // Traverse to find the correct position
        while (current != NULL && current->type->weight <= material->weight)
        {
            current = current->next;
        }

        if (current == NULL)
        {
            // Append at the end
            LoadedMaterial *last = wagon->loaded_materials;
            while (last->next != NULL)
            {
                last = last->next;
            }
            last->next = new_material;
            new_material->prev = last;
        }
        else
        {
            // Insert before current
            new_material->next = current;
            new_material->prev = current->prev;

            if (current->prev != NULL)
            {
                current->prev->next = new_material;
            }
            else
            {
                wagon->loaded_materials = new_material;
            }
            current->prev = new_material;
        }
    }
}

// 8
void empty_specific_wagon(Wagon *wagon)
{
    if (wagon == NULL)
    {
        printf("\n==========\nError: Wagon is missing.\n==========\n\n");
        return;
    }

    // Free all materials in the wagon
    LoadedMaterial *current_material = wagon->loaded_materials;
    while (current_material != NULL)
    {
        LoadedMaterial *to_free = current_material;
        current_material = current_material->next;
        free(to_free);
    }

    wagon->loaded_materials = NULL;
    wagon->current_weight = 0;

    printf("\n==========\nWagon %d has been emptied.\n==========\n\n", wagon->wagon_id);
}

void load_material_to_wagon(Train *train, MaterialType *material, int wagon_id, int quantity)
{
    if (train == NULL || material == NULL)
    {
        printf("\n==========\nError: Train or material data is missing.\n==========\n\n");
        return;
    }

    Wagon *current_wagon = train->first_wagon;

    // Find the specified wagon by ID
    while (current_wagon != NULL && current_wagon->wagon_id != wagon_id)
    {
        current_wagon = current_wagon->next;
    }

    if (current_wagon == NULL)
    {
        printf("\n==========\nError: Wagon ID %d does not exist. Starting from the head of the train.\n==========\n\n", wagon_id);
        current_wagon = train->first_wagon;
    }

    int remaining_quantity = quantity;
    if (remaining_quantity > (material->quantity - material->loaded))
    {
        printf("\n==========\nNot enough %s available to load. Requested: %d, Available: %d\n==========\n\n",
               material->name, quantity, material->quantity - material->loaded);
        return;
    }

    // Load materials into the wagons, handling overflow
    while (remaining_quantity > 0)
    {
        // Create a new wagon if needed
        if (current_wagon == NULL)
        {
            current_wagon = create_new_wagon(train);
        }

        // Check available space in the current wagon
        float available_space = current_wagon->max_weight - current_wagon->current_weight;
        int max_loadable = (int)(available_space / material->weight);

        if (max_loadable > 0)
        {
            int to_load = (remaining_quantity < max_loadable) ? remaining_quantity : max_loadable;

            for (int i = 0; i < to_load; i++)
            {
                insert_material_into_wagon(current_wagon, material);
                current_wagon->current_weight += material->weight;
                material->loaded += 1;
                remaining_quantity -= 1;
            }

            printf("\n==========\nLoaded %d %s into Wagon %d.\n==========\n\n", to_load, material->name, current_wagon->wagon_id);
        }

        // Move to the next wagon
        if (remaining_quantity > 0)
        {
            if (current_wagon->next == NULL)
            {
                current_wagon->next = create_new_wagon(train);
                current_wagon->next->prev = current_wagon;
            }
            current_wagon = current_wagon->next;
        }
    }

    printf("\n==========\nMaterial loading completed.\n==========\n\n");
}

void load_material_to_wagon_main(Train *train, MaterialType *materials, int material_count)
{
    char input[50];
    int wagon_id;

    // Get and validate the Wagon ID
    while (1)
    {
        printf("Enter Wagon ID: ");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &wagon_id) != 1)
        {
            printf("\n==========\nInvalid Wagon ID. Please enter a number.\n==========\n\n");
            continue;
        }
        break;
    }

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

    // Select the material to load
    printf("Select material to load:\n");
    for (int i = 0; i < material_count; i++)
    {
        printf("%d. %s\n", i + 1, materials[i].name);
    }

    int material_choice;
    while (1)
    {
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &material_choice) != 1 || material_choice < 1 || material_choice > material_count)
        {
            printf("\n==========\nInvalid material choice. Please try again.\n==========\n\n");
            continue;
        }
        break;
    }

    MaterialType *selected_material = &materials[material_choice - 1];

    // Get the quantity to load
    int quantity;
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

    load_material_to_wagon(train, selected_material, wagon_id, quantity);
}

// 5
void unload_material_from_wagon_main(Train *train, MaterialType *materials, int material_count)
{
    if (train == NULL || train->first_wagon == NULL)
    {
        printf("\n==========\nError: Train or wagons are missing.\n==========\n\n");
        return;
    }

    char input[50];
    int wagon_id;

    // Get and validate the Wagon ID
    while (1)
    {
        printf("Enter Wagon ID: ");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &wagon_id) != 1 || wagon_id <= 0)
        {
            printf("\n==========\nInvalid Wagon ID. Please enter a valid number.\n==========\n\n");
            continue;
        }
        break;
    }

    // Find the specified wagon
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

    // Check if the wagon has materials
    if (current_wagon->loaded_materials == NULL)
    {
        printf("\n==========\nNo materials loaded in Wagon %d.\n==========\n\n", wagon_id);
        return;
    }

    // Ask the user for the material type
    printf("Select material to unload:\n");
    for (int i = 0; i < material_count; i++)
    {
        printf("%d. %s\n", i + 1, materials[i].name);
    }

    int material_choice;
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

    MaterialType *selected_material = &materials[material_choice - 1];

    // Ask the user for the quantity to unload
    int quantity_to_unload;
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

    // Traverse the LoadedMaterial list in the specified wagon
    LoadedMaterial *current_material = current_wagon->loaded_materials;
    int unloaded_count = 0;

    while (current_material != NULL && quantity_to_unload > 0)
    {
        if (strcmp(current_material->type->name, selected_material->name) == 0)
        {
            // Unload one unit of the selected material
            current_wagon->current_weight -= current_material->type->weight;
            current_material->type->loaded -= 1;
            quantity_to_unload--;
            unloaded_count++;

            // Remove the material from the list if needed
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
        }
        else
        {
            current_material = current_material->next;
        }
    }

    // Check if we couldn't unload all requested materials
    if (quantity_to_unload > 0)
    {
        printf("\n==========\nNot enough materials to unload. %d remaining.\n==========\n\n", quantity_to_unload);
    }
    else
    {
        printf("\n==========\nUnloaded %d %s from Wagon %d.\n==========\n\n", unloaded_count, selected_material->name, wagon_id);
    }
    // Call the function to delete empty wagons and renumber
    delete_empty_wagons(train);
}

void delete_empty_wagons(Train *train)
{
    if (train == NULL || train->first_wagon == NULL)
    {
        printf("\n==========\nError: Train or wagons are missing.\n==========\n\n");
        return;
    }

    Wagon *current_wagon = train->first_wagon;
    Wagon *previous_wagon = NULL;
    int new_wagon_id = 1; // Start renumbering from 1

    while (current_wagon != NULL)
    {
        // Check if the wagon is empty
        if (current_wagon->loaded_materials == NULL && current_wagon->current_weight == 0)
        {
            // Remove the empty wagon
            Wagon *to_free = current_wagon;

            if (previous_wagon == NULL)
            {
                // If it's the first wagon, update the train's head
                train->first_wagon = current_wagon->next;
            }
            else
            {
                // Otherwise, connect the previous wagon to the next wagon
                previous_wagon->next = current_wagon->next;
            }

            if (current_wagon->next != NULL)
            {
                current_wagon->next->prev = previous_wagon;
            }

            current_wagon = current_wagon->next;
            free(to_free);
            train->wagon_count--;
        }
        else
        {
            // If the wagon is not empty, renumber it
            current_wagon->wagon_id = new_wagon_id++;
            previous_wagon = current_wagon;
            current_wagon = current_wagon->next;
        }
    }

    printf("\n==========\nEmpty wagons deleted and remaining wagons renumbered.\n==========\n\n");
}