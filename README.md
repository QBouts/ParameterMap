![Build and Test status](https://github.com/qbouts/ParameterMap/workflows/Build%20and%20Test/badge.svg)

# ParameterMap
Contains code for working with parameter maps. A parameter map be thought of as a set of {name, value} pairs.
The ParameterMap class provided by [ParameterMap.h](include/ParameterMap.h) allows users to build up a parameter map
and submit the stored parameters to a function.

# Example
Examples are also included in the [examples](examples) directory.
## Basic usage

```c++
#include <iostream>
#include <string>

#include "ParameterMap.h"
using qbouts::ParameterMap;

void process_person(const std::string &name, int age) {
  std::cout << "Processing \"" << name << "\" (age: " 
            << std::to_string(age) << ")\n";
}

int main() {
  ParameterMap<const std::string, int> params{"name", "age"};

  params.set("name", "Homer Simpson");
  if (!params.is_set("age")) {
    std::cout << "Age parameter has not been set.\n";
  }
  params.set("age", 35);
  params.submit(&process_person);

  params.set("age", 38);
  std::cout << "Age parameter has been updated to: "  
            << std::to_string(params.get<int>("age")) << ".\n";
}
```

## A more realistic use case
I initially created the ParameterMap class when I was writing some (de)serialization code for a different project.
For this project I had to call several factory functions with parameters retrieved from xml files. Furthermore, I had
the requirement that it should be possible to specify default values for parameters. 

As an example of such a usage, take the fictitious factory function below, which creates textures
```c++
  std::unique_ptr<Texture> create_texture(
                             const std::string &path, 
                             double size_percent, 
                             bool flip);
```

The defaults file for a texture 
might look somewhat like this
```xml
<!-- defaults.xml -->
<size_percent type="double">100</size_percent>
<flip type="bool">false</flip>
```
Different textures could then be specified as follows
```xml
<!-- texture.xml -->
<tree>
  <path type="string">tree.png</path>
</tree>
<car>
  <path type="string">car.png</path>
  <size_percent type="double">56.5</size_percent>
</car>
<brick>
  <path type="string">house.png</path>
  <flip type="bool">true</flip>
</brick>
```

Using ParameterMaps it becomes much easier to compose the function calls to create_texture with the correct arguments. 
You can simply use one ParameterMap to keep track of default values and a second map to gather the parameters of a 
specific texture as the xml file is being parsed. 

Before submitting the parameter to the create_texture function, 
you loop over the map and fill in defaults if no parameters where set. The (non compiled / pseudo) code might look something like this

```c++
auto read_params_from_xml(const XMLNode &node){
  ParameterMap<const std::string&, double, bool> 
    params{"path", "size_percent", "flip"};
  
  for(const auto &property: node){
    if(property.type == "string"){
      params.set(property.get_name(), property.value_as<std::string>());
    }
    if(property.type == "double"){
      params.set(property.get_name(), property.value_as<double>());
    }
    if(property.type == "bool"){
      params.set(property.get_name(), property.value_as<bool>());
    }
  }
  return params;
}

std::unique_ptr<Texture> create_from_xml(const std::string name){
  // Read parameters from configuration
  XMLReader defaults_xml("defaults.xml");
  auto defaults = read_params_from_xml(defaults_xml.get_root());
  
  XMLReader textures_xml("textures.xml");
  auto params = read_params_from_xml(textures_xml.get(name));

  // apply defaults for any missing parameters
  for(size_t i=0;i<3;i++){
    if(!params.is_set(i)){
      params.set(i, defaults.get(i));
    }
  }

  // submit parameters to function
  return params.submit(&create_texture);
}
```

# Compilation requirements
To compile the code provided in this repository you need a C++17 compatible compiler which supports C++20 concepts. 
The code has been verified to compile successfully on Debian Linux using 
* gcc 9.3 using the -std=c++17 -fconcepts flags
* clang 10.0.1 using the -std=c++2a flag

## Compilation of tests and examples
To compile the tests both CMake and GoogleTest (gtest) need to be installed.
From the root directory of the repository you can compile using
```
$ mkdir build
$ cmake . -B build 
$ cd build && make
```

# More documentation
Documentation is provided in the form of doxygen comments. The text below is a copy comment at the top of the ParameterMap class. Please look at the source code for further documentation on the specific members of the class.

## Creating a ParameterMap
The ParameterMap is constructed with a set of parameter types supplied as template arguments and names for each parameter.
Once constructed parameters can be stored as well as retrieved using set and get functions.
Parameters can be identified using the names supplied upon constructing the map as well as using their index.

## Calling a function with the stored parameters
Using the submit member function the parameters can be 'submitted' to a supplied function: The function will be
called with the stored parameters.

## Performance
Care has been taken to avoid making unnecessary copies of parameters or string comparisons.
When using a ParameterMap in a performance sensitive part of your code be aware of the following:
- Any operations where parameters are identified by their name (set, get, is_set) will compute an std::hash of the 
  given name, which takes linear time in the length of the name. Where possible, prefer the use of 
  the index based variants of these functions or build the parameter map outside of the performance critical section
  of your code.
- For most functions the overhead of using submit compared to calling the function directly will be negligible.
  There is however one small exception to be aware of: function calls made using submit do not benefit from move
  semantics. To be specific: For functions that accept parameters by rvalue reference (e.g. int&&), a copy of the
  stored parameter will be made. This is required as the parameter map may not be modified by the submit call.

