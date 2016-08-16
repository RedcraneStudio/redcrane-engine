/*
 * Copyright (C) 2016 Luke San Antonio Bialecki
 * All rights reserved.
 */

// Requirements / things to keep in mind
// - Getting and setting values of properties should be fast.

#include <unordered_map>
#include "entity.h"
#include "redcrane.hpp"
#include "../gfx/scene.h"

namespace redc
{
  // These classes define components for entities
  struct Component
  {
    virtual ~Component() {}
  };

  struct Rendering_Component : public Component
  {
    redc::Asset asset;
    glm::mat4 model;
  };

  struct Physics_Component : public Component
  {
    btRigidBody* body;
  };

  struct Scripting_Component : public Component
  {
    std::string name;
    std::unordered_map<std::string, Property_Value> properties;
  };

  std::vector<Scripting_Component> scripting_components;

  struct Entity
  {
    std::vector<Scripting_Component> scripting_components;
    Rendering_Component* rendering_component;
    Physics_Component* physics_component;

    Scripting_Component custom_props;
  };

  struct Scripting_Component_Decl
  {
    std::string name;
    Scripting_Component_Decl *inherit;
    std::unordered_map<std::string, Property_Value> default_properties;

    ~Scripting_Component_Decl();
  };

  Scripting_Component_Decl::~Scripting_Component_Decl()
  {
    for(std::pair<std::string, Property_Value>& prop : default_properties)
    {
      // Free strings that were copied in.
      if(prop.second.type == Property_Value)
      {
        free();
      }
    }
  }

  Scripting_Component instantatiate(Scripting_Component_Decl const& comp)
  {
    Scripting_Component ret;
    ret.name = comp.name;

    // TODO: Implement

    return ret;
  }

  void set_property_default(Scripting_Component_Decl const& comp,
                            std::string const& prop,
                            Property_Value const& value)
  {
    auto prop_find = decl->default_properties.find(prop);

    if(prop_find != decl->default_properties.end())
    {
      prop_find->second = value;
    }
  }
}

// = Component (declaration) initialization
void *redc_component_new(const char *name)
{
  Scripting_Component_Decl *decl = new Scripting_Component_Decl();
  decl->name = std::string{name};

  return decl;
};
void redc_component_delete(void *comp)
{
  Scripting_Component_Decl *decl = (Scripting_Component_Decl*) comp;
  delete decl;
}

// = General queries
const char *get_component_name(void *comp)
{
  Scripting_Component_Decl *decl = (Scripting_Component_Decl*) comp;
  return decl->name.data();
}

// = Add a property
void redc_component_add_property(void *comp, const char *p)
{
  Scripting_Component_Decl *decl = (Scripting_Component_Decl*) comp;

  std::string prop{p};
  Property_Value value;
  value.type = Property_Value::Empty;

  decl->default_properties.emplace_back(prop, value);
}
// = Set what component this one inherits from.
void redc_component_set_inherit(void *comp, void *other)
{
  Scripting_Component_Decl *decl = (Scripting_Component_Decl*) comp;
  decl->inherit = (Scripting_Component_Decl*) other;
}
void *redc_component_get_inherit(void const *comp)
{
  Scripting_Component_Decl const *decl = (Scripting_Component_Decl*) comp;
  return decl->inherit;
}

// = Default property setters
void redc_component_set_property_default_int(void *c, const char *p, int v)
{
  Scripting_Component_Decl *decl = (Scripting_Component_Decl*) c;
  std::string prop{p};
  Property_Value val;
  val.type = Property_Value::Int;
  val.i = v;
  redc::set_property_default(*decl, prop, val);
}
void redc_component_set_property_default_float(void *c, const char *p, float v)
{

  Scripting_Component_Decl *decl = (Scripting_Component_Decl*) c;
  std::string prop{p};
  Property_Value val;
  val.type = Property_Value::Float;
  val.f = v;
  redc::set_property_default(*decl, prop, val);
}
void redc_component_set_property_default_string(void *c, const char *p,
                                                const char *v)
{
  Scripting_Component_Decl *decl = (Scripting_Component_Decl*) c;
  std::string prop{p};
  Property_Value val;
  val.type = Property_Value::String;

  // Find length
  size_t len = strlen(v);

  // Allocate new string + terminating character.
  val.str = (const char*) malloc(len + 1);
  // Copy to the new string
  memcpy(val.str, v, len + 1);

  // Continue
  redc::set_property_default(*decl, prop, val);
}
void redc_component_set_property_default_pointer(void *c, const char *p, void *v)
{
  Scripting_Component_Decl *decl = (Scripting_Component_Decl*) c;
  std::string prop{p};
  Property_Value val;
  val.type = Property_Value::Pointer;
  val.ptr = v;
  redc::set_property_default(*decl, prop, val);
}

// = Default property getters
int redc_component_get_property_default_int(void const *c, const char *p)
{
  Scripting_Component_Decl const *decl = (Scripting_Component_Decl*) c;
  std::string prop{p};
  Property_Value val = redc::get_property_default(decl, prop);

  switch(val.type)
  {
  case Property_Value::Int:
    return val.i;
  case Property_Value::Float:
    return (int) val.f;
  case Property_Value::Pointer:
    return static_cast<int>(val.ptr);
  default:
    return 0;
  };
}
float redc_component_get_property_default_float(void const *c, const char *p)
{
  Scripting_Component_Decl const *decl = (Scripting_Component_Decl*) c;
  std::string prop{p};
  Property_Value val = redc::get_property_default(decl, prop);

  switch(val.type)
  {
  case Property_Value::Int:
    return (float) val.i;
  case Property_Value::Float:
    return val.f;
  case Property_Value::Pointer:
    return (float) static_cast<int>(val.ptr);
  default:
    return 0.0f;
  };
}
const char *redc_component_get_property_default_string(void const *c,
                                                       const char *p)
{
  Scripting_Component_Decl const *decl = (Scripting_Component_Decl*) c;
  std::string prop{p};
  Property_Value val = redc::get_property_default(decl, prop);

  switch(val.type)
  {
  case Property_Value::String:
    return val.str;
  default:
    return "";
  };
}
void *redc_component_get_property_default_pointer(void const *c, const char *p);
{
  Scripting_Component_Decl const *decl = (Scripting_Component_Decl*) c;
  std::string prop{p};
  Property_Value val = redc::get_property_default(decl, prop);

  switch(val.type)
  {
  case Property_Value::Int:
    return static_cast<void*>(val.i);
  case Property_Value::Pointer:
    return val.ptr;
  default:
    return (void*) 0;
  };
}

// = Entity init code
void *redc_entity_new()
{
  redc::Entity *ret = new redc::Entity();
}
void redc_entity_delete(void *entity)
{
  delete (redc::Entity*) entity
}

// = Built in components
void redc_entity_init_transform_comp(void *e, Redc_Transform_Init init)
{

}
void redc_entity_init_render_comp(void *e, Redc_Render_Init init)
{

}
void redc_entity_init_physics_comp(void *e, Redc_Physics_Init init)
{

}

void redc_entity_add_component(void *e, void *comp)
{
  redc::Entity *entity = (redc::Entity*) e;
  Scripting_Component_Decl const *decl = (Scripting_Component_Decl*) comp;

  entity->scripting_components.push_back(redc::instantiate(*decl));
}

bool redc_entity_has_component(void *e, const char *name)
{
  redc::Entity *entity = (redc::Entity*) e;

  using std::begin; using std::end;
  auto iter = std::find_if(begin(entity->scripting_components),
                           end(entity->scripting_components),
  [name](Scripting_Component const& comp)
  {
    return std::strcmp(name, comp.name.data()) == 0;
  });
  return iter != entity->scripting_components.end();
}

// Add a new property, must be done explicitally.
void redc_entity_add_property(void **entity, const char *p);

// Set property
void *redc_entity_set_property_int(void *e, const char *p, int val);
void *redc_entity_set_property_float(void *e, const char *p, float val);
void *redc_entity_set_property_string(void *e, const char *p, const char *val);
void *redc_entity_set_property_pointer(void *e, const char *p, void *ptr);

// Query value, undefined if the default or last set value is not of that type,
// though in reality I should implement float -> int and vice versa.
int redc_entity_get_int(void *e, const char *p);
float redc_entity_get_float(void *e, const char *p);
const char *redc_entity_get_string(void *e, const char *p);
void *redc_entity_get_pointer(void *e, const char *p);
