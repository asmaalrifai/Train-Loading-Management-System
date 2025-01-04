#include <stdlib.h>
#include <string.h>
#include "../include/train.h"

Train *create_mock_train() {
    Train *train = (Train *)malloc(sizeof(Train));
    strcpy(train->train_id, "T123");
    train->wagon_count = 2;

    Wagon *wagon1 = (Wagon *)malloc(sizeof(Wagon));
    wagon1->wagon_id = 1;
    wagon1->max_weight = 1000.0;
    wagon1->current_weight = 500.0;
    wagon1->next = NULL;
    wagon1->prev = NULL;

    MaterialType *large_box = (MaterialType *)malloc(sizeof(MaterialType));
    strcpy(large_box->name, "Large Box");
    large_box->weight = 200.0;
    large_box->quantity = 50;
    large_box->loaded = 5;

    LoadedMaterial *material1 = (LoadedMaterial *)malloc(sizeof(LoadedMaterial));
    material1->type = large_box;
    material1->next = NULL;
    material1->prev = NULL;

    wagon1->loaded_materials = material1;

    Wagon *wagon2 = (Wagon *)malloc(sizeof(Wagon));
    wagon2->wagon_id = 2;
    wagon2->max_weight = 1000.0;
    wagon2->current_weight = 600.0;
    wagon2->next = NULL;
    wagon2->prev = wagon1;

    MaterialType *medium_box = (MaterialType *)malloc(sizeof(MaterialType));
    strcpy(medium_box->name, "Medium Box");
    medium_box->weight = 150.0;
    medium_box->quantity = 50;
    medium_box->loaded = 4;

    LoadedMaterial *material2 = (LoadedMaterial *)malloc(sizeof(LoadedMaterial));
    material2->type = medium_box;
    material2->next = NULL;
    material2->prev = NULL;

    wagon2->loaded_materials = material2;

    wagon1->next = wagon2;
    train->first_wagon = wagon1;

    return train;
}
