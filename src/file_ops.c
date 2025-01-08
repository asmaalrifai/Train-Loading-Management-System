//file_ops.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/wagon.h"
#include "../include/train.h"
#include "../include/material.h"
#include "../include/file_ops.h"
#include "../include/utils.h"




// 1
void load_train_status_from_file(Train *train, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("\n==========\nError: Unable to open file %s for reading.\n==========\n\n", filename);
        return;
    }

    // Empty the train before loading new data
    if (train->first_wagon != NULL)
    {
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
    }

    char line[256];
    Wagon *last_wagon = NULL;

    while (fgets(line, sizeof(line), file))
    {
        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "Train ID:", 9) == 0)
        {
            sscanf(line, "Train ID: %s", train->train_id);
        }
        else if (strncmp(line, "Total Wagons:", 13) == 0)
        {
            int total_wagons;
            sscanf(line, "Total Wagons: %d", &total_wagons);
            train->wagon_count = total_wagons;
        }
        else if (strncmp(line, "Wagon ID:", 9) == 0)
        {
            // Allocate a new wagon
            Wagon *new_wagon = (Wagon *)malloc(sizeof(Wagon));
            if (!new_wagon)
            {
                printf("\n==========\nError: Memory allocation failed for wagon.\n==========\n\n");
                fclose(file);
                return;
            }
            sscanf(line, "Wagon ID: %d", &new_wagon->wagon_id);
            new_wagon->next = NULL;
            new_wagon->prev = last_wagon;
            new_wagon->loaded_materials = NULL;
            if (last_wagon != NULL)
            {
                last_wagon->next = new_wagon;
            }
            else
            {
                train->first_wagon = new_wagon;
            }
            last_wagon = new_wagon;
        }
        else if (strncmp(line, "  Max Weight:", 13) == 0)
        {
            sscanf(line, "  Max Weight: %f kg", &last_wagon->max_weight);
        }
        else if (strncmp(line, "  Current Weight:", 17) == 0)
        {
            sscanf(line, "  Current Weight: %f kg", &last_wagon->current_weight);
        }
        else if (strncmp(line, "    -", 5) == 0)
        {
            // Allocate a new material
            LoadedMaterial *new_material = (LoadedMaterial *)malloc(sizeof(LoadedMaterial));
            if (!new_material)
            {
                printf("\n==========\nError: Memory allocation failed for material.\n==========\n\n");
                fclose(file);
                return;
            }
            new_material->next = NULL;
            new_material->prev = NULL;

            char material_name[50];
            float material_weight;
            sscanf(line, "    - %49[^:]: %f kg", material_name, &material_weight);

            // Create a MaterialType for this material
            MaterialType *material_type = (MaterialType *)malloc(sizeof(MaterialType));
            if (!material_type)
            {
                printf("\n==========\nError: Memory allocation failed for material type.\n==========\n\n");
                free(new_material);
                fclose(file);
                return;
            }
            strcpy(material_type->name, material_name);
            material_type->weight = material_weight;

            new_material->type = material_type;

            // Insert material into the wagon
            if (last_wagon->loaded_materials == NULL)
            {
                last_wagon->loaded_materials = new_material;
            }
            else
            {
                LoadedMaterial *last_material = last_wagon->loaded_materials;
                while (last_material->next != NULL)
                {
                    last_material = last_material->next;
                }
                last_material->next = new_material;
                new_material->prev = last_material;
            }
        }
    }

    fclose(file);
    printf("\n==========\nTrain status loaded from file: %s\n==========\n\n", filename);
}

// 9
void save_train_status_to_file(Train *train, const char *filename)
{
    if (train == NULL)
    {
        printf("\n==========\nError: Train is missing. Nothing to save.\n==========\n\n");
        return;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("\n==========\nError: Unable to open file %s for writing.\n==========\n\n", filename);
        return;
    }

    // Write train ID
    fprintf(file, "Train ID: %s\n", train->train_id);

    // Check if the train is empty
    if (train->first_wagon == NULL)
    {
        fprintf(file, "Total Wagons: 0\n");
        fprintf(file, "The train is empty.\n");
        fclose(file);
        printf("\n==========\nTrain status saved to file: %s\n==========\n\n", filename);
        return;
    }

    // Write total wagons
    fprintf(file, "Total Wagons: %d\n", train->wagon_count);

    // Traverse wagons
    Wagon *current_wagon = train->first_wagon;
    while (current_wagon != NULL)
    {
        fprintf(file, "\nWagon ID: %d\n", current_wagon->wagon_id);
        fprintf(file, "  Max Weight: %.2f kg\n", current_wagon->max_weight);
        fprintf(file, "  Current Weight: %.2f kg\n", current_wagon->current_weight);

        if (current_wagon->loaded_materials == NULL)
        {
            fprintf(file, "  No materials loaded.\n");
        }
        else
        {
            fprintf(file, "  Loaded Materials:\n");

            // Traverse materials
            LoadedMaterial *current_material = current_wagon->loaded_materials;
            while (current_material != NULL)
            {
                fprintf(file, "    - %s: %.2f kg\n",
                        current_material->type->name,
                        current_material->type->weight);
                current_material = current_material->next;
            }
        }

        current_wagon = current_wagon->next;
    }

    fclose(file);
    printf("\n==========\nTrain status saved to file: %s\n==========\n\n", filename);
}