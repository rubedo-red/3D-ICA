#pragma once
#include <vector>
#include "mesh.h"

inline GLuint default_texture{0};

void create_default_texture();

static constexpr glm::vec4 primitive_colors[] = {
	{1.f, 1.f, 1.f, 1.f},
	{1.f, 0.f, 0.f, 1.f},
	{0.f, 1.f, 0.f, 1.f},
	{0.f, 0.f, 1.f, 1.f},
	{1.f, 1.f, 0.f, 1.f},
	{1.f, 0.f, 1.f, 1.f},
	{0.f, 1.f, 1.f, 1.f},
	{0.f, 0.f, 0.f, 1.f}
};

static constexpr GLfloat cube_verts[] = {
	-1.f, 1.f, 1.f,
	1.f, 1.f, 1.f,
	-1.f, -1.f, 1.f,
	1.f, -1.f, 1.f,

	-1.f, 1.f, -1.f,
	1.f, 1.f, -1.f,
	-1.f, -1.f, -1.f,
	1.f, -1.f, -1.f,
};

static constexpr VertexData cube_data[] = {
	{-1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{-1.f, -1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{1.f, -1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f},

	{-1.f, 1.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{1.f, 1.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{-1.f, -1.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	{1.f, -1.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f},
};

static constexpr GLuint cube_indices[] = {
	// back
	2, 1, 0,
	1, 2, 3,
	// front
	4, 5, 6,
	7, 6, 5,
	// top
	0, 1, 4,
	4, 1, 5,
	// bottom
	6, 3, 2,
	3, 6, 7,
	// left
	4, 2, 0,
	2, 4, 6,
	// right
	1, 3, 5,
	7, 5, 3
};