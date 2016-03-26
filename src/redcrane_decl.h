/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 */

typedef struct
{
  const char* window_title;
} Redc_Config;

void* redc_init_engine(Redc_Config cfg);
void redc_uninit_engine(void* eng);

bool redc_running(void* eng);
void redc_step(void* eng);

void* redc_load_mesh(void* engine, const char* str);
void redc_unload_mesh(void* mesh);

void* redc_make_scene(void* engine);
void redc_unmake_scene(void* scene);

// TODO: Change the way cameras work

uint16_t redc_scene_add_camera(void* sc, const char* tp);
uint16_t redc_scene_get_active_camera(void* sc);
void redc_scene_activate_camera(void* sc, uint16_t cam);

void redc_scene_attach(void* sc, void* mesh, void* parent);

void redc_draw_mesh(void* engine, void* mesh);
void redc_swap_window(void* engine);
