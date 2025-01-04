//  display_train() in train.c to traverse the train and show wagon details.

#include <stdio.h>
#include "../include/train.h"

void display_train_status(Train *train) {
    if (train == NULL || train->first_wagon == NULL) {
        printf("\n==========\nNo wagons in the train.\n==========\n\n");
        return;
    }

    printf("\n==========\nTrain ID: %s\nTotal Wagons: %d\n==========\n", train->train_id, train->wagon_count);

    Wagon *current_wagon = train->first_wagon;
    while (current_wagon != NULL) {
        printf("Wagon ID: %d\n", current_wagon->wagon_id);
        printf("  Max Weight: %.2f kg\n", current_wagon->max_weight);
        printf("  Current Weight: %.2f kg\n", current_wagon->current_weight);

        if (current_wagon->loaded_materials == NULL) {
            printf("  No materials loaded.\n");
        } else {
            printf("  Loaded Materials:\n");
            LoadedMaterial *current_material = current_wagon->loaded_materials;
            while (current_material != NULL) {
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
