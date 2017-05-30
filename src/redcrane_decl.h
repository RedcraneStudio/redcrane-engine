/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 */

// See cwrap/engine.cpp

// We need this on the lua side.
void free(void *ptr);

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

void redc_ms_sleep(unsigned long ms);

float redc_cur_time();

void* redc_init_engine(Redc_Config cfg);
void redc_init_client(void* eng);
void redc_init_server(void* eng);
void redc_uninit_engine(void* eng);

void redc_step_engine(void* eng);

bool redc_running(void *eng);

const char* redc_get_asset_path(void* eng);
void redc_window_swap(void* eng);

typedef struct {
  float x;
  float y;
} Redc_Vec2;

Redc_Vec2 redc_window_extents(void* eng);

void redc_gc(void* eng);

// See cwrap/mesh.cpp

void* redc_load_mesh(void* engine, const char* str);
void redc_unload_mesh(void* mesh);

// See cwrap/texture.cpp

void* redc_load_texture(void* eng, const char* str);
void redc_unload_texture(void* tex);

// See cwrap/scene.cpp

typedef uint16_t obj_id;

void* redc_make_scene(void* engine);
void redc_unmake_scene(void* scene);

obj_id redc_scene_add_camera(void *sc, const char *tp);
obj_id redc_scene_add_mesh(void* sc, void* msh);

obj_id redc_scene_add_player(void* sc);

void redc_scene_set_active_camera(void *sc, obj_id);
obj_id redc_scene_get_active_camera(void *sc);

// Set a camera to follow the active player. Multiple cameras can have this
// property even if that isn't really helpful.
void redc_scene_camera_set_follow_player(void *sc, obj_id cam, bool en);

void redc_scene_set_parent(void* sc, obj_id obj, obj_id parent);

void redc_scene_object_set_texture(void *sc, obj_id obj, void *tex);

void redc_scene_step(void *sc);
void redc_scene_render(void *sc);

// See cwrap/shader.cpp

void *redc_make_shader_builder(void *eng, const char *name);
// DON'T call this if you've already compiled the shader. DO call this if you
// want to cancel the creation of a shader, etc.
void redc_unmake_shader_builder(void *builder);

// These functions append code to a given shader kind or whatever you want to
// call it. Append as much code as necessary, then move on to compile.
void redc_shader_append_vertex_part(void *builder, const char *str);
void redc_shader_append_fragment_part(void *builder, const char *str);
void redc_shader_append_geometry_part(void *builder, const char *str);

// This function gives a certain variable name a tag, these tags should be well
// known in the code. Right now we will use the mechanism that was already in
// place where the shader stores the location of a few commonly-used uniform
// variables, for example the projection matrix, the diffuse color, etc. Sooner
// or later it might be better to put that information in a hashmap so we don't
// commit to anything. Right now we have a couple of tags that we want to
// recognize in the engine.
// - projection: The projection matrix (mat4)
// - view: The view matrix (mat4)
// - model: The model matrix (mat4)
void redc_shader_tag_uniform(void *build, const char *tag, const char *name);

void redc_shader_compile_vertex_part(void *builder);
void redc_shader_compile_fragment_part(void *builder);
void redc_shader_compile_geometry_part(void *builder);

// Returns an actual shader object ready to be set for an object or maybe
// scene-wide (TODO: That). This function will deallocate the shader builder and
// will return a new, compiled shader that must be deallocated with a call to
// redc_shader_destroy.
void *redc_shader_link(void *shade);

// This should be called on a fully-linked shader (ie the return value of
// redc_shader_compile).
void redc_shader_destroy(void *shader);

// See cwrap/map.cpp

void redc_map_load(void* eng, const char* file);

size_t redc_map_get_num_lights(void *map_ptr);

typedef struct
{
  float power;
} Redc_Light_State;

Redc_Light_State redc_map_get_light_i_state(void *map_ptr, size_t ref);
Redc_Light_State redc_map_get_light_state(void *map, const char *light);

int redc_map_set_light_i_state(void *map_ptr, size_t ref,
                               Redc_Light_State state);
int redc_map_set_light_state(void *map, const char *light,
                             Redc_Light_State state);

// See cwrap/server.cpp

int redc_server_running(void *eng);
int redc_server_start(void *eng, int max_clients);
void redc_server_step(void *eng);
void redc_server_req_player(void *eng);

typedef struct
{
  const char *type;
  const char *name;
  void *data;
} Redc_Event;
int redc_server_poll_event(void *eng, Redc_Event *event);

// See cwrap/text.cpp

void redc_text_draw(void *eng, const char *text, float x, float y);

void *redc_text_stream_new(void *eng, float timeout);
void redc_text_stream_delete(void *stream);

void redc_text_stream_push_string(void *stream, const char *text);
const char* redc_text_stream_full_text(void *stream);
void redc_text_stream_step(void *stream, float dt);

// Init dialogue, load font, etc.
void redc_dialogue_init(void *eng,
                        const char *default_font,
                        int default_size,
                        int default_timeout,
                        char *default_color);

// Add a new line.
void redc_dialogue_push_line(void *eng, const char *text);
// Add text to the end of the current line.
void redc_dialogue_push_text(void *eng, const char *text);
