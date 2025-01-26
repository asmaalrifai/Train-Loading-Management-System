#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/train.h"
#include "../include/wagon.h"
#include "../include/material.h"
#include "../include/file_ops.h"
#include "../include/utils.h"

// Create a new train
Train *create_train() {
    Train *train = (Train *)malloc(sizeof(Train));
    if (!train) {
        printf("\nError: Memory allocation failed for Train.\n");
        exit(1);
    }
    strcpy(train->train_id, "FasterThanLight");
    train->first_wagon = NULL;
    train->wagon_count = 0;
    return train;
}

// Display the train's status
void display_train_status(Train *train) {
    if (!train || !train->first_wagon) {
        printf("\n==========\nNo wagons in the train.\n==========\n\n");
        return;
    }

    printf("\n==========\nTrain ID: %s\nTotal Wagons: %d\n==========\n", train->train_id, train->wagon_count);

    Wagon *current_wagon = train->first_wagon;
    while (current_wagon) {
        display_wagon_status(current_wagon);
        current_wagon = current_wagon->next;
    }
}

// Load materials into the train
void load_material_to_train(Train *train, MaterialType *material) {
    if (!train || !material) {
        printf("\n==========\nError: Train or material data is missing.\n==========\n\n");
        return;
    }

    int remaining_quantity = material->quantity - material->loaded;
    if (remaining_quantity <= 0) {
        printf("\n==========\nNo more %s available to load.\n==========\n\n", material->name);
        return;
    }

    Wagon *current_wagon = train->first_wagon;
    while (remaining_quantity > 0) {
        if (!current_wagon) {
            current_wagon = create_new_wagon(train);
        }

        while (remaining_quantity > 0 && check_wagon_space(current_wagon, material)) {
            insert_material_into_wagon(current_wagon, material);
            current_wagon->current_weight += material->weight;
            material->loaded++;
            remaining_quantity--;
        }

        current_wagon = current_wagon->next;
    }

    printf("\n==========\nMaterial loading completed.\n==========\n\n");
}

// Load specified quantity of material into the train
void load_specified_material_to_train(Train *train, MaterialType *material, int quantity) {
    if (!train || !material) {
        printf("\n==========\nError: Train or material data is missing.\n==========\n\n");
        return;
    }

    if (!check_material_availability(material, quantity)) {
        printf("\n==========\nInvalid quantity. Available quantity: %d\n==========\n\n", material->quantity - material->loaded);
        return;
    }

    int remaining_quantity = quantity;
    Wagon *current_wagon = train->first_wagon;

    while (remaining_quantity > 0) {
        if (!current_wagon) {
            current_wagon = create_new_wagon(train);
        }

        while (remaining_quantity > 0 && check_wagon_space(current_wagon, material)) {
            insert_material_into_wagon(current_wagon, material);
            current_wagon->current_weight += material->weight;
            material->loaded++;
            remaining_quantity--;
        }

        current_wagon = current_wagon->next;
    }

    printf("\n==========\nMaterial loading completed.\n==========\n\n");
}


void unload_material_from_tail(Train *train, MaterialType *materials, int material_count) {
    if (!train || !train->first_wagon) {
        printf("\n==========\nError: Train or wagons are missing.\n==========\n\n");
        return;
    }

    char input[50];
    int material_choice, quantity_to_unload;

    // Ask the user to select the material type
    printf("\nSelect material to unload:\n");
    for (int i = 0; i < material_count; i++) {
        printf("%d. %s\n", i + 1, materials[i].name);
    }

    while (1) {
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &material_choice) != 1 || material_choice < 1 || material_choice > material_count) {
            printf("\n==========\nInvalid material choice. \n==========\n\n");
            continue;
        }
        break;
    }

    MaterialType *selected_material = &materials[material_choice - 1];

    // Ask the user to enter the quantity to unload
    while (1) {
        printf("Enter the amount of %s to unload: ", selected_material->name);
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &quantity_to_unload) != 1 || quantity_to_unload <= 0) {
            printf("\n==========\nInvalid quantity. \n==========\n\n");
            continue;
        }
        break;
    }

    int remaining_quantity = quantity_to_unload;
    Wagon *current_wagon = train->first_wagon;

    // Move to the last wagon
    while (current_wagon->next) {
        current_wagon = current_wagon->next;
    }

    // Start unloading from the tail
    while (current_wagon && remaining_quantity > 0) {
        LoadedMaterial *current_material = current_wagon->loaded_materials;

        while (current_material && remaining_quantity > 0) {
            if (strcmp(current_material->type->name, selected_material->name) == 0) {
                // Unload one unit of material
                current_wagon->current_weight -= current_material->type->weight;
                current_material->type->loaded--;
                remaining_quantity--;

                // Remove the material node if needed
                if (current_material->prev) {
                    current_material->prev->next = current_material->next;
                } else {
                    current_wagon->loaded_materials = current_material->next;
                }

                if (current_material->next) {
                    current_material->next->prev = current_material->prev;
                }

                LoadedMaterial *to_free = current_material;
                current_material = current_material->next;
                free(to_free);

                printf("\nUnloaded 1 %s from Wagon %d.\n", selected_material->name, current_wagon->wagon_id);
            } else {
                current_material = current_material->next;
            }
        }

        // Move to the previous wagon
        current_wagon = current_wagon->prev;
    }

    // Inform the user if not enough materials were available
    if (remaining_quantity > 0) {
        printf("\n==========\nCould not unload the requested amount. %d %s remaining.\n==========\n\n",
               remaining_quantity, selected_material->name);
    } else {
        printf("\n==========\nUnloading completed.\n==========\n\n");
    }

    // Delete empty wagons and renumber the remaining wagons
    delete_empty_wagons(train);
}




void load_specified_material_to_train_main(Train *train, MaterialType *materials, int material_count) {
    char input[50];
    int material_choice;
    int quantity;

    printf("Select material to load:\n");
    for (int i = 0; i < material_count; i++) {
        printf("%d. %s\n", i + 1, materials[i].name);
    }

    while (1) {
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &material_choice) != 1 || material_choice < 1 || material_choice > material_count) {
            printf("\nInvalid material choice.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Enter the number of materials to load: ");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d", &quantity) != 1 || quantity <= 0) {
            printf("\nInvalid quantity. \n");
            continue;
        }
        break;
    }

    load_specified_material_to_train(train, &materials[material_choice - 1], quantity);
}


void empty_train_or_wagon(Train *train) {
    if (!train || !train->first_wagon) {
        printf("\n==========\nTrain is already empty.\n==========\n\n");
        return;
    }

    printf("\nDo you want to empty:\n");
    printf("1. The entire train\n");
    printf("2. A specific wagon\n");
    printf("Enter your choice: ");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("\n==========\nInvalid input. Operation canceled.\n==========\n\n");
        clear_stdin(); // Clear input buffer
        return;
    }
    clear_stdin();

    if (choice == 1) {
        // Empty the entire train
        Wagon *current_wagon = train->first_wagon;

        while (current_wagon) {
            LoadedMaterial *current_material = current_wagon->loaded_materials;
            while (current_material) {
                current_material->type->loaded -= 1; // Update material quantity
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

        printf("\n==========\nThe train has been emptied.\n==========\n\n");
    } else if (choice == 2) {
        // Empty a specific wagon
        printf("Enter the Wagon ID to empty: ");
        int wagon_id;
        if (scanf("%d", &wagon_id) != 1) {
            printf("\n==========\nInvalid input. Operation canceled.\n==========\n\n");
            clear_stdin();
            return;
        }
        clear_stdin();

        Wagon *current_wagon = train->first_wagon;

        // Find the wagon with the specified ID
        while (current_wagon && current_wagon->wagon_id != wagon_id) {
            current_wagon = current_wagon->next;
        }

        if (!current_wagon) {
            printf("\n==========\nError: Wagon ID %d does not exist.\n==========\n\n", wagon_id);
            return;
        }

        // Empty the specific wagon
        LoadedMaterial *current_material = current_wagon->loaded_materials;
        while (current_material) {
            current_material->type->loaded -= 1; // Update material quantity
            LoadedMaterial *to_free = current_material;
            current_material = current_material->next;
            free(to_free);
        }

        current_wagon->loaded_materials = NULL;
        current_wagon->current_weight = 0;

        printf("\n==========\nWagon %d has been emptied.\n==========\n\n", wagon_id);

        // Delete empty wagons and renumber
        delete_empty_wagons(train);
    } else {
        printf("\n==========\nInvalid choice. Operation canceled.\n==========\n\n");
    }
}

