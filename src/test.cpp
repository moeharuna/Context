#include "context.hpp"
#include <vector>
#include <iostream>
#include <memory>
#include <map>
#include <algorithm>
void context_map()
{
  Context map = Context();
  map["a"] = {1};
  map["d"] = {3};
  map["b"] = {2};

  //range-for only works for maps
  for(auto[key, value] : map) {
    std::cout<<key<<" "<<value.getInt()<<"\n";
  }
}

void wrong_type_exception()
{
  Context cxt = Context{1};
  try {
    cxt.getBool();
  } catch(ContextBadType e) {
    std::cerr<<e.what()<<"\n";
  }
}

void array_context()
{
  Context cxt = {1, 2, 3, 4, 5};
  for(int i=1; i<cxt.size(); ++i) {
    std::cout<<cxt.at(i).getInt()<<" ";
  }
  std::cout<<"\n";
}

void staring_context()
{
  Context cxt = {"String test"};
  std::cout<<cxt.getString()<<"\n";
}

int main()
{
  context_map();
  array_context();
  staring_context();
  wrong_type_exception();  
}
