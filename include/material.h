#ifndef MATERIAL_H
#define MATERIAL_H

struct Train;         
struct Wagon;     

typedef struct MaterialType {
    char name[50];
    float weight;
    int quantity; // Total available
    int loaded;   // Currently on train
} MaterialType;

typedef struct LoadedMaterial {
    struct MaterialType *type;
    struct LoadedMaterial *next, *prev;
} LoadedMaterial;

void display_material_status(MaterialType *materials, int material_count, struct Train *train);

#endif 
