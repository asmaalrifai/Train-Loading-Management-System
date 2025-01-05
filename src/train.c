// train.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void load_material_to_wagon_main(Train *train, MaterialType *materials, int material_count)
{
    printf("Enter Wagon ID: ");
    int wagon_id;
    scanf("%d", &wagon_id);

    printf("Select material to load:\n");
    for (int i = 0; i < material_count; i++)
    {
        printf("%d. %s\n", i + 1, materials[i].name);
    }

    int material_choice;
    scanf("%d", &material_choice);

    if (material_choice >= 1 && material_choice <= material_count)
    {
        load_material_to_wagon(train, &materials[material_choice - 1], wagon_id);
    }
    else
    {
        printf("\nInvalid material choice.\n");
    }
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
    printf("Select material to load:\n");
    for (int i = 0; i < material_count; i++)
    {
        printf("%d. %s\n", i + 1, materials[i].name);
    }

    int material_choice;
    scanf("%d", &material_choice);

    if (material_choice >= 1 && material_choice <= material_count)
    {
        printf("Enter the number of materials to load: ");
        int quantity;
        scanf("%d", &quantity);

        load_specified_material_to_train(train, &materials[material_choice - 1], quantity);
    }
    else
    {
        printf("\nInvalid material choice.\n");
    }
}

// 4
void unload_material_from_tail(Train *train)
{
    if (train == NULL || train->first_wagon == NULL)
    {
        printf("\n==========\nError: Train or wagons are missing.\n==========\n\n");
        return;
    }

    int material_choice;
    int quantity_to_unload;

    // Display the material options
    printf("\nSelect material to unload:\n");
    printf("1. Large Box\n");
    printf("2. Medium Box\n");
    printf("3. Small Box\n");
    printf("Enter your choice: ");
    scanf("%d", &material_choice);

    // Map user input to the correct material
    MaterialType *selected_material = NULL;

    if (material_choice == 1)
    {
        selected_material = &(MaterialType){"Large Box", 200.0, 0, 0}; // Dummy type for comparison
    }
    else if (material_choice == 2)
    {
        selected_material = &(MaterialType){"Medium Box", 150.0, 0, 0};
    }
    else if (material_choice == 3)
    {
        selected_material = &(MaterialType){"Small Box", 100.0, 0, 0};
    }
    else
    {
        printf("\n==========\nInvalid choice. Unloading aborted.\n==========\n\n");
        return;
    }

    // Ask for the number of materials to unload
    printf("Enter the number of materials to unload: ");
    scanf("%d", &quantity_to_unload);

    if (quantity_to_unload <= 0)
    {
        printf("\n==========\nInvalid quantity. Unloading aborted.\n==========\n\n");
        return;
    }

    Wagon *current_wagon = train->first_wagon;

    // Traverse to the tail of the train
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
                // Unload one unit of the selected material
                current_wagon->current_weight -= current_material->type->weight;
                current_material->type->loaded -= 1;
                quantity_to_unload--;

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

                printf("\n==========\nUnloaded 1 %s from Wagon %d.\n==========\n\n", selected_material->name, current_wagon->wagon_id);
            }
            else
            {
                current_material = current_material->next;
            }
        }

        current_wagon = current_wagon->prev; // Move to the previous wagon
    }

    if (quantity_to_unload > 0)
    {
        printf("\n==========\nNot enough materials to unload. %d remaining.\n==========\n\n", quantity_to_unload);
    }
    else
    {
        printf("\n==========\nUnloading completed.\n==========\n\n");
    }
}

// 5
void unload_material_from_wagon(Train *train, MaterialType *materials, int material_count)
{
    if (train == NULL || train->first_wagon == NULL)
    {
        printf("\n==========\nError: Train or wagons are missing.\n==========\n\n");
        return;
    }

    // Ask the user for the wagon ID
    printf("Enter Wagon ID: ");
    int wagon_id;
    scanf("%d", &wagon_id);

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
    scanf("%d", &material_choice);

    if (material_choice < 1 || material_choice > material_count)
    {
        printf("\n==========\nInvalid material choice.\n==========\n\n");
        return;
    }

    MaterialType *selected_material = &materials[material_choice - 1];

    // Ask the user for the quantity to unload
    printf("Enter the number of materials to unload: ");
    int quantity_to_unload;
    scanf("%d", &quantity_to_unload);

    if (quantity_to_unload <= 0)
    {
        printf("\n==========\nInvalid quantity. Unloading aborted.\n==========\n\n");
        return;
    }

    // Traverse the LoadedMaterial list in the specified wagon
    LoadedMaterial *current_material = current_wagon->loaded_materials;

    while (current_material != NULL && quantity_to_unload > 0)
    {
        if (strcmp(current_material->type->name, selected_material->name) == 0)
        {
            // Unload one unit of the selected material
            current_wagon->current_weight -= current_material->type->weight;
            current_material->type->loaded -= 1;
            quantity_to_unload--;

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

            printf("\n==========\nUnloaded 1 %s from Wagon %d.\n==========\n\n", selected_material->name, wagon_id);
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
        printf("\n==========\nUnloading completed.\n==========\n\n");
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

void empty_train_or_wagon(Train *train)
{
    if (train == NULL || train->first_wagon == NULL)
    {
        printf("\n==========\nTrain is already empty.\n==========\n\n");
        return;
    }

    // Ask the user if they want to empty a specific wagon or the entire train
    printf("\nDo you want to empty a specific wagon or the entire train?\n");
    printf("1. Specific Wagon\n");
    printf("2. Entire Train\n");
    printf("Enter your choice: ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1)
    {
        // Ask for the wagon ID
        printf("Enter the Wagon ID to empty: ");
        int wagon_id;
        scanf("%d", &wagon_id);

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

        // Confirm with the user
        printf("\nAre you sure you want to empty Wagon %d? This action cannot be undone. (y/n): ", wagon_id);
        char confirm;
        scanf(" %c", &confirm);

        if (confirm == 'y' || confirm == 'Y')
        {
            // Empty the specific wagon
            empty_specific_wagon(current_wagon);
        }
        else
        {
            printf("\n==========\nOperation canceled.\n==========\n\n");
        }
    }
    else if (choice == 2)
    {
        // Confirm with the user
        printf("\nAre you sure you want to delete all data? This action cannot be undone. (y/n): ");
        char confirm;
        scanf(" %c", &confirm);

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
