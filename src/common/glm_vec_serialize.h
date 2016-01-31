/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 *
 * \file glm_vec_serialize.h Contains serialization code for glm::vec{2,3,4}.
 */
#pragma once
#ifdef REDC_USE_MSGPACK
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <msgpack.hpp>

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

#define CONVERT_BEGIN(n, v) \
  template<> \
  struct convert<glm::vec##n> { \
    msgpack::object const& operator()(msgpack::object const& o, \
                                      glm::vec##n & v) const { \
      if (o.type != msgpack::type::ARRAY) throw msgpack::type_error(); \
      if (o.via.array.size != 2) throw msgpack::type_error();

#define CONVERT_END() \
      return o; \
    } \
  };

#define GET_FLOAT(n) o.via.array.ptr[n].as<float>()

CONVERT_BEGIN(2, v)
  v = glm::vec2(
    GET_FLOAT(0),
    GET_FLOAT(1)
  );
CONVERT_END()

CONVERT_BEGIN(3, v)
  v = glm::vec3(
    GET_FLOAT(0),
    GET_FLOAT(1),
    GET_FLOAT(2)
  );
CONVERT_END()

CONVERT_BEGIN(4, v)
  v = glm::vec4(
    GET_FLOAT(0),
    GET_FLOAT(1),
    GET_FLOAT(2),
    GET_FLOAT(3)
  );
CONVERT_END()

#undef CONVERT_BEGIN
#undef GET_FLOAT
#undef CONVERT_END

#define PACK_BEGIN(n) \
template<> \
struct pack<glm::vec##n> { \
  template <typename Stream> \
  packer<Stream>& operator()(msgpack::packer<Stream>& o, \
                             glm::vec##n const& v) const \
  { \
    o.pack_array(n);

#define PACK_END() \
      return o; \
    } \
  };

#define PACK_MEMBER(x) o.pack(v.x);

PACK_BEGIN(2)
  PACK_MEMBER(x)
  PACK_MEMBER(y)
PACK_END()

PACK_BEGIN(3)
  PACK_MEMBER(x)
  PACK_MEMBER(y)
  PACK_MEMBER(z)
PACK_END()

PACK_BEGIN(4)
  PACK_MEMBER(x)
  PACK_MEMBER(y)
  PACK_MEMBER(z)
  PACK_MEMBER(w)
PACK_END()

#undef PACK_BEGIN
#undef PACK_MEMBER
#undef PACK_END

#define OBJECT_WITH_ZONE_BEGIN(n) \
template <> \
struct object_with_zone<glm::vec##n> { \
  void operator()(msgpack::object::with_zone& o, glm::vec##n const& v) const \
  { \
    o.type = type::ARRAY; \
    o.via.array.size = n;\
    o.via.array.ptr = static_cast<msgpack::object*>( \
      o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size)); \

#define SET_INDEX(n, mem) o.via.array.ptr[n] = msgpack::object(v.mem,  o.zone);

#define OBJECT_WITH_ZONE_END() \
    } \
};

OBJECT_WITH_ZONE_BEGIN(2)
  SET_INDEX(0, x)
  SET_INDEX(1, y)
OBJECT_WITH_ZONE_END()
OBJECT_WITH_ZONE_BEGIN(3)
  SET_INDEX(0, x)
  SET_INDEX(1, y)
  SET_INDEX(2, z)
OBJECT_WITH_ZONE_END()
OBJECT_WITH_ZONE_BEGIN(4)
  SET_INDEX(0, x)
  SET_INDEX(1, y)
  SET_INDEX(2, z)
  SET_INDEX(3, w)
OBJECT_WITH_ZONE_END()

#undef OBJECT_WITH_ZONE_BEGIN
#undef SET_INDEX
#undef OBJECT_WITH_ZONE_END

template<>
struct convert<glm::quat> {
    msgpack::object const& operator()(msgpack::object const& o, glm::quat& q) const {
        if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
        if (o.via.array.size != 4) throw msgpack::type_error();
        q = glm::quat(o.via.array.ptr[0].as<float>(),
                      o.via.array.ptr[1].as<float>(),
                      o.via.array.ptr[2].as<float>(),
                      o.via.array.ptr[3].as<float>());
        return o;
    }
};

template<>
struct pack<glm::quat> {
    template <typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, glm::quat const& q) const {
        // packing member variables as an array.
        o.pack_array(4);
        o.pack(q.x);
        o.pack(q.y);
        o.pack(q.z);
        o.pack(q.w);
        return o;
    }
};

template <>
struct object_with_zone<glm::quat> {
    void operator()(msgpack::object::with_zone& o, glm::quat const& q) const {
        o.type = type::ARRAY;
        o.via.array.size = 4;
        o.via.array.ptr = static_cast<msgpack::object*>(
            o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));
        o.via.array.ptr[0] = msgpack::object(q.x, o.zone);
        o.via.array.ptr[1] = msgpack::object(q.y, o.zone);
        o.via.array.ptr[2] = msgpack::object(q.z, o.zone);
        o.via.array.ptr[3] = msgpack::object(q.w, o.zone);
    }
};

} // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack

#endif
