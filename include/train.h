// train.h
#ifndef TRAIN_H
#define TRAIN_H

#include "../include/wagon.h"

typedef struct Train
{
    char train_id[20];  // Train identifier
    Wagon *first_wagon; // Pointer to the first wagon
    int wagon_count;    // Total wagons
} Train;

void display_train_status(Train *train);
void empty_train_or_wagon(Train *train);
void unload_material_from_tail(Train *train);
void load_specified_material_to_train_main(Train *train, MaterialType *materials, int material_count);
Train *create_mock_train();

#endif // TRAIN_H

