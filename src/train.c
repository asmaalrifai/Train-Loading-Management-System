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
        // Create a new wagon if all existing wagons are full
        if (current_wagon == NULL)
        {
            Wagon *new_wagon = (Wagon *)malloc(sizeof(Wagon));
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

            current_wagon = new_wagon;
            train->wagon_count++;
        }

        // Calculate available space
        float available_space = current_wagon->max_weight - current_wagon->current_weight;
        int max_loadable = (int)(available_space / material->weight);

        if (max_loadable > 0)
        {
            int to_load = (remaining_quantity < max_loadable) ? remaining_quantity : max_loadable;

            // Create a new node for each unit of material loaded
            for (int i = 0; i < to_load; i++)
            {
                LoadedMaterial *new_material = (LoadedMaterial *)malloc(sizeof(LoadedMaterial));
                new_material->type = material;
                new_material->next = NULL;
                new_material->prev = NULL;

                // Insert into the wagon's loaded_materials list based on weight
                if (current_wagon->loaded_materials == NULL)
                {
                    current_wagon->loaded_materials = new_material;
                }
                else
                {
                    LoadedMaterial *current = current_wagon->loaded_materials;

                    while (current != NULL && current->type->weight <= material->weight)
                    {
                        current = current->next;
                    }

                    if (current == NULL)
                    {
                        LoadedMaterial *last = current_wagon->loaded_materials;
                        while (last->next != NULL)
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

                        if (current->prev != NULL)
                        {
                            current->prev->next = new_material;
                        }
                        else
                        {
                            current_wagon->loaded_materials = new_material;
                        }
                        current->prev = new_material;
                    }
                }

                current_wagon->current_weight += material->weight;
                material->loaded += 1;
                remaining_quantity -= 1;
            }
        }

        // Move to the next wagon if needed
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

    int remaining_quantity = material->quantity - material->loaded;
    if (remaining_quantity <= 0)
    {
        printf("\n==========\nNo more %s available to load.\n==========\n\n", material->name);
        return;
    }

    float available_space = current_wagon->max_weight - current_wagon->current_weight;
    if (available_space < material->weight)
    {
        printf("\n==========\nError: No space left in Wagon %d to load %s.\n==========\n\n", wagon_id, material->name);
        return;
    }

    // Create a new LoadedMaterial node
    LoadedMaterial *new_material = (LoadedMaterial *)malloc(sizeof(LoadedMaterial));
    new_material->type = material;
    new_material->next = NULL;
    new_material->prev = NULL;

    // Insert into the wagon's loaded_materials list based on weight
    if (current_wagon->loaded_materials == NULL)
    {
        // If the list is empty, add as the first node
        current_wagon->loaded_materials = new_material;
    }
    else
    {
        LoadedMaterial *current = current_wagon->loaded_materials;

        // Traverse the list to find the correct position based on weight
        while (current != NULL && current->type->weight <= material->weight)
        {
            current = current->next;
        }

        if (current == NULL)
        {
            // Append at the end of the list
            LoadedMaterial *last = current_wagon->loaded_materials;
            while (last->next != NULL)
            {
                last = last->next;
            }
            last->next = new_material;
            new_material->prev = last;
        }
        else
        {
            // Insert before the current node
            new_material->next = current;
            new_material->prev = current->prev;

            if (current->prev != NULL)
            {
                current->prev->next = new_material;
            }
            else
            {
                // If inserting at the beginning, update the head
                current_wagon->loaded_materials = new_material;
            }
            current->prev = new_material;
        }
    }

    // Update weights and quantities
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

    if (quantity <= 0 || quantity > (material->quantity - material->loaded))
    {
        printf("\n==========\nInvalid quantity. Available quantity: %d\n==========\n\n", material->quantity - material->loaded);
        return;
    }

    int remaining_quantity = quantity; // User-specified quantity
    Wagon *current_wagon = train->first_wagon;

    while (remaining_quantity > 0)
    {
        // Create a new wagon if all existing wagons are full
        if (current_wagon == NULL)
        {
            Wagon *new_wagon = (Wagon *)malloc(sizeof(Wagon));
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

            current_wagon = new_wagon;
            train->wagon_count++;
        }

        // Check available space in the current wagon
        float available_space = current_wagon->max_weight - current_wagon->current_weight;
        if (available_space < material->weight)
        {
            current_wagon = current_wagon->next;
            continue;
        }

        while (remaining_quantity > 0 && available_space >= material->weight)
        {
            // Create a new LoadedMaterial node
            LoadedMaterial *new_material = (LoadedMaterial *)malloc(sizeof(LoadedMaterial));
            new_material->type = material;
            new_material->next = NULL;
            new_material->prev = NULL;

            // Insert into the wagon's loaded_materials list based on weight
            if (current_wagon->loaded_materials == NULL)
            {
                current_wagon->loaded_materials = new_material;
            }
            else
            {
                LoadedMaterial *current = current_wagon->loaded_materials;

                while (current != NULL && current->type->weight <= material->weight)
                {
                    current = current->next;
                }

                if (current == NULL)
                {
                    LoadedMaterial *last = current_wagon->loaded_materials;
                    while (last->next != NULL)
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

                    if (current->prev != NULL)
                    {
                        current->prev->next = new_material;
                    }
                    else
                    {
                        current_wagon->loaded_materials = new_material;
                    }
                    current->prev = new_material;
                }
            }

            current_wagon->current_weight += material->weight;
            material->loaded += 1;
            remaining_quantity -= 1;
            available_space -= material->weight;

            printf("\nLoaded 1 %s into Wagon %d.\n", material->name, current_wagon->wagon_id);
        }

        current_wagon = current_wagon->next;
    }

    printf("\n==========\nMaterial loading completed.\n==========\n\n");
}
