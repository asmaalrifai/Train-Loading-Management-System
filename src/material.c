// mateerial,c
#include <stdio.h>
#include <string.h>
#include "../include/wagon.h"
#include "../include/train.h"
#include "../include/material.h"
#include "../include/file_ops.h"
#include "../include/utils.h"



void display_material_status(MaterialType *materials, int material_count, Train *train) {
    if (materials == NULL || material_count == 0) {
        printf("\n==========\nNo materials available.\n==========\n\n");
        return;
    }

    // Reset loaded quantities to 0 before recalculating
    for (int i = 0; i < material_count; i++) {
        materials[i].loaded = 0;
    }

    // Traverse the train to count loaded quantities
    if (train != NULL && train->first_wagon != NULL) {
        Wagon *current_wagon = train->first_wagon;
        while (current_wagon != NULL) {
            LoadedMaterial *current_material = current_wagon->loaded_materials;
            while (current_material != NULL) {
                for (int i = 0; i < material_count; i++) {
                    if (strcmp(current_material->type->name, materials[i].name) == 0) {
                        materials[i].loaded++;
                        break;
                    }
                }
                current_material = current_material->next;
            }
            current_wagon = current_wagon->next;
        }
    }

    // Display the material status
    printf("\n==========\nMaterial Status\n==========\n");
    for (int i = 0; i < material_count; i++) {
        printf("Material: %s\n", materials[i].name);
        printf("  Weight: %.2f kg\n", materials[i].weight);
        printf("  Total Quantity: %d\n", materials[i].quantity);
        printf("  Loaded Quantity: %d\n", materials[i].loaded);
        printf("\n");
    }
}

