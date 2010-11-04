//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAPREDUCE_SERIALIZATION_HPP_
#define MAPREDUCE_SERIALIZATION_HPP_

#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>

using google::protobuf::io::CodedInputStream;
using google::protobuf::io::CodedOutputStream;
using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::StringOutputStream;
using google::protobuf::uint8;
using google::protobuf::uint32;

namespace mapreduce {

// Serialization handler.
template <typename ClassT, class Enable = void> class SerializationHandler;

template <typename ClassT> class SerializationHandler<ClassT, bool> {
 public:
  inline static void Serialize(ClassT& object, ZeroCopyOutputStream* output) {
    ClassT& local = object;
    SerializationHandler<ClassT>::Serialize(&object, output);
  }
};

// Serialization support for google protobuf objects.
template <typename ClassT>
class SerializationHandler< ClassT,
  typename boost::enable_if<boost::is_base_of< ::google::protobuf::Message, ClassT> >::type > {
 public:
  inline static void Serialize(const ClassT* object,
                               ZeroCopyOutputStream* output) {
    object->SerializeToZeroCopyStream(output);
  }
  inline static void Deserialize(ZeroCopyInputStream* input, ClassT* object) {
    object->ParseFromZeroCopyStream(input);
  }
};


// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
// Return a mutable char* pointing to a string's internal buffer,
// which may not be null-terminated. Writing through this pointer will
// modify the string.
//
// string_as_array(&str)[i] is valid for 0 <= i < str.size() until the
// next call to a string method that invalidates iterators.
//
// As of 2006-04, there is no standard-blessed way of getting a
// mutable reference to a string's internal buffer. However, issue 530
// (http://www.open-std.org/JTC1/SC22/WG21/docs/lwg-active.html#530)
// proposes this as the method. According to Matt Austern, this should
// already work on all current implementations.
inline char* string_as_array(std::string* str) {
  // DO NOT USE const_cast<char*>(str->data())! See the unittest for why.
  return str->empty() ? NULL : &*str->begin();
}

// Serialization interface implemented by internal objects such as
// JobDescription.
class Serializable {
 public:
  virtual ~Serializable() {}
  virtual void Serialize(CodedOutputStream* output) const = 0;
  virtual void Deserialize(CodedInputStream* input) = 0;
};

// Serialization support for the Serializable interface.
template <typename ClassT>
class SerializationHandler< ClassT,
  typename boost::enable_if<boost::is_base_of<Serializable, ClassT> >::type > {
 public:
  inline static void Serialize(const Serializable* object,
                               ZeroCopyOutputStream* output) {
    CodedOutputStream stream(output);
    object->Serialize(&stream);
  }
  inline static void Deserialize(ZeroCopyInputStream* input, Serializable* object) {
    CodedInputStream stream(input);
    object->Deserialize(&stream);
  }
  static void kuka2() {}
};

//
// Serialization support for some standard types.
//
// std:string
template <>
class SerializationHandler<std::string> {
 public:
  inline static void Serialize(const std::string* object,
                               ZeroCopyOutputStream* output) {
    CodedOutputStream coded_out(output);
    coded_out.WriteVarint32(object->length());
    coded_out.WriteRaw(object->data(), object->length());
  }
  inline static void Deserialize(ZeroCopyInputStream* input,
                                 std::string* object) {
    CodedInputStream coded_in(input);
    uint32 size;
    coded_in.ReadVarint32(&size);
    coded_in.ReadString(object, size);
  }
};

// int
template <>
class SerializationHandler<int> {
 public:
  inline static void Serialize(const int* value, ZeroCopyOutputStream* output) {
    CodedOutputStream coded_out(output);
    coded_out.WriteVarint32SignExtended(*value);
  }
  inline static void Deserialize(ZeroCopyInputStream* input, int* value) {
    CodedInputStream coded_in(input);
    uint32 read_value;
    coded_in.ReadVarint32(&read_value);
    *(reinterpret_cast<uint32*>(value)) = read_value;
  }
};

} // namespace mapreduce

#endif  // MAPREDUCE_SERIALIZATION_HPP_
