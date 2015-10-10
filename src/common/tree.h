/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include <vector>
#include <cmath>

namespace game
{
  template <class T>
  struct Node;

  /*!
   * \brief A tree structure stored in breadth-first search order in memory.
   *
   * \tparam N The fixed amount of children for each new depth level. AKA for a
   * Quadtree N should be 4.
   */
  template <class T, unsigned int N>
  struct Tree
  {
    //! The type of node.
    using node_t = Node<T>;

  private:
    //! The vector type used to internally store the nodes contigously.
    using node_vector_t = std::vector<node_t>;

  public:
    //! Plain-Jane vector::iterator.
    using iterator_t = typename node_vector_t::iterator;
    //! Plain-Jane vector::const_iterator.
    using const_iterator_t = typename node_vector_t::const_iterator;

    //! The type used for referencing the depth of the tree.
    using depth_t = std::size_t;

    /*!
     * \brief Resize the tree.
     *
     * \notes Each increase in depth adds to memory
     * usage by N^depth (from the previous depth level). It will incur a
     * reallocation most likely.

     * \param depth is like vector.resize in that it is not zero-based.
     */
    void set_depth(depth_t depth) noexcept;

    /*!
     * \brief Query the fixed depth of the tree.
     */
    depth_t get_depth() const noexcept { return depth_; }

    /*!
     * \brief Returns an iterator to the first element of the depth given.
     *
     * \param Zero-based depth value. Ex: 0 is the very first, root element.
     * \returns The iterator straight from the vector.
     */
    iterator_t level_begin(depth_t depth) noexcept;

    const_iterator_t level_begin(depth_t depth) const noexcept;


    /*!
     * \brief Returns an iterator to the next-to-last element of the depth
     * given.
     *
     * Returns the corresponding iterator to \sa level_begin with the same
     * depth. Can also be called with depths greater than what was used in the
     * level begin call to get an iterator pair spanning multiple levels.
     *
     * \param Zero-based depth.
     * \returns The iterator straight from the vector.
     */
    iterator_t level_end(depth_t depth) noexcept;

    const_iterator_t level_end(depth_t depth) const noexcept;

    //! Returns an iterator to the first node of the first depth level.
    iterator_t begin() noexcept { return nodes_.begin(); }

    //! Returns a const iterator to the first node of the first depth level.
    const_iterator_t begin() const noexcept { return nodes_.begin(); }

    //! Returns an iterator to the last node of the last depth level.
    iterator_t end() noexcept { return nodes_.end(); }

    //! Returns a const iterator to the last node of the last depth level.
    const_iterator_t end() const noexcept { return nodes_.end(); }

    //! Returns the node at a certain depth + index.
    node_t& node_at_depth(std::size_t depth, std::size_t i) noexcept
    { return *(level_begin(depth) + i); }

    //! Returns the node at a given index from the start of block of nodes.
    node_t& node_at_index(std::size_t i) noexcept
    { return nodes_[i]; }
  private:
    // Dynamically sized contigous block of nodes. This is kind of hefty if
    // there is a lot of resizing, but the intended usage of this class is
    // for a somewhat statically sized quadtree (as varying depths on different
    // paths down the tree is not implementated, that may be obvious).

    //! Contigious block of nodes stored in breadth-first order.
    node_vector_t nodes_;

    // Technically this could be calculated from the size of nodes, but ehhh.
    // Keeping it cached is all around better, as it could only really ever
    // change from calls to set_depth in which case saving the input makes
    // complete sense.

    //! Amount of depth levels (not zero-based).
    depth_t depth_ = 0;
  };

  //! The node stores the content and current depth level.
  template <class T>
  struct Node
  {
    std::size_t depth() const noexcept { return depth_; }

    T val;
  private:
    std::size_t depth_ = 0;

    template <class, unsigned int> friend struct Tree;
  };


  // Depth is not zero based, but level is.
  unsigned int tree_amount_nodes(unsigned int N, std::size_t depth) noexcept;
  std::size_t tree_level_offset(unsigned int N, std::size_t level) noexcept;

  template <class T, unsigned int N>
  void Tree<T, N>::set_depth(depth_t depth) noexcept
  {
    auto req_nodes = tree_amount_nodes(N, depth);

    auto orig_depth = depth_;
    nodes_.resize(req_nodes);
    depth_ = depth;

    // Only set the depth for every *new* node. This loop goes from any new
    // depth levels (orig_depth + 1) to the new depth level.
    // Resize 5 => 6 : Loop runs with i == 6
    // Resize 6 => 8 : Loop runs with i == 7 and i == 8
    // These are obviously not zero-based values.
    for(depth_t i = orig_depth + 1; i <= depth; ++i)
    {
      // Get the zero-based depth-level to query iterators.
      auto level = i - 1;

      // It's important to preserve depth information in this loop, otherwise
      // we could just use a single loop.
      for(auto iter = level_begin(level); iter != level_end(level); ++iter)
      {
        iter->depth_ = level;
      }
    }
  }

  // Make sure to update the const implementation if this one ever changes. I'm
  // not really sure how to implement them in terms of each other since
  // iterator_t and const_iterator_t aren't our types.
  template <class T, unsigned int N>
  typename Tree<T, N>::iterator_t
  Tree<T, N>::level_begin(depth_t depth) noexcept
  {
    // If the user requests a depth level that we don't have, return our end
    // iterator.

    // Note that our depth is not zero-based while the parameter is zero-based.
    // By using less-than-or-equal-to we make sense of it all.

    if(this->depth_ <= depth)
    {
      // If have 2 depth levels and the user requests depth level 2 we have a
      // problem so return our end iterator.
      return this->end();
    }

    // Get the offset of the given depth, since we know it's valid and in out
    // range.
    auto offset = tree_level_offset(N, depth);

    // Return that iterator.
    return nodes_.begin() + offset;
  }

  // Identical implementation to the one above, so look at those comments.
  template <class T, unsigned int N>
  typename Tree<T, N>::const_iterator_t
  Tree<T, N>::level_begin(depth_t depth) const noexcept
  {
    if(this->depth_ <= depth)
    {
      return this->end();
    }
    auto offset = tree_level_offset(N, depth);
    return nodes_.begin() + tree_level_offset(N, depth);
  }

  template <class T, unsigned int N>
  typename Tree<T, N>::iterator_t
  Tree<T, N>::level_end(depth_t depth) noexcept
  {
    // Return the begin iterator to the next depth level. This works because
    // if we go out of our valid depth levels, the iterator to the end of the
    // nodes_ vector will be returned which was exactly what we wanted anyway.
    return level_begin(depth+1);
  }
  // Identical implementation to the previous function exception const.
  template <class T, unsigned int N>
  typename Tree<T, N>::const_iterator_t
  Tree<T, N>::level_end(depth_t depth) const noexcept
  {
    return level_begin(depth+1);
  }

  template <class T>
  using Quadtree = Tree<T, 4>;
}
