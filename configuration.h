/*file configuration.h
 *author
 *date
*/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class Configuration {
 public:
  Configuration();
	~Configuration();

	static Configuration* GetInstance() { 
		if (!instance) {
      instance = new Configuration();
    }
		return instance; 
	};

  void ReadConfig(const char* filename);


 public:
  unsigned width_area;
  unsigned height_area;

  unsigned n_generation;
  unsigned n_sensor_per_indiv;
  unsigned n_sensor_type;
  std::vector<unsigned> n_sensor_per_type;

  unsigned n_parent;
  unsigned max_population_size;
  float blx_a;
  
 private:
	static Configuration* instance;
};

#endif
