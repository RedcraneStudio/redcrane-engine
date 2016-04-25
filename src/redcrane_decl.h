/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 */

// See cwrap/engine.cpp

typedef struct
{
  const char* cwd;
  const char* mod_name;
  uint16_t default_port;
  const char* client_entry;
  const char* server_entry;
} Redc_Config;

void redc_log_d(const char* str);
void redc_log_i(const char* str);
void redc_log_w(const char* str);
void redc_log_e(const char* str);

void* redc_init_engine(Redc_Config cfg);
void redc_init_client(void* eng);
void redc_init_server(void* eng);
void redc_uninit_engine(void* eng);

bool redc_running(void *eng);

const char* redc_get_asset_path(void* eng);
void redc_window_swap(void* eng);

void redc_gc(void* eng);

// See cwrap/mesh.cpp

void* redc_load_mesh(void* engine, const char* str);
void redc_unload_mesh(void* mesh);

// See cwrap/scene.cpp

typedef uint16_t obj_id;

void* redc_make_scene(void* engine);
void redc_unmake_scene(void* scene);

obj_id redc_scene_add_camera(void *sc, const char *tp);
obj_id redc_scene_add_mesh(void* sc, void* msh);

obj_id redc_scene_add_player(void* sc);

void redc_scene_set_active_camera(void *sc, obj_id);
obj_id redc_scene_get_active_camera(void *sc);

void redc_scene_set_parent(void* sc, obj_id obj, obj_id parent);

void redc_scene_step(void *sc);
void redc_scene_render(void *sc);

// See cwrap/shader.cpp

void *redc_make_shader(void *eng, const char* dir);
void redc_unmake_shader(void *shader);

// See cwrap/map.cpp

void* redc_map_load(void* eng, const char* file);
