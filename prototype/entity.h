
// Property Types
// - Int
// - Float
// - String
// - Pointer (Entity and asset are both implemented with this type, with
// additional semantic checking of the pointer).

// Asset must be of type

// Returning an unnamed class definition
void *redc_entity_class_new(void *eng, const char *name);
void *redc_entity_class_load(void *eng, const char *name, const char *decl_string);
void redc_entity_class_delete(void *kl);

// Return an entity class based on its name
void *redc_get_entity_class(void *eng, const char *name);

// Use some copy-on-write scheme so that entities already instantiated aren't
// affected.
void redc_entity_class_add_property(void **klass, const char *p);
void redc_entity_class_set_inherit(void **klass, void *oe);

void redc_entity_class_set_property_default_int(void **ekl, const char *p, int v);
void redc_entity_class_set_property_default_float(void **ekl, const char *p, float v);
void redc_entity_class_set_property_default_string(void **ekl, const char *p, const char *v);
void redc_entity_class_set_property_default_pointer(void **ekl, const char *p, void *v);

int redc_entity_class_get_property_default_int(void *ekl, const char *p);
float redc_entity_class_get_property_default_float(void *ekl, const char *p);
const char *redc_entity_class_get_property_default_string(void *ekl, const char *p);
void *redc_entity_class_get_property_default_pointer(void *ekl, const char *p);

void *redc_entity_instantiate(void *klass);
void *redc_entity_instantiate_from_class_name(const char *klname);
void redc_entity_destroy(void *e);

// Add a new property, must be done explicitally.
void redc_entity_add_property(void **entity, const char *p);

// Set property
void *redc_entity_set_property_int(void *e, const char *p, int val);
void *redc_entity_set_property_float(void *e, const char *p, float val);
void *redc_entity_set_property_string(void *e, const char *p, const char *val);
void *redc_entity_set_property_pointer(void *e, const char *p, void *ptr);

// Query value, undefined if the default or last set value is not of that type,
// though in reality I should implement float -> int and vice versa.
int redc_entity_get_property_int(void *e, const char *p);
float redc_entity_get_property_float(void *e, const char *p);
const char *redc_entity_get_property_string(void *e, const char *p);
void *redc_entity_get_property_pointer(void *e, const char *p);

// Could be any class in the entities lineage, or an empty string if it's a
// unique property created with redc_entity_add_property.
const char *redc_entity_get_property_owner(void *e, const char *p);
