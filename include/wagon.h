#ifndef WAGON_H
#define WAGON_H

#include "../include/material.h"

typedef struct Train Train;


typedef struct Wagon {
    int wagon_id;                     // Unique ID for the wagon
    float max_weight;                 // Maximum weight capacity
    float current_weight;             // Current weight of the wagon
    LoadedMaterial *loaded_materials; // List of loaded materials
    struct Wagon *next, *prev;        // Pointers for the doubly linked list
} Wagon;

// Wagon management functions
Wagon *create_new_wagon(Train *train);
void delete_empty_wagons(Train *train);
void empty_specific_wagon(Wagon *wagon);

// Material handling functions
void insert_material_into_wagon(Wagon *wagon, MaterialType *material);
void load_material_to_wagon(Train *train, MaterialType *material, int wagon_id, int quantity);
void display_wagon_status(Wagon *wagon);
void load_material_to_wagon_main(Train *train, MaterialType *materials, int material_count);
void unload_material_from_wagon_main(Train *train, MaterialType *materials, int material_count);
void unload_material_from_wagon(Wagon *wagon, MaterialType *material, int quantity);
Wagon *delete_empty_wagon_if_needed(Train *train, Wagon *wagon);

#endif 
