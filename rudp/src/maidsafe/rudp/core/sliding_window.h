/*******************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of MaidSafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of MaidSafe.net. *
 ******************************************************************************/
// Original author: Christopher M. Kohlhoff (chris at kohlhoff dot com)

#ifndef MAIDSAFE_RUDP_CORE_SLIDING_WINDOW_H_
#define MAIDSAFE_RUDP_CORE_SLIDING_WINDOW_H_

#include <cstdint>
#include <cassert>
#include <deque>

#include "maidsafe/common/utils.h"

#include "maidsafe/rudp/parameters.h"

namespace maidsafe {

namespace rudp {

namespace detail {

template <typename T>
class SlidingWindow {
 public:
  // The maximum possible sequence number. When reached, sequence numbers are
  // wrapped around to start from 0.
  enum { kMaxSequenceNumber = 0x7fffffff };

  // Construct to start with a random sequence number.
  SlidingWindow() : items_(), maximum_size_(0), begin_(0), end_(0) {
    Reset(GenerateSequenceNumber());
  }

  // Construct to start with a specified sequence number.
  explicit SlidingWindow(uint32_t initial_sequence_number)
      : items_(),
        maximum_size_(Parameters::default_window_size),
        begin_(initial_sequence_number),
        end_(initial_sequence_number) {}

  // Reset to empty starting with the specified sequence number.
  void Reset(uint32_t initial_sequence_number) {
    assert(initial_sequence_number <= kMaxSequenceNumber);
    maximum_size_ = Parameters::default_window_size;
    begin_ = end_ = initial_sequence_number;
    items_.clear();
  }

  // Get the sequence number of the first item in window.
  uint32_t Begin() const {
    return begin_;
  }

  // Get the one-past-the-end sequence number.
  uint32_t End() const {
    return end_;
  }

  // Determine whether a sequence number is in the window.
  bool Contains(uint32_t n) const {
    return IsInRange(begin_, end_, n);
  }

  // Determine whether a sequence number is within one window size past the
  // end. This is used to filter out packets with non-sensical sequence numbers.
  bool IsComingSoon(uint32_t n) const {
    uint32_t begin = end_;
    uint32_t end = (begin + Parameters::maximum_window_size) %
                   (static_cast<uint32_t>(kMaxSequenceNumber) + 1);
    return IsInRange(begin, end, n);
  }

  // Get the maximum size of the window.
  size_t MaximumSize() const {
    return maximum_size_;
  }

  // Set the maximum size of the window.
  void SetMaximumSize(size_t size) {
    maximum_size_ = size < Parameters::maximum_window_size
                    ? size : Parameters::maximum_window_size;
  }

  // Get the current size of the window.
  size_t Size() const {
    return items_.size();
  }

  // Get whether the window is empty.
  bool IsEmpty() const {
    return items_.empty();
  }

  // Get whether the window is full.
  bool IsFull() const {
    return items_.size() >= maximum_size_;
  }

  // Add a new item to the end.
  // Precondition: !IsFull().
  uint32_t Append() {
    assert(!IsFull());
    items_.push_back(T());
    uint32_t n = end_;
    end_ = Next(end_);
    return n;
  }

  // Remove the first item from the window.
  // Precondition: !IsEmpty().
  void Remove() {
    assert(!IsEmpty());
    items_.erase(items_.begin());
    begin_ = Next(begin_);
  }

  // Get the item with the specified sequence number.
  // Precondition: Contains(n).
  T &operator[](uint32_t n) {
    return items_[SequenceNumberToIndex(n)];
  }

  // Get the item with the specified sequence number.
  // Precondition: Contains(n).
  const T &operator[](uint32_t n) const {
    return items_[SequenceNumberToIndex(n)];
  }

  // Get the element at the front of the window.
  // Precondition: !IsEmpty().
  T &Front() {
    return items_.front();
  }

  // Get the element at the front of the window.
  // Precondition: !IsEmpty().
  const T &Front() const {
    return items_.front();
  }

  // Get the element at the back of the window.
  // Precondition: !IsEmpty().
  T &Back() {
    assert(!IsEmpty());
    return items_.front();
  }

  // Get the element at the back of the window.
  // Precondition: !IsEmpty().
  const T &Back() const {
    assert(!IsEmpty());
    return items_.back();
  }

  // Get the sequence number that follows a given number.
  static uint32_t Next(uint32_t n) {
    return (n == kMaxSequenceNumber) ? 0 : n + 1;
  }

 private:
  // Disallow copying and assignment.
  SlidingWindow(const SlidingWindow&);
  SlidingWindow &operator=(const SlidingWindow&);

  // Helper function to convert a sequence number into an index in the window.
  size_t SequenceNumberToIndex(uint32_t n) const {
    assert(Contains(n));
    if (begin_ <= end_)
      return n - begin_;
    else if (n < end_)
      return kMaxSequenceNumber - begin_ + n + 1;
    else
      return n - begin_;
  }

  // Helper function to generate an initial sequence number.
  static uint32_t GenerateSequenceNumber() {
    uint32_t seqnum = 0;
    while (seqnum == 0)
      seqnum = (RandomUint32() & 0x7fffffff);
    return seqnum;
  }

  // Helper function to determine if a sequence number is in a given range.
  static bool IsInRange(uint32_t begin, uint32_t end, uint32_t n) {
    if (begin <= end)
      return (begin <= n) && (n < end);
    else
      return (n < end) || ((n >= begin) && (n <= kMaxSequenceNumber));
  }

  // The items in the window.
  std::deque<T> items_;

  // The maximum number of items allowed in the window.
  size_t maximum_size_;

  // The sequence number of the first item in window.
  uint32_t begin_;

  // The one-past-the-end sequence number for the window. Will be used as the
  // sequence number of the next item added.
  uint32_t end_;
};

}  // namespace detail

}  // namespace rudp

}  // namespace maidsafe

#endif  // MAIDSAFE_RUDP_CORE_SLIDING_WINDOW_H_
