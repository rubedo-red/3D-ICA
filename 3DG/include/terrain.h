#pragma once
#include "mesh.h"

Mesh create_terrain_mesh(std::string noise, size_t cells_x=100, size_t cells_y=100, GLfloat cell_size=0.1f, GLfloat height=1.f);
