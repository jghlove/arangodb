//
// IResearch search engine 
// 
// Copyright (c) 2016 by EMC Corporation, All Rights Reserved
// 
// This software contains the intellectual property of EMC Corporation or is licensed to
// EMC Corporation from third parties. Use of this software and the intellectual property
// contained therein is expressly limited to the terms and conditions of the License
// Agreement under which it is provided by or on behalf of EMC.
// 

#ifndef IRESEARCH_SEGMENT_READER_H
#define IRESEARCH_SEGMENT_READER_H

#include "index_reader.hpp"

NS_ROOT

////////////////////////////////////////////////////////////////////////////////
/// @brief interface for a segment reader
////////////////////////////////////////////////////////////////////////////////
class IRESEARCH_API segment_reader final : public sub_reader {
 public:
  typedef segment_reader element_type; // type same as self
  typedef segment_reader ptr; // pointer to self

  template<typename T>
  static bool has(const segment_meta& meta) NOEXCEPT;

  static segment_reader open(const directory& dir, const segment_meta& meta);

  segment_reader() = default; // required for context<segment_reader>
  segment_reader(const segment_reader& other);
  segment_reader(segment_reader&& other) NOEXCEPT;
  segment_reader& operator=(const segment_reader& other);
  segment_reader& operator=(segment_reader&& other) NOEXCEPT;

  explicit operator bool() const NOEXCEPT { return bool(impl_); }

  segment_reader& operator*() NOEXCEPT { return *this; }
  const segment_reader& operator*() const NOEXCEPT { return *this; }
  segment_reader* operator->() NOEXCEPT { return this; }
  const segment_reader* operator->() const NOEXCEPT { return this; }

  virtual index_reader::reader_iterator begin() const override;

  virtual index_reader::reader_iterator end() const override {
    return impl_->end();
  }

  virtual const column_meta* column(const string_ref& name) const override {
    return impl_->column(name);
  }

  virtual column_iterator::ptr columns() const override {
    return impl_->columns();
  }

  using sub_reader::docs_count;
  virtual uint64_t docs_count() const override {
    return impl_->docs_count();
  }

  virtual docs_iterator_t::ptr docs_iterator() const override {
    return impl_->docs_iterator();
  }

  virtual const term_reader* field(const string_ref& name) const override {
    return impl_->field(name);
  }

  virtual field_iterator::ptr fields() const override {
    return impl_->fields();
  }

  virtual uint64_t live_docs_count() const override {
    return impl_->live_docs_count();
  }

  segment_reader reopen(const segment_meta& meta) const;

  void reset() NOEXCEPT {
    impl_.reset();
  }

  virtual size_t size() const override {
    return impl_->size();
  }

  using sub_reader::column_reader;
  virtual const columnstore_reader::column_reader* column_reader(
      field_id field) const override {
    return impl_->column_reader(field);
  }

 private:
  class atomic_helper;
  typedef std::shared_ptr<sub_reader> impl_ptr;

  IRESEARCH_API_PRIVATE_VARIABLES_BEGIN
  impl_ptr impl_;
  IRESEARCH_API_PRIVATE_VARIABLES_END

  segment_reader(impl_ptr&& impl) NOEXCEPT;
}; // segment_reade

template<>
/*static*/ IRESEARCH_API bool segment_reader::has<columnstore_reader>(
    const segment_meta& meta
) NOEXCEPT;

template<>
/*static*/ IRESEARCH_API bool segment_reader::has<document_mask_reader>(
    const segment_meta& meta
) NOEXCEPT;

NS_END

#endif
