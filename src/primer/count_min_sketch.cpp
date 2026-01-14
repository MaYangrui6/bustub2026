//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// count_min_sketch.cpp
//
// Identification: src/primer/count_min_sketch.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/count_min_sketch.h"

#include <stdexcept>
#include <string>

namespace bustub {

/**
 * Constructor for the count-min sketch.
 *
 * @param width The width of the sketch matrix.
 * @param depth The depth of the sketch matrix.
 * @throws std::invalid_argument if width or depth are zero.
 */
template <typename KeyType>
CountMinSketch<KeyType>::CountMinSketch(uint32_t width, uint32_t depth) : width_(width), depth_(depth) {
  if (width == 0 || depth == 0) {
    throw std::invalid_argument("Width and depth must be non-zero");
  }
  // Allocate memory for atomic array
  count_matrix_ = std::unique_ptr<std::atomic<uint32_t>[]>(new std::atomic<uint32_t>[width_ * depth_]);
  // Initialize each atomic element separately
  for (size_t i = 0; i < width_ * depth_; i++) {
    count_matrix_[i].store(0, std::memory_order_relaxed);
  }

  /** @spring2026 PLEASE DO NOT MODIFY THE FOLLOWING */
  // Initialize seeded hash functions
  hash_functions_.reserve(depth_);
  for (size_t i = 0; i < depth_; i++) {
    hash_functions_.push_back(this->HashFunction(i));
  }
}

template <typename KeyType>
CountMinSketch<KeyType>::CountMinSketch(CountMinSketch &&other) noexcept
    : width_(other.width_),
      depth_(other.depth_),
      hash_functions_(std::move(other.hash_functions_)),
      count_matrix_(std::move(other.count_matrix_)) {
  // Reset other's state
  other.width_ = 0;
  other.depth_ = 0;
}

template <typename KeyType>
auto CountMinSketch<KeyType>::operator=(CountMinSketch &&other) noexcept -> CountMinSketch & {
  if (this != &other) {
    // Transfer resources from other
    width_ = other.width_;
    depth_ = other.depth_;
    count_matrix_ = std::move(other.count_matrix_);
    hash_functions_ = std::move(other.hash_functions_);

    // Reset other's state
    other.width_ = 0;
    other.depth_ = 0;
  }
  return *this;
}

template <typename KeyType>
void CountMinSketch<KeyType>::Insert(const KeyType &item) {
  /** @TODO(student) Implement this function! */
  for (size_t i = 0; i < depth_; ++i) {
    size_t pos = hash_functions_[i](item) % width_;
    count_matrix_[i * width_ + pos].fetch_add(1, std::memory_order_relaxed);
  }
}

template <typename KeyType>
void CountMinSketch<KeyType>::Merge(const CountMinSketch<KeyType> &other) {
  if (width_ != other.width_ || depth_ != other.depth_) {
    throw std::invalid_argument("Incompatible CountMinSketch dimensions for merge.");
  }
  /** @TODO(student) Implement this function! */
  for (size_t i = 0; i < width_ * depth_; i++) {
    size_t other_count = other.count_matrix_[i].load(std::memory_order_relaxed);
    if (other_count > 0) {
      count_matrix_[i].fetch_add(other_count, std::memory_order_relaxed);
    }
  }
}

template <typename KeyType>
auto CountMinSketch<KeyType>::Count(const KeyType &item) const -> uint32_t {
  size_t min_count = UINT32_MAX;
  for (size_t i = 0; i < depth_; ++i) {
    size_t current_count =
        count_matrix_[i * width_ + (hash_functions_[i](item) % width_)].load(std::memory_order_relaxed);
    if (current_count < min_count) {
      min_count = current_count;
    }
  }
  return min_count;
}

template <typename KeyType>
void CountMinSketch<KeyType>::Clear() {
  /** @TODO(student) Implement this function! */
  for (size_t i = 0; i < width_ * depth_; i++) {
    count_matrix_[i].store(0, std::memory_order_relaxed);
  }
}

template <typename KeyType>
auto CountMinSketch<KeyType>::TopK(uint16_t k, const std::vector<KeyType> &candidates)
    -> std::vector<std::pair<KeyType, uint32_t>> {
  // Directly use vector to store key-count pairs
  std::vector<std::pair<KeyType, uint32_t>> count_vector;
  count_vector.reserve(candidates.size());

  for (const auto &item : candidates) {
    count_vector.emplace_back(item, Count(item));
  }

  // Sort by count in descending order
  std::sort(count_vector.begin(), count_vector.end(), [](const auto &a, const auto &b) { return a.second > b.second; });

  // Take top k elements
  if (count_vector.size() > k) {
    count_vector.resize(k);
  }

  return count_vector;
}

// Explicit instantiations for all types used in tests
template class CountMinSketch<std::string>;
template class CountMinSketch<int64_t>;  // For int64_t tests
template class CountMinSketch<int>;      // This covers both int and int32_t
}  // namespace bustub
