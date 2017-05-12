/*file configuration.cpp
 *author
 *date
*/

#include <iostream>

#include "configuration.h"
Configuration* Configuration::instance = new Configuration();

Configuration::Configuration() {
  width_area = 800;
  width_area = 800;

  n_generation = 1000;
  n_sensor_per_indiv = 18;
}

void Configuration::ReadConfig(const char* filename) {
  std::ifstream fin(filename);
	std::string line;
	std::string::size_type sz;
	while (std::getline(fin, line)) {
		std::istringstream in(line);
		std::string option, val;
		in >> option >> val;
    if (option.compare("width") == 0) {
      width_area = std::atoi(val.c_str());
    } else if (option.compare("height") == 0) {
      height_area = std::atoi(val.c_str());
    } else if (option.compare("generator") == 0) {
      n_generation = std::atoi(val.c_str());
    } else if (option.compare("sensor_per_indiv") == 0) {
      n_sensor_per_indiv = std::atoi(val.c_str());
    } else if (option.compare("num_sensor_type") == 0) {
      n_sensor_type = std::atoi(val.c_str());
      for (unsigned i = 0; i < n_sensor_type; i++) {
        std::string val_i;
        in >> val_i;
        std::cout << std::atoi(val_i.c_str()) << std::endl;
        n_sensor_per_type.push_back(std::atoi(val_i.c_str()));
      }
    } else if (option.compare("num_parent") == 0) {
      n_parent = std::atoi(val.c_str());
    } else if (option.compare("max_population") == 0) {
      max_population_size = std::atoi(val.c_str());
    } else if (option.compare("blx_a") == 0) {
      blx_a = std::atof(val.c_str());
    }

  }
}
