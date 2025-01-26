#ifndef UTILS_H
#define UTILS_H

struct Train;         
struct MaterialType;  
struct Wagon;       

int check_material_availability(struct MaterialType *material, int quantity);
int check_wagon_space(struct Wagon *wagon, struct MaterialType *material);
void clear_stdin();

#endif 
