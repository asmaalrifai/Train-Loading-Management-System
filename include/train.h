#ifndef TRAIN_H
#define TRAIN_H

#include "../include/wagon.h"

// Train structure
typedef struct Train {
    char train_id[20];  // Train identifier
    Wagon *first_wagon; // Pointer to the first wagon
    int wagon_count;    // Total wagons
} Train;

// Train management functions
Train *create_train();
void display_train_status(Train *train);

// Material loading/unloading functions
void load_material_to_train(Train *train, MaterialType *material);
void load_specified_material_to_train(Train *train, MaterialType *material, int quantity);
void unload_material_from_tail(Train *train, MaterialType *materials, int material_count);
void load_specified_material_to_train_main(Train *train, MaterialType *materials, int material_count);
void empty_train_or_wagon(Train *train);


#endif // TRAIN_H
