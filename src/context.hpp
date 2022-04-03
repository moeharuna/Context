#include <cstddef> // size_t
#include <initializer_list>
#include <typeinfo>
#include <vector>
#include <map>
#include <string>
#include <variant>
#include <iterator>

class ContextBadType : std::exception {
  std::string m_error_text;
public:
  const char* what() const noexcept;
  ContextBadType();
  ContextBadType(const std::type_info& context_type, const std::type_info& requested_type);
};   

class Context
{
public:
  typedef std::vector<Context> array;
  typedef std::map<std::string, Context> map;
private:
  typedef std::variant<int, bool, double, std::string, map, array> Value;
  Value value;

  template <typename T>
  T* unwrap();
  template <typename T>
  const T* unwrap() const;
  
  template <typename T>
  static Value value_from_init_list(std::initializer_list<T>);
public:  
  //empty map by default
  Context();
  Context(const Context&) = default;
  
  Context(int);
  Context(double);
  Context(bool);
  Context(const std::string&);
  Context(std::string&&);
  Context(array&&);
  Context(map&&);

  //Context(T) if only 1 value inside list.
  //Otherwise creates Context(array<T>) instead(even if there are 0 values)
  Context(std::initializer_list<Context>);
  Context(std::initializer_list<map>) = delete; // use initializer_list<Context>
  Context(std::initializer_list<array>) = delete;
  template <class T>
  Context(std::initializer_list<T>);

  Context(std::initializer_list<int>);
  Context(std::initializer_list<double>);
  Context(std::initializer_list<bool>);
  Context(std::initializer_list<std::string>);
  Context(std::initializer_list<const char*>);
    
  int& getInt();
  const int& getInt() const;
  
  double& getDouble();
  const double& getDouble() const;
    
  bool& getBool();
  const bool& getBool() const;

  std::string& getString();
  const std::string& getString() const;

  template <typename T>
  T& get();
  template <typename T>
  const T& get() const;
  
  Context& operator[](const std::string &);
  Context& operator[](std::string &&);
  Context& operator[](size_t);
  const Context& operator[](size_t) const;

  const Context& at(size_t) const;
  Context& at(size_t i);
  const Context& at(const std::string&) const;

  const std::size_t size() const;
  
  void insert(array::iterator place, const Context& value);
  void insert(array::iterator place, Context&& value);
  
  void push_back(Context&&);
  void push_back(const Context&);

  map::iterator begin();
  map::iterator end();
  const map::iterator begin() const;
  const map::iterator end() const;

  array::iterator arr_begin();
  array::iterator arr_end();

  const array::iterator arr_begin() const;
  const array::iterator arr_end() const;
  
  const std::type_info & context_type() const;
};
