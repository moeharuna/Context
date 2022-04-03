#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <variant>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cassert>
#include <typeinfo>
#include <stdio.h>
#include "context.hpp"
#ifdef __GNUC__
#include <cxxabi.h>
#endif

Context::Context() : value(Context::map()) {}
Context::Context(int scalar) : value(scalar) {}
Context::Context(double scalar) : value (scalar) {}
Context::Context(bool scalar) : value (scalar) {}
Context::Context(const std::string& scalar) : value(scalar) {}
Context::Context(std::string&& scalar) : value (scalar) {}
Context::Context(std::vector<Context>&& array) : value(array) {}
Context::Context(std::map<std::string, Context> &&map) : value(map) {}

Context::Context(std::initializer_list<Context> list)
{
  if(list.size()==1) this->value = list.begin()->value;
  else {
    std::vector<Context> inner;
    inner.insert(inner.end(), list.begin(), list.end());
    this->value = inner;
  }
}

void check_scalar_type(int) {};
void check_scalar_type(bool) {};
void check_scalar_type(double){};
void check_scalar_type(const std::string&){};   //i use these to error out in compile time when possible

template <typename T>
Context::Value Context::value_from_init_list(std::initializer_list<T> list) {
  
  if(list.size()==0) return array();
  check_scalar_type(*list.begin());
  if(list.size()==1) return (*list.begin());
  else {
    auto ar = array();
    for(const auto &val:list) {
      ar.push_back(Context(val));
    }
    return ar;
  }
}


Context::Context(std::initializer_list<int> list) {
  this->value = value_from_init_list<int>(list);
}
Context::Context(std::initializer_list<double> list) {
  this->value = value_from_init_list<double>(list);
}
Context::Context(std::initializer_list<bool> list) {
  this->value = value_from_init_list<bool>(list);
}
Context::Context(std::initializer_list<std::string> list) {
  this->value = value_from_init_list<std::string>(list);
}

Context::Context(std::initializer_list<const char *> list) {
  if(list.size()==0) this->value =  array();
  if(list.size()==1) this->value =  (*list.begin());
  else {
    auto ar = array();
    for(const auto &val:list) {
      ar.push_back(Context(val));
    }
    this->value = ar;
  }
}

template <typename T>
T* Context::unwrap() {
  if (auto result = std::get_if<T>(&this->value)) {
    return result;
  }
  else {
    throw ContextBadType(context_type(), typeid(T));
  }
}
template <typename T>
const T* Context::unwrap()  const{
  return this->unwrap<T>();
}

template <typename T>
T& Context::get()
{
  try {
    return std::get<T>(value);
  } catch(std::bad_variant_access e) {
    throw ContextBadType(context_type(), typeid(T));
  }
}
template <typename T>
const T& Context::get() const
{
  return get<T>();
}



int& Context::getInt(){ return get<int>();}
const int &Context::getInt() const { return getInt(); }

double& Context::getDouble() { return get<double>(); }
const double& Context::getDouble() const {return getDouble();}
    
bool& Context::getBool() { return get<bool>(); };
const bool& Context::getBool() const { return this->getBool(); }

std::string &Context::getString() { return get<std::string>(); }

const std::string &Context::getString() const { return getString(); }


Context& Context::operator[](const std::string &s) {
  return get<map>()[s];
}
Context& Context::operator[](std::string &&s) {
  return get<map>()[s];
}

Context &Context::operator[](size_t i) { return get<array>()[i]; }

const Context& Context::operator[](size_t i) const {
  return this[i];
}


Context& Context::at(size_t i) {
  return get<array>().at(i);
}
const Context &Context::at(size_t i) const { return at(i); }


const Context& Context::at(const std::string& s) const {
  return get<map>().at(s);
}


void Context::insert(array::iterator place, const Context& v)
{
  auto arr = this->unwrap<array>();
  arr->insert(place, v);
}

void Context::insert(array::iterator place, Context&& v)
{
  auto arr = unwrap<array>();
  arr->insert(place, v);
}

void Context::push_back(const Context &v) {
  auto arr = unwrap<array>();
  arr->push_back(v);
}

void Context::push_back(Context&& v)
{
  auto arr = unwrap<array>();
  arr->push_back(v);
}

const std::size_t Context::size() const {
  if(const auto* map = std::get_if<Context::map>(&value))
    return map->size();
  else if(const auto* array = std::get_if<Context::array>(&value))
    return array->size();
  else
    throw ContextBadType();
}

Context::map::iterator Context::begin() {
  auto map = unwrap<Context::map>();
  return map->begin();
}
Context::map::iterator Context::end() {
  auto map = unwrap<Context::map>();
  return map->end();
}

Context::array::iterator Context::arr_begin() {
  auto array = unwrap<Context::array>();
  return array->begin();
}
Context::array::iterator Context::arr_end() {
  auto array = unwrap<Context::array>();
  return array->end();
}

const std::type_info & Context::context_type() const{
  return std::visit( [](auto&&x)->decltype(auto){ return typeid(x); }, value );
}


#ifdef __GNUC__
void demangle(std::string& s) {
  int status;
  char *demangle = abi::__cxa_demangle(s.c_str(), NULL, NULL, &status);
  if(status == 0) {
    s = demangle;
    free(demangle);
  }
}
#endif

ContextBadType::ContextBadType() {
  this->m_error_text = "ContextBadType: You requested a wrong type";
}

ContextBadType::ContextBadType(const std::type_info& container_type, const std::type_info& requested_type) {
  std::string context_type_name = container_type.name();
  std::string required_type_name = requested_type.name();
#ifdef __GNUC__
  demangle(context_type_name);
  demangle(required_type_name);
#endif
  this->m_error_text =
    "ContextBadType: You requested type was `" +
    required_type_name + "` but context has `" +
    context_type_name + "` inside. ";
}

const char *ContextBadType::what() const noexcept {
  return this->m_error_text.c_str();
}


