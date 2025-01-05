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
    wagon1->current_weight = 0.0;
    wagon1->loaded_materials = NULL;
    wagon1->next = NULL;
    wagon1->prev = NULL;

    Wagon *wagon2 = (Wagon *)malloc(sizeof(Wagon));
    wagon2->wagon_id = 2;
    wagon2->max_weight = 1000.0;
    wagon2->current_weight = 0.0;
    wagon2->loaded_materials = NULL;
    wagon2->next = NULL;
    wagon2->prev = wagon1;

    wagon1->next = wagon2;
    train->first_wagon = wagon1;

    return train;
}
