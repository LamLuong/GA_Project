/*file population.h
 *author
 *date
*/

#ifndef POPULATION_H
#define POPULATION_H

#include "individual.h"

#define N_PARENT 25
#define MAX_SIZE_POPULATION 50


class Population {
 public:
  Population();
  ~Population();

  void InitPopulation();

 private:
  bool CompareIdx(const uint list_idx1[], const uint list_idx2[], uint size);
 private:
  std::vector<Individual>list_individual_;
};

Population::Population() {
  std::vector<std::string> array_type_sensor = {"R1", "R1", "R1", "R2", "R2", "R2", "R2", "R2", "R2",
                                                "R3", "R3", "R3", "R3", "R3", "R3", "R3", "R3", "R3" };
  for (uint i = 0; i < N_PARENT; i++) {
    Individual individual_tmp(array_type_sensor);
    if (list_individual_.size() == 0) {
      individual_tmp.SetRandomIdx();
      individual_tmp.InitSensor();

      list_individual_.push_back(individual_tmp);
    } else {
      bool is_valid_idx = false;
      while (!is_valid_idx) {
        individual_tmp.SetRandomIdx();
        uint curr_index[TOTAL_SENSORS];
        individual_tmp.GetRandomIdx(curr_index);

        for (uint j = 0; j < list_individual_.size(); j++) {
          uint curr_index_tmp[TOTAL_SENSORS];
          list_individual_[i].GetRandomIdx(curr_index_tmp);
          if (!CompareIdx(curr_index, curr_index_tmp, TOTAL_SENSORS)) {
              is_valid_idx = true;
              break;
          }

        }
      }
      individual_tmp.InitSensor();
      list_individual_.push_back(individual_tmp);
    }
  }

  char filename[100] = "\0";
  for (uint i = 0; i < list_individual_.size(); i++) {
    list_individual_[i].CaclSensorsScore();
    std::cout << i << "  " << list_individual_[i].GetSensorsScore() << std::endl;
    sprintf(filename, "results/image_%d.jpg", i);
    list_individual_[i].DrawResult(filename);
  }
  
}

Population::~Population() {}

bool Population::CompareIdx(const uint list_idx1[], const uint list_idx2[], uint size) {
  for (uint i = 0; i < size; i++) {
    if (list_idx1[i] != list_idx2[i]) {
      return false;
    }
  }
  return true;
}

#endif // POPULATION_H
