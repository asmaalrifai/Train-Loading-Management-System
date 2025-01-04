//  display_material_type() in material.c to show material inventory.

#include <stdio.h>
#include "../include/train.h"

void display_material_status(MaterialType *materials, int material_count) {
    if (materials == NULL || material_count == 0) {
        printf("\n==========\nNo materials available.\n==========\n\n");
        return;
    }

    printf("\n==========\nMaterial Status\n==========\n");
    for (int i = 0; i < material_count; i++) {
        printf("Material: %s\n", materials[i].name);
        printf("  Weight: %.2f kg\n", materials[i].weight);
        printf("  Total Quantity: %d\n", materials[i].quantity);
        printf("  Loaded Quantity: %d\n", materials[i].loaded);
        printf("\n");
    }
}
