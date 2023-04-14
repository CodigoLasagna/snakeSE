#include "source/engine/engine.h"
#include <time.h>


#define debug 0

typedef struct _tnode
{
	float x;
	float y;
	int dir;
	struct _tnode *sig;
}sTnode;

typedef sTnode *Tnode;


void push(Tnode *tail, Tnode *head, float x, float y, int dir);
Tnode pop(Tnode *head);
int apply_input(int dir, int *last_dir, GLFWwindow *window);
int move(int dir, float *x, float *y, float speed);

int main()
{
	Tconfig config;
	GLFWwindow *window;
	Tcamera camera;
	Trenderer render;
	Tobject light;
	Tobject tile_floor, obj_pellet, obj_background, obj_body, obj_guide;
	unsigned int floorDiffuse, floorSpecular, diff_apple, spec_apple, diff_background, diff_body, diff_head, diff_tail, diff_guide;
	unsigned int lightShader, instanceShader, objects_shader;
	unsigned int uboMatrices;
	unsigned int ibTiles;
	unsigned int i, j;
	size_t counter = 0, sep = 0;
	float objectsHeight = -4.0f;
	float x, y, prev_x, prev_y;
	float xp, yp;
	int dir = 0, last_dir = 0;
	float speed = 0.16f;
	int clock_time = 0;
	int score = 0;
	int status = 1;
	float effectTimer = 0;
	vec3 pelletColor = {0.4f, 0.1f, 1.0f};
	vec3 lighColor = {1.0f, 1.0f, 1.0f};
	vec3 bodyColorAl = {0.4f, 1.0f, 0.85f};
	vec3 bodyColorDe = {0.8f, 0.2f, 0.6f};
	vec3 adjustRot = {0.0f, 0.0f, 1.0f};
	unsigned int seed;
	vec3 scene_color = {0.2f, 0.4f, 0.35f};
	vec3 lpos = {0.0f, 10.0f, -1.0f};
	Tnode head = NULL;
	Tnode tail = NULL;
	Tnode temp = NULL;
	Tnode aux = NULL;
	
	window = prepareGLFW(640, 640);
	prepareGLEW();
	
	init_config(&config, 640, 640);
	init_camera(&camera, config, 4);
	
	createShader(&lightShader, "./source/shaders/vertexShaders/obj_shader.vert", "./source/shaders/fragmentShaders/light.frag");
	createShader(&instanceShader, "./source/shaders/vertexShaders/ins_shader.vert", "./source/shaders/fragmentShaders/surfaceLight.frag");
	createShader(&objects_shader, "./source/shaders/vertexShaders/obj_shader.vert", "./source/shaders/fragmentShaders/surfaceLight.frag");
	createShader(&(render.shader), "./source/shaders/vertexShaders/rendertoquad.vert", "./source/shaders/fragmentShaders/chromAberration.frag");
	
	createTexture(&floorDiffuse, "./source/images/apple.png", 2, 1);
	createTexture(&floorSpecular, "./source/images/grass_02_specular.png", 1, 1);
	
	createTexture(&diff_apple, "./source/images/pellet.png", 2, 1);
	createTexture(&spec_apple, "./source/images/grass_02_specular.png", 1, 1);
	
	createTexture(&diff_body, "./source/images/body.png", 2, 1);
	createTexture(&diff_head, "./source/images/head.png", 2, 1);
	createTexture(&diff_tail, "./source/images/tail.png", 2, 1);
	
	createTexture(&diff_background, "./source/images/bkg.png", 2, 1);
	createTexture(&diff_guide, "./source/images/wasd.png", 2, 1);
	
	instance_create_quad(&obj_background, 0.0f, 0.0f, -1.003f, 640, 640, 1.0f, 5);
	
	instance_create_cube(&light, 0.0f, 0.0f, 0.3f, 100, 100, 100, 0.5f, 5);
	instance_create_quad(&tile_floor, 0.0f, 0.0f, 0.0f, 100, 100, 1.0f, 5);
	instance_create_quad(&obj_pellet, 0.0f, 0.0f, -1.001f, 16, 16, 1.0f, 5);
	instance_create_quad(&obj_body, 0.0f, 0.0f, -1.0f, 16, 16, 1.0f, 5);
	instance_create_quad(&obj_guide, 0.0f, 0.0f, -1.0002f, 192, 192, 1.0f, 5);
	
	prepare_lightobj(&lightShader, pelletColor);
	prepare_material(&instanceShader, 1, 0.0f);
	prepare_material_lum(&instanceShader, 0, false, pelletColor, 0.7f, 0.09f, 0.032f);
	
	prepare_material(&objects_shader, 200, 0.0f);
	prepare_material_lum(&objects_shader, 0, false, lighColor, 0.0001f, 0.7f, 2.5f);
	prepare_material_lum(&objects_shader, 1, false, pelletColor, 1.0f, 4.8f, 14.5f);
	for(i = 2; i < 200; i++)
	{
		prepare_material_lum(&objects_shader, i, false, bodyColorAl, 1.0f, 0.7f, 90.0f);
		light_position(&objects_shader, i, lpos);
	}
	
	
	prepare_uniformblockData(&lightShader, "Matrices");
	prepare_uniformblockData(&instanceShader, "Matrices");
	prepare_uniformblockData(&objects_shader, "Matrices");
	
	prepare_uniformblockMatrices(&uboMatrices, camera);
	prepare_renderer(&render, config);
	
	seed = time(NULL);
	srand(seed);
	
	while(obj_pellet.position[0] == 0)
	{
		xp = ((float)((random()%36)-18)*16)/100;
		yp = ((float)((random()%36)-18)*16)/100;
		obj_pellet.position[0] = xp+0.08f;
		obj_pellet.position[1] = yp+0.08f;
	}
	x = 0.0, y = 0.0;
	push(&tail, &head, x, y, dir);
	
	
	while (!glfwWindowShouldClose(window))
	{
		glfwGetWindowSize(window, &(camera.width), &(camera.height));
		camera.currentFrame = (float)glfwGetTime();
		camera.deltaTime = camera.currentFrame - camera.lastFrame;
		camera.lastFrame = camera.currentFrame;
		
		/*render normal scene*/
		run_uniformblockMatrices(uboMatrices, camera);
		run_camera(&camera, window);
		run_renderer(&render, camera, scene_color);
		processInput(window, &config);
		
		
		bind_texture(diff_background, 0);
		instance_draw(obj_background, &objects_shader, camera);
		
		if (!score)
		{
			bind_texture(diff_guide, 0);
			instance_draw(obj_guide, &objects_shader, camera);
		}
		
		glm_mat4_identity(obj_pellet.model);
		glm_translate(obj_pellet.model, obj_pellet.position);
		glm_rotate(obj_pellet.model, glm_rad(glfwGetTime() * 20), adjustRot);
		
		instance_draw(light, &lightShader, camera);
		bind_texture(diff_apple, 0);
		instance_draw(obj_pellet, &objects_shader, camera);
		
		
		glm_mat4_identity(obj_body.model);
		glm_translate(obj_body.model, obj_body.position);
		
		
		
		temp = head;
		counter = 2;
		while(temp)
		{
			obj_body.position[0] = temp->x+0.08f;
			obj_body.position[1] = temp->y+0.08f;
			glm_mat4_identity(obj_body.model);
			glm_translate(obj_body.model, obj_body.position);
			bind_texture(diff_body, 0);
			if (temp == tail)
			{
				bind_texture(diff_head, 0);
			}
			if (tail != temp && tail != head)
			{
				if (x == temp->x)
				{
					if (y == temp->y)
					{
						if (status)
						{
							for(i = 2; i < 200; i++)
							{
								prepare_material_lum(&objects_shader, i, false, bodyColorDe, 1.0f, 0.7f, 90.0f);
							}
							system("play ./source/sounds/death");
							effectTimer = 0.1;
							status = 0;
							prev_x = x;
							prev_y = y;
						}
					}
				}
			}
			
			if (temp == head && temp != tail)
			{
				bind_texture(diff_tail, 0);
			}
			glm_rotate(obj_body.model, glm_rad(90*(temp->dir-1)), adjustRot);
			light_position(&objects_shader, counter, obj_body.position);
			instance_draw(obj_body, &objects_shader, camera);
			temp = temp->sig;
			counter++;
		}
		
		clock_time++;
		if ((clock_time%5) == 0)
		{
			if (status)
			{
				move(dir, &x, &y, speed);
				push(&tail, &head, x, y, dir);
			}
			if (head)
			{
				printf("h x:%f y:%f\n", x*100+320, y*100+320);
				printf("----------------------------\n");
				if (x == xp && y == yp)
				{
					xp = ((float)((random()%36)-18)*16)/100;
					yp = ((float)((random()%36)-18)*16)/100;
					obj_pellet.position[0] = xp+0.08f;
					obj_pellet.position[1] = yp+0.08f;
					system("play ./source/sounds/pickup & disown");
					effectTimer = 0.2;
					score++;
				}
				else
				{
					if (head->sig)
					{
						aux = pop(&head);
						free(aux);
						if (!status)
						{
							effectTimer += 0.015;
							system("play ./source/sounds/hurt3 & disown");
						}
					}
					else
					{
						status = 1;
						for(i = 2; i < 200; i++)
						{
							prepare_material_lum(&objects_shader, i, false, bodyColorAl, 1.0f, 0.7f, 90.0f);
							light_position(&objects_shader, i, lpos);
						}
					}
				}
				
			}
		}
		if (((int)((head->x)*100)%16) == 0)
		{
			if (((int)((head->y)*100)%16) == 0)
			{
				dir = last_dir;
			}
		}
		if (effectTimer >= 0)
		{
			if (status)
			{
				effectTimer -= 0.01;
			}
			else
			{
				effectTimer -= 0.0025;
			}
			setFloat(&(render.shader), "iTime", sin(effectTimer));
		}
		else
		{
			setFloat(&(render.shader), "iTime", (0.01));
		}
		
		light_position(&objects_shader, 1, obj_pellet.position);
		
		apply_input(dir, &last_dir, window);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}

void push(Tnode *tail, Tnode *head, float x, float y, int dir)
{
	Tnode new = NULL;
	new = (Tnode) malloc(sizeof(sTnode));
	new->x = x;
	new->y = y;
	new->dir = dir;
	new->sig = NULL;
	if (!*head)
	{
		(*head) = new;
		(*tail) = new;
	}
	else
	{
		(*tail)->sig = new;
		(*tail) = new;
	}
}

Tnode pop(Tnode *head)
{
	Tnode temp = NULL;
	if (*head)
	{
		temp = (*head);
		*head = (temp->sig);
	}
	return temp;
}

int move(int dir, float *x, float *y, float speed)
{
	switch(dir)
	{
		case 0:
			(*x) += speed;
		break;
		case 1:
			(*y) += speed;
		break;
		case 2:
			(*x) -= speed;
		break;
		case 3:
			(*y) -= speed;
		break;
	}
	
	(*x) = (floor((*x)*100)/100);
	(*y) = (floor((*y)*100)/100);
	if (((int)(((*x))*100)%2) != 0)
	{
		(*x) += 0.01;
	}
	if (((int)(((*y))*100)%2) != 0)
	{
		(*y) += 0.01;
	}
	
	if ((*x) > 3.2)
	{
		(*x) = -3.2;
	}
	if ((*x) < -3.22)
	{
		(*x) = 3.2;
	}
	if ((*y) > 3.2)
	{
		(*y) = -3.2;
	}
	if ((*y) < -3.22)
	{
		(*y) = 3.2;
	}
	return 0;
}

int apply_input(int dir, int *last_dir, GLFWwindow *window)
{
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			if (dir != 2)
			{
				*last_dir = 0;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			if (dir != 1)
			{
				*last_dir = 3;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			if (dir != 0)
			{
				*last_dir = 2;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			if (dir != 3)
			{
				*last_dir = 1;
			}
		}
	return 0;
}
