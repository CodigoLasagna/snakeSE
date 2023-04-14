#ifndef ELEMENTS_H
#define ELEMENTS_H
#pragma once

#include <cglm/cglm.h>
#include "shaders.h"
#include <GLFW/glfw3.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

typedef struct _tconfig{
	int resWidth, resHeight;
	int wireframe;
}Tconfig;

typedef struct _tobject{
	unsigned int VAO; /*Vertex array object*/
	unsigned int VBO; /*Vertex buffer object*/
	unsigned int EBO; /*Element buffer object*/
	unsigned int max_meshes;
	int width, height, thickness;
	vec3 position;
	vec3 scale;
	int type;
	int indices_n;
	mat4 model;
}Tobject;

typedef struct _tmodel{
	unsigned int *VAO, *VBO, *EBO, *NBO, *TBO;
	unsigned int max_meshes;
	unsigned int *indices;
	mat4 model;
	float x, y, z;
	vec3 scale;
	int type;
	vec3 pos;
}Tmodel;

typedef struct _tshadow_cube_map
{
	unsigned int depth_shader, render_shader;
	unsigned int cube_map_texture;
	unsigned int frameBuffer;
	unsigned int near_plane, far_plane;
	unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;
	unsigned int aspect_ratio;
	mat4 shadowTransforms[6];
	mat4 shadowProj;
	mat4 matHelper;
	vec3 vecHelper;
	vec3 lookatVecs[6];
	GLenum faces;
}TshadowCM;

typedef struct _tcamera{
	float currentFrame;
	float lastFrame;
	float deltaTime;
	
	int width;
	int height;
	float nWidth;
	float nHeight;
	float scale;
	int perspective;
	mat4 model;
	mat4 view;
	mat4 projection;
	
	vec3 pos;
	vec3 front;
	vec3 up;
	
	float speed;
	int firstMouse;
	int allowMouse;
	
	float yaw;
	float pitch;
	float lastX;
	float lastY;
	float fov;
	
	GLuint frameBuffer, renderBufferColor, renderBufferDepth, renderBufferStencil;
}Tcamera;

typedef struct _trenderer{
	unsigned int shader;
	Tobject screen;
	unsigned int frameBuffer;
	unsigned int texcolBuffer;
	unsigned int renderBuffer;
}Trenderer;

int instance_create_quad(Tobject *ID, float x, float y, float z, int width, int height, float size, int nAttributes);
int instance_create_cube(Tobject *ID, float x, float y, float z, int width, int height, int thickness, float scale, int nAttributes);
	
int instance_draw(Tobject ID, unsigned int *shader, Tcamera camera);
	
int instanced_object_buffer(unsigned int *iBuffer, Tobject *ID, unsigned int amount, mat4 matrices[]);
int instanced_object_draw(Tobject ID, unsigned int amount);

int prepare_lightobj(unsigned int *light_shader, vec3 color);
int prepare_material(unsigned int *surface_shader, int lights_number, float specular_str);
int prepare_material_lum(unsigned int *surface_shader, int light_n, bool light_type, vec3 color, float ambient_mag, float linear, float quadratic);
int light_position(unsigned int *surface_shader, int light_n, vec3 position);

int load_model(Tmodel *model, char *directory, float x, float y, float z, float scale, int type);
int draw_model(Tmodel *model, unsigned int *shader, Tcamera camera);

int instanced_model_buffer(unsigned int *iBuffer, Tmodel *ID, unsigned int amount, mat4 matrices[]);
int instanced_model_draw(Tmodel ID, unsigned int amount);

int init_camera(Tcamera *camera, Tconfig config, float scale);
int run_camera(Tcamera *camera, GLFWwindow *window);
int apply_camera(unsigned int *shader, Tcamera camera);

int prepare_renderer(Trenderer *renderer, Tconfig config);
int run_renderer(Trenderer *renderer, Tcamera camera, vec3 bkg);
int run_rendererShader(Trenderer *renderer, unsigned int *shader, Tcamera camera);

int prepare_uniformblockData(unsigned int *shader, char *name);
int prepare_uniformblockMatrices(unsigned int *uboBuffer, Tcamera camera);
int run_uniformblockMatrices(unsigned int uboBuffer, Tcamera camera);

int init_config(Tconfig *config, int width, int height);

int prepare_shadow_cubemap(TshadowCM *shadowCM, unsigned int shadow_width, unsigned int shadow_height, float near_plane, float far_plane);
int calculate_shadow_cubemap_light(TshadowCM *shadowCM, Tobject light, Tcamera camera);

#endif
