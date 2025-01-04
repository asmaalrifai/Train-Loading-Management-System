//  display_train() in train.c to traverse the train and show wagon details.

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
            LoadedMaterial *current_material = current_wagon->loaded_materials;
            while (current_material != NULL)
            {
                printf("    - %s: %.2f kg\n",
                       current_material->type->name,
                       current_material->type->weight);
                current_material = current_material->next;
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
        // Find a suitable wagon or create a new one
        if (current_wagon == NULL)
        {
            // Create a new wagon if none exist or all are full
            Wagon *new_wagon = (Wagon *)malloc(sizeof(Wagon));
            new_wagon->wagon_id = train->wagon_count + 1;
            new_wagon->max_weight = 1000.0;
            new_wagon->current_weight = 0.0;
            new_wagon->loaded_materials = NULL;
            new_wagon->next = NULL;
            new_wagon->prev = NULL;

            if (train->first_wagon == NULL)
            {
                train->first_wagon = new_wagon; // Attach the first wagon
            }
            else
            {
                Wagon *last_wagon = train->first_wagon;
                while (last_wagon->next != NULL)
                {
                    last_wagon = last_wagon->next;
                }
                last_wagon->next = new_wagon; // Attach to the end
                new_wagon->prev = last_wagon;
            }

            current_wagon = new_wagon;
            train->wagon_count++;
        }

        // Calculate available space in the current wagon
        float available_space = current_wagon->max_weight - current_wagon->current_weight;
        int max_loadable = (int)(available_space / material->weight);

        if (max_loadable > 0)
        {
            int to_load = (remaining_quantity < max_loadable) ? remaining_quantity : max_loadable;

            // Update wagon and material properties
            current_wagon->current_weight += to_load * material->weight;
            remaining_quantity -= to_load;
            material->loaded += to_load;

            // Add to the loaded materials in the wagon
            LoadedMaterial *new_material = (LoadedMaterial *)malloc(sizeof(LoadedMaterial));
            new_material->type = material;
            new_material->next = current_wagon->loaded_materials;
            new_material->prev = NULL;

            if (current_wagon->loaded_materials != NULL)
            {
                current_wagon->loaded_materials->prev = new_material;
            }
            current_wagon->loaded_materials = new_material;

            printf("\nLoaded %d %s into Wagon %d.\n", to_load, material->name, current_wagon->wagon_id);
        }

        current_wagon = current_wagon->next;
    }
    printf("\n==========\nMaterial loading completed.\n==========\n\n");
}
