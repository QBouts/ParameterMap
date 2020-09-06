#include <iostream>
#include <string>

#include "ParameterMap.h"

void process_person(const std::string &name, int age) {
	std::cout << "Processing \"" << name << "\" (age: " << std::to_string(age) << ")\n";
}

int main() {
	qbouts::ParameterMap<const std::string, int> params{"name", "age"};

	params.set("name", "Homer Simpson");
	if (!params.is_set("age")) {
		std::cout << "Age parameter has not been set.\n";
	}
	params.set("age", 35);
	params.submit(&process_person);

	params.set("age", 38);
	std::cout << "Age parameter has been updated to: " << std::to_string(params.get<int>("age")) << ".\n";
}