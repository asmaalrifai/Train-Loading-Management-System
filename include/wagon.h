// wagon.h
#ifndef WAGON_H
#define WAGON_H

#include "../include/material.h"

typedef struct Train Train; // Forward declaration for Train

typedef struct Wagon
{
    int wagon_id;                     // Unique ID
    float max_weight;                 // Maximum weight capacity
    float current_weight;             // Current weight
    LoadedMaterial *loaded_materials; // List of loaded materials
    struct Wagon *next, *prev;
} Wagon;

Wagon *create_new_wagon(Train *train);

void insert_material_into_wagon(Wagon *wagon, MaterialType *material);


void load_material_to_wagon(Train *train, MaterialType *material, int wagon_id, int quantity);
void load_material_to_wagon_main(Train *train, MaterialType *materials, int material_count);
void unload_material_from_wagon_main(Train *train, MaterialType *materials, int material_count);
void delete_empty_wagons(Train *train);
void empty_specific_wagon(Wagon *wagon);

#endif // WAGON_H

