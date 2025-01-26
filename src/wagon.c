#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/wagon.h"
#include "../include/train.h"
#include "../include/material.h"
#include "../include/utils.h"

// Create a new wagon
Wagon *create_new_wagon(Train *train)
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

    if (!train->first_wagon)
    {
        train->first_wagon = new_wagon;
    }
    else
    {
        Wagon *last_wagon = train->first_wagon;
        while (last_wagon->next)
        {
            last_wagon = last_wagon->next;
        }
        last_wagon->next = new_wagon;
        new_wagon->prev = last_wagon;
    }

    train->wagon_count++;
    return new_wagon;
}

// Insert material into the wagon (small on top then medium then large)
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

    if (!wagon->loaded_materials)
    {
        wagon->loaded_materials = new_material;
    }
    else
    {
        LoadedMaterial *current = wagon->loaded_materials;
        while (current && current->type->weight <= material->weight)
        {
            current = current->next;
        }

        if (!current)
        {
            LoadedMaterial *last = wagon->loaded_materials;
            while (last->next)
            {
                last = last->next;
            }
            last->next = new_material;
            new_material->prev = last;
        }
        else
        {
            new_material->next = current;
            new_material->prev = current->prev;
            if (current->prev)
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

// Empty a specific wagon
void empty_specific_wagon(Wagon *wagon)
{
    if (!wagon)
    {
        printf("\n==========\nError: Wagon is missing.\n==========\n\n");
        return;
    }

    LoadedMaterial *current_material = wagon->loaded_materials;
    while (current_material)
    {
        LoadedMaterial *to_free = current_material;
        current_material = current_material->next;
        free(to_free);
    }

    wagon->loaded_materials = NULL;
    wagon->current_weight = 0;

    printf("\n==========\nWagon %d has been emptied.\n==========\n\n", wagon->wagon_id);
}

void display_wagon_status(Wagon *wagon)
{
    if (!wagon)
        return;
    printf("Wagon ID: %d\n", wagon->wagon_id);
    printf("  Max Weight: %.2f kg\n", wagon->max_weight);
    printf("  Current Weight: %.2f kg\n", wagon->current_weight);
    if (!wagon->loaded_materials)
    {
        printf("  No materials loaded.\n");
    }
    else
    {
        printf("  Loaded Materials:\n");
        LoadedMaterial *current = wagon->loaded_materials;
        while (current)
        {
            printf("    - %s: %.2f kg\n", current->type->name, current->type->weight);
            current = current->next;
        }
    }
}

void load_material_to_wagon_main(Train *train, MaterialType *materials, int material_count)
{
    char input[50];
    int wagon_id;

    printf("Enter Wagon ID: ");
    fgets(input, sizeof(input), stdin);

    if (sscanf(input, "%d", &wagon_id) != 1)
    {
        printf("\nInvalid Wagon ID. enter a number.\n");
        return;
    }

    Wagon *current_wagon = train->first_wagon;
    while (current_wagon && current_wagon->wagon_id != wagon_id)
    {
        current_wagon = current_wagon->next;
    }

    if (!current_wagon)
    {
        printf("\nError: Wagon ID %d does not exist.\n", wagon_id);
        return;
    }

    printf("Select material to load:\n");
    for (int i = 0; i < material_count; i++)
    {
        printf("%d. %s\n", i + 1, materials[i].name);
    }

    int material_choice, quantity;
    printf("Enter material choice: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%d", &material_choice);

    printf("Enter quantity: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%d", &quantity);

    load_material_to_wagon(train, &materials[material_choice - 1], wagon_id, quantity);
}

void unload_material_from_wagon_main(Train *train, MaterialType *materials, int material_count)
{
    char input[50];
    int wagon_id;

    printf("Enter Wagon ID: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%d", &wagon_id);

    Wagon *current_wagon = train->first_wagon;
    while (current_wagon && current_wagon->wagon_id != wagon_id)
    {
        current_wagon = current_wagon->next;
    }

    if (!current_wagon)
    {
        printf("\nError: Wagon ID %d does not exist.\n", wagon_id);
        return;
    }

    printf("Select material to unload:\n");
    for (int i = 0; i < material_count; i++)
    {
        printf("%d. %s\n", i + 1, materials[i].name);
    }

    int material_choice, quantity;
    printf("Enter material choice: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%d", &material_choice);

    printf("Enter quantity: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%d", &quantity);

    unload_material_from_wagon(current_wagon, &materials[material_choice - 1], quantity);
    delete_empty_wagons(train);
}

void load_material_to_wagon(Train *train, MaterialType *material, int wagon_id, int quantity)
{
    if (!train || !material)
    {
        printf("\nError: Train or material data is missing.\n");
        return;
    }

    Wagon *current_wagon = train->first_wagon;

    // Find the wagon by its ID
    while (current_wagon && current_wagon->wagon_id != wagon_id)
    {
        current_wagon = current_wagon->next;
    }

    if (!current_wagon)
    {
        printf("\nError: Wagon ID %d does not exist.\n", wagon_id);
        return;
    }

    int remaining_quantity = quantity;
    while (remaining_quantity > 0)
    {
        // Calculate available space in the wagon
        float available_space = current_wagon->max_weight - current_wagon->current_weight;

        // Check if the wagon can accommodate more materials
        if (available_space < material->weight)
        {
            printf("\nError: Wagon %d is full or cannot accommodate more %s materials.\n", wagon_id, material->name);
            break;
        }

        // Calculate how many materials can be loaded
        int max_loadable = (int)(available_space / material->weight);

        // Load as many materials as possible, up to the requested quantity
        int to_load = (remaining_quantity < max_loadable) ? remaining_quantity : max_loadable;

        for (int i = 0; i < to_load; i++)
        {
            insert_material_into_wagon(current_wagon, material);
            current_wagon->current_weight += material->weight;
            material->loaded++;
            remaining_quantity--;
        }

        printf("\nLoaded %d %s into Wagon %d.\n", to_load, material->name, wagon_id);

        // Check if there's remaining quantity but the wagon cannot accommodate more
        if (remaining_quantity > 0 && (current_wagon->max_weight - current_wagon->current_weight) < material->weight)
        {
            printf("\nWagon %d is full. Cannot load remaining %d materials.\n", wagon_id, remaining_quantity);
            break;
        }
    }

    if (remaining_quantity > 0)
    {
        printf("\nCould not load %d materials due to insufficient space in Wagon %d.\n", remaining_quantity, wagon_id);
    }
    else
    {
        printf("\nMaterial loading completed for Wagon %d.\n", wagon_id);
    }
}

void unload_material_from_wagon(Wagon *wagon, MaterialType *material, int quantity)
{
    if (!wagon || !material)
    {
        printf("\nError: Wagon or material data is missing.\n");
        return;
    }

    LoadedMaterial *current_material = wagon->loaded_materials;
    int unloaded_count = 0;

    while (current_material && quantity > 0)
    {
        if (strcmp(current_material->type->name, material->name) == 0)
        {
            wagon->current_weight -= material->weight;
            material->loaded--;
            quantity--;
            unloaded_count++;

            if (current_material->prev)
            {
                current_material->prev->next = current_material->next;
            }
            else
            {
                wagon->loaded_materials = current_material->next;
            }
            if (current_material->next)
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

    printf("\nUnloaded %d %s from Wagon %d.\n", unloaded_count, material->name, wagon->wagon_id);
}

void delete_empty_wagons(Train *train)
{
    if (!train || !train->first_wagon)
    {
        printf("\n==========\nTrain or wagons are missing.\n==========\n\n");
        return;
    }

    Wagon *current_wagon = train->first_wagon;
    Wagon *previous_wagon = NULL;
    int new_wagon_id = 1; // Start renumbering from 1

    while (current_wagon != NULL)
    {
        // Check if the wagon is empty
        if (current_wagon->current_weight == 0 && current_wagon->loaded_materials == NULL)
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
