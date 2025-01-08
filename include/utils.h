//utils.h
#ifndef UTILS_H
#define UTILS_H

struct Train;         // Forward declaration
struct MaterialType;  // Forward declaration
struct Wagon;         // Forward declaration

int check_material_availability(struct MaterialType *material, int quantity);
int check_wagon_space(struct Wagon *wagon, struct MaterialType *material);
void clear_stdin();

#endif // UTILS_H
