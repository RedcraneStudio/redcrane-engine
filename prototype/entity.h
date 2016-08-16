
// Property Types
// - Int
// - Float
// - String
// - Pointer (Entity and asset are both implemented with this type, with
// additional semantic checking of the pointer).

// Asset must be of type pointer

// Returns a new component definition
void *redc_component_new(const char *name);
void redc_component_delete(void *comp);

// No need to free the name
const char *get_component_name(void *comp);

struct Property_Value
{
  enum
  {
    Empty, Int, Float, String, Pointer
  } type;

  union
  {
    int i;
    float f;
    // Owned, null-terminated.
    const char *str;
    // Not owned
    void *ptr;
  };
};

void redc_component_add_property(void *comp, const char *p);
// Strings are copied around, pointer is stored as-is.
void redc_component_add_property_value(void *comp, const char *p,
                                       Property_Value val);

void redc_component_set_property_default_int(void *c, const char *p, int v);
void redc_component_set_property_default_float(void *c, const char *p, float v);
void redc_component_set_property_default_string(void *c, const char *p, const char *v);
void redc_component_set_property_default_pointer(void *c, const char *p, void *v);

int redc_component_get_property_default_int(void const *c, const char *p);
float redc_component_get_property_default_float(void const *c, const char *p);
const char *redc_component_get_property_default_string(void const *c, const char *p);
void *redc_component_get_property_default_pointer(void const *c, const char *p);

void *redc_entity_new();
void redc_entity_delete(void *entity);

void redc_entity_init_transform_comp(void *e);
// Add transform functions here.

struct Redc_Render_Init
{
  const char *asset;
};

void redc_entity_init_render_comp(void *e, Redc_Render_Init init);

struct Redc_Physics_Init
{
  const char *vertices_accessor;
  const char *indices_accessor;
};
void redc_entity_init_physics_comp(void *e, Redc_Physics_Init init);

void redc_entity_add_component(void *entity, void *comp);
bool redc_entity_has_component(void *entity, void *comp);

// Add a new property, must be done explicitally.
void redc_entity_add_property(void *entity, const char *p);

// Set property
void *redc_entity_set_int(void *e, const char *p, int val);
void *redc_entity_set_float(void *e, const char *p, float val);
void *redc_entity_set_string(void *e, const char *p, const char *val);
void *redc_entity_set_pointer(void *e, const char *p, void *ptr);

// Query value, undefined if the default or last set value is not of that type,
// though in reality I should implement float -> int and vice versa.
int redc_entity_get_int(void *e, const char *p);
float redc_entity_get_float(void *e, const char *p);
const char *redc_entity_get_string(void *e, const char *p);
void *redc_entity_get_pointer(void *e, const char *p);

// Could be any class in the entities lineage, or an empty string if it's a
// unique property created with redc_entity_add_property.
const char *redc_entity_get_property_owner(void *e, const char *p);
