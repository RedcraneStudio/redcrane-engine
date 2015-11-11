/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <vector>
#include <set>

#include "../common/algorithm.h"
#include "../common/aabb.h"

#include "../common/ID_Gen.hpp"
namespace game { namespace collis
{
  using SAP_AABB_Ref = uint16_t;

  namespace detail
  {
    enum class SAP_Endpoint_Type
    {
      Begin, End
    };

    struct SAP_Endpoint
    {
      SAP_AABB_Ref id;

      // Fuck it, we're being a little redundant here, but it's probably not
      // that big a deal. This is also better for cache coherency anyway. We
      // still need the id though since we can't rely on floating point
      // equality.
      AABB aabb;

      SAP_Endpoint_Type type;
      float value;
    };

    bool operator<(SAP_Endpoint const& se1, SAP_Endpoint const& se2) noexcept;
  }

  using pair_res_t = std::vector<std::pair<SAP_AABB_Ref, SAP_AABB_Ref> >;

  class SAP
  {
    ID_Gen<SAP_AABB_Ref> ref_counter_;

    // We need a set of unique sap endpoints, sorted by their deref'ed ptr.
    std::vector<detail::SAP_Endpoint> x_axis_;
  public:
    SAP_AABB_Ref insert(AABB const& aabb) noexcept;
    void update(SAP_AABB_Ref, AABB const&) noexcept;

    pair_res_t collisions() const noexcept;
  };
} }
