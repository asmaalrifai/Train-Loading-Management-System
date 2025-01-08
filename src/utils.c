// utils.c
#include <stdlib.h>
#include <string.h>
#include "../include/wagon.h"
#include "../include/train.h"
#include "../include/material.h"
#include "../include/file_ops.h"
#include "../include/utils.h"





int check_material_availability(MaterialType *material, int quantity)
{
    return (quantity > 0 && quantity <= (material->quantity - material->loaded));
}

int check_wagon_space(Wagon *wagon, MaterialType *material)
{
    return (wagon->max_weight - wagon->current_weight >= material->weight);
}

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
