/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "sweep_and_prune.h"
#include <algorithm>
#include "../common/algorithm.h"
namespace redc { namespace collis
{
  namespace detail
  {
    bool operator<(SAP_Endpoint const& se1, SAP_Endpoint const& se2) noexcept
    {
      return se1.value < se2.value;
    }
  }

  SAP_AABB_Ref SAP::insert(AABB const& aabb) noexcept
  {
    // New aabb
    auto id = ref_counter_.get();

    // Build each endpoint
    auto begin_endpoint = detail::SAP_Endpoint{};
    begin_endpoint.id = id;
    begin_endpoint.aabb = aabb;
    begin_endpoint.type = detail::SAP_Endpoint_Type::Begin;
    begin_endpoint.value = aabb.min.x;

    auto end_endpoint = detail::SAP_Endpoint{};
    end_endpoint.id = id;
    end_endpoint.aabb = aabb;
    end_endpoint.type = detail::SAP_Endpoint_Type::End;
    end_endpoint.value = aabb.min.x + aabb.width;

    sorted_insert(x_axis_, begin_endpoint);
    sorted_insert(x_axis_, end_endpoint);

    return id;
  }

  void SAP::update(SAP_AABB_Ref id, AABB const& aabb) noexcept
  {
    bool sort = false;
    for(auto& endpt : x_axis_)
    {
      if(endpt.id == id)
      {
        if(endpt.type == detail::SAP_Endpoint_Type::Begin)
        {
          endpt.value = aabb.min.x;
        }
        else if(endpt.type == detail::SAP_Endpoint_Type::End)
        {
          endpt.value = aabb.min.x + aabb.width;
        }

        sort = true;
      }
    }

    using std::begin; using std::end;
    if(sort) std::sort(begin(x_axis_), end(x_axis_));
  }

  pair_res_t SAP::collisions() const noexcept
  {
    pair_res_t ret;

    std::vector<detail::SAP_Endpoint> active_list;

    for(auto const& endpt : x_axis_)
    {
      if(endpt.type == detail::SAP_Endpoint_Type::Begin)
      {
        active_list.push_back(endpt);
      }
      else if(endpt.type == detail::SAP_Endpoint_Type::End)
      {
        // Pair our endpoints id with every other one in the active list.
        bool start_pairing = false;
        for(auto const& active_endpoint : active_list)
        {
          if(active_endpoint.id != endpt.id && start_pairing)
          {
            auto pair_aabb = active_endpoint.aabb;

            // Check collision in the y and z dimensions.
            if((endpt.aabb.min.y <= pair_aabb.min.y &&
                pair_aabb.min.y <= endpt.aabb.min.y + endpt.aabb.height) ||
               (endpt.aabb.min.y <= pair_aabb.min.y + pair_aabb.height &&
                pair_aabb.min.y + pair_aabb.height <=
                  endpt.aabb.min.y + endpt.aabb.height) ||
               (endpt.aabb.min.y <= pair_aabb.min.y &&
                pair_aabb.min.y + pair_aabb.height <=
                  endpt.aabb.min.y + endpt.aabb.height) ||
               (pair_aabb.min.y <= endpt.aabb.min.y &&
                endpt.aabb.min.y + endpt.aabb.height <=
                  pair_aabb.min.y + pair_aabb.height))
            {
              if((endpt.aabb.min.z <= pair_aabb.min.z &&
                  pair_aabb.min.z <= endpt.aabb.min.z + endpt.aabb.depth) ||
                 (endpt.aabb.min.z <= pair_aabb.min.z + pair_aabb.depth &&
                  pair_aabb.min.z + pair_aabb.depth <=
                    endpt.aabb.min.z + endpt.aabb.depth) ||
                 (endpt.aabb.min.z <= pair_aabb.min.z &&
                  pair_aabb.min.z + pair_aabb.depth <=
                    endpt.aabb.min.z + endpt.aabb.depth) ||
                 (pair_aabb.min.z <= endpt.aabb.min.z &&
                  endpt.aabb.min.z + endpt.aabb.depth <=
                    pair_aabb.min.z + pair_aabb.depth))
              {
                ret.emplace_back(endpt.id, active_endpoint.id);
              }
            }
          }
          else if(active_endpoint.id == endpt.id)
          {
            // Start pairing after encountering ourselves.
            start_pairing = true;
          }
        }
      }
    }

    return ret;
  }
} }
