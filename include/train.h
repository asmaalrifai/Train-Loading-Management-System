// train,h
#ifndef TRAIN_H
#define TRAIN_H

typedef struct MaterialType
{
    char name[50];
    float weight;
    int quantity; // Total available
    int loaded;   // Currently on train
} MaterialType;

typedef struct LoadedMaterial
{
    MaterialType *type; // Pointer to material type
    struct LoadedMaterial *next, *prev;
} LoadedMaterial;

typedef struct Wagon
{
    int wagon_id;                     // Unique ID
    float max_weight;                 // Maximum weight capacity
    float current_weight;             // Current weight
    LoadedMaterial *loaded_materials; // List of loaded materials
    struct Wagon *next, *prev;
} Wagon;

typedef struct Train
{
    char train_id[20];  // Train identifier
    Wagon *first_wagon; // Pointer to the first wagon
    int wagon_count;    // Total wagons
} Train;

//6, 7
void display_train_status(Train *train); //
void display_material_status(MaterialType *materials, int material_count); //

Train *create_mock_train(); //

// 2, 3
// void load_material_to_train(Train *train, MaterialType *material);
void load_material_to_wagon(Train *train, MaterialType *material, int wagon_id); //
void load_specified_material_to_train(Train *train, MaterialType *material, int quantity); //

#endif
