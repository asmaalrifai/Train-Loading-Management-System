//file_ops.h
#ifndef FILE_OPS_H
#define FILE_OPS_H
#include <stdio.h>
#include <stdlib.h>
#include "../include/train.h"
#include "../include/utils.h"
#include "../include/material.h"
#include "../include/wagon.h"

void load_train_status_from_file(Train *train, const char *filename);
void save_train_status_to_file(Train *train, const char *filename);


#endif // FILE_OPS_H