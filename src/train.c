// train.c
#include <stdio.h>
#include <stdlib.h>

#include "../include/train.h"

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

static Wagon *create_new_wagon(Train *train)
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

static void insert_material_into_wagon(Wagon *wagon, MaterialType *material)
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

static int check_material_availability(MaterialType *material, int quantity)
{
    return (quantity > 0 && quantity <= (material->quantity - material->loaded));
}

static int check_wagon_space(Wagon *wagon, MaterialType *material)
{
    return (wagon->max_weight - wagon->current_weight >= material->weight);
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

void load_material_to_wagon(Train *train, MaterialType *material, int wagon_id)
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
        printf("\n==========\nError: Wagon ID %d does not exist.\n==========\n\n", wagon_id);
        return;
    }

    if (!check_material_availability(material, 1))
    {
        printf("\n==========\nNo more %s available to load.\n==========\n\n", material->name);
        return;
    }

    if (!check_wagon_space(current_wagon, material))
    {
        printf("\n==========\nError: No space left in Wagon %d to load %s.\n==========\n\n", wagon_id, material->name);
        return;
    }

    insert_material_into_wagon(current_wagon, material);
    current_wagon->current_weight += material->weight;
    material->loaded += 1;

    printf("\nLoaded 1 %s into Wagon %d.\n", material->name, wagon_id);
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
