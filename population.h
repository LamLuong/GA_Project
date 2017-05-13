/*file population.h
 *author
 *date
*/

#ifndef POPULATION_H
#define POPULATION_H

#include <algorithm>
#include <set>
#include <cmath>

#include "individual.h"
#include "hungarian.h"
/*
#define N_PARENT 25
#define MAX_SIZE_POPULATION 50
#define BLX_ANPHA 0.5
#define GENERATION 1000
*/
const double EulerConstant = std::exp(1.0);

struct ComparingIndividual {
   inline bool operator()(const Individual& idv1, const Individual& idv2) const {
    return idv1.olap_area_square_ < idv2.olap_area_square_; 
  }
};

class Population {
 public:
  Population();
  ~Population();

  void InitPopulation();
  void HibridIndividual(const Individual& indv1, const Individual& indv2,
                        Individual& new_indv);

  void LoopHibrid();

 private:
  bool CompareIdx(const uint list_idx1[], const uint list_idx2[], uint size);
  void NormalIndividual(const Individual& indv1,
                        const Individual& indv2,
                        std::vector<std::pair<int, int> >&);
 private:
  std::vector<Individual>list_individual_;
  std::vector<std::string> array_type_sensor;
};

Population::Population() {
/*  array_type_sensor = {"R1", "R1", "R1",
                      "R2", "R2", "R2",
                      "R2", "R2", "R2",
                      "R3", "R3", "R3",
                      "R3", "R3", "R3",
                      "R3", "R3", "R3"};
*/
  uint total_sensors = Configuration::GetInstance()->n_sensor_per_indiv;
  for (uint i = 0; i < Configuration::GetInstance()->n_parent; i++) {
    Individual individual_tmp;
    if (list_individual_.size() == 0) {
      individual_tmp.SetRandomIdx();
      individual_tmp.InitSensor();

      list_individual_.push_back(individual_tmp);
    } else {
      bool is_valid_idx = false;
      while (!is_valid_idx) {
        individual_tmp.SetRandomIdx();
        uint* curr_index = new uint[total_sensors];
        individual_tmp.GetRandomIdx(curr_index);
        for (uint j = 0; j < list_individual_.size(); j++) {
          uint* curr_index_tmp = new uint[total_sensors];
          list_individual_[j].GetRandomIdx(curr_index_tmp);
          if (!CompareIdx(curr_index, curr_index_tmp, total_sensors)) {
              is_valid_idx = true;
              delete[] curr_index;
              delete[] curr_index_tmp;
              break;
          }
        }
        if (!is_valid_idx) {
          delete[] curr_index;
        }
      }
      individual_tmp.InitSensor();
      list_individual_.push_back(individual_tmp);
    }
  }
/*
  char filename[100] = "\0";
  for (uint i = 0; i < list_individual_.size(); i++) {
    list_individual_[i].CaclSensorsScore();
    std::cout << i << "  " << list_individual_[i].GetSensorsScore() << std::endl;
    sprintf(filename, "results/image_%d.jpg", i);
    list_individual_[i].DrawResult(filename);
  }
  */

}

Population::~Population() {}

bool Population::CompareIdx(const uint list_idx1[],
                            const uint list_idx2[],
                            uint size) {
  for (uint i = 0; i < size; i++) {
    if (list_idx1[i] != list_idx2[i]) {
      return false;
    }
  }
  return true;
}

void Population::NormalIndividual(const Individual& indv1,
                                  const Individual& indv2,
                                 std::vector<std::pair<int, int> >& _output_result
                                 ) {

//  std::vector<uint> idx_to_num = {3, 6, 9};
  uint offset_sensor = 0;

  for (uint num_type = 0; num_type < Configuration::GetInstance()->n_sensor_type; num_type++) {
    if (num_type > 0) {
      //offset_sensor += idx_to_num[num_type - 1];
      offset_sensor += Configuration::GetInstance()->n_sensor_per_type[num_type - 1].first;
    }

    uint num_sensor_in_type = Configuration::GetInstance()->n_sensor_per_type[num_type].first;
    int** cost_matrix = new int* [num_sensor_in_type];
    for (uint i = 0; i < num_sensor_in_type; i++) {
      cost_matrix[i] = new int [num_sensor_in_type];
    }

    for (uint i = 0; i < num_sensor_in_type; i++) {
      for (uint j = 0; j < num_sensor_in_type; j++) {
        cost_matrix[i][j] = (int)CaclSensorDistance(
                                 indv1.list_sensor[i + offset_sensor],
                                 indv2.list_sensor[j + offset_sensor]
                                 );
      }
    }

    Hungarian min_hungarian(num_sensor_in_type, (const int**)cost_matrix);
    Hungarian::Status status = min_hungarian.Solve();
    if (status != Hungarian::OK) {
      for (uint i = 0; i < num_sensor_in_type; i++) {
        delete [] cost_matrix[i];
      }
        delete [] cost_matrix;
      std::cout << "false " << std::endl;
      continue;
    }

    for (uint i = 0; i < num_sensor_in_type; i++) {
      uint y_match = min_hungarian.GetXMatch(i);
      _output_result.push_back({i + offset_sensor, y_match + offset_sensor});
    }

/*    for (unsigned k = 0; k < _output_result.size(); k++) {
      std::cout << _output_result[k].first << "  " << _output_result[k].second << std::endl;
    }

    std::cout << std::endl << std::endl;*/
    for (uint i = 0; i < num_sensor_in_type; i++) {
      delete [] cost_matrix[i];
    }
    delete [] cost_matrix;
  }
}

void Population::HibridIndividual(const Individual& indv1,
                                  const Individual& indv2,
                                  Individual& new_indv) {
  std::vector<std::pair<int, int> > output_result;
  NormalIndividual(indv1, indv2, output_result);
  float blx_a = Configuration::GetInstance()->blx_a;

  for (uint i = 0; i < output_result.size(); i++) {
    uint sensor1_pos = output_result[i].first;
    uint sensor2_pos = output_result[i].second;

    float distance_x = fabs(indv1.list_sensor[sensor1_pos].x
                            - indv2.list_sensor[sensor2_pos].x);

    float ontop_x = fmax(indv1.list_sensor[sensor1_pos].x, indv2.list_sensor[sensor2_pos].x)
                      + blx_a * distance_x;

    float botdown_x = fmin(indv1.list_sensor[sensor1_pos].x, indv2.list_sensor[sensor2_pos].x)
                      - blx_a * distance_x;

    new_indv.list_sensor[sensor1_pos].x = botdown_x + static_cast <float> (rand()) / 
                    static_cast <float> (RAND_MAX / (ontop_x - botdown_x));


    float distance_y = fabs(indv1.list_sensor[sensor1_pos].y
                            - indv2.list_sensor[sensor2_pos].y);

    float ontop_y = fmax(indv1.list_sensor[i].y, indv2.list_sensor[i].y)
                    + blx_a * distance_y;

    float botdown_y = fmin(indv1.list_sensor[sensor1_pos].y, indv2.list_sensor[sensor2_pos].y)
                    - blx_a * distance_y;

    new_indv.list_sensor[sensor1_pos].y = botdown_y + static_cast <float> (rand()) / 
                    static_cast <float> (RAND_MAX / (ontop_y - botdown_y));
  }
}

void Population::LoopHibrid() {
  std::vector<std::set<uint> > pair_hibrid;

  while (1) {
    uint p1 = 0, p2 = 0; 
    while (1) {
      p1 = rand() % Configuration::GetInstance()->n_parent;
      p2 = rand() % Configuration::GetInstance()->n_parent;
      if (p1 != p2) {
        break;
      }
    }

    std::set<uint> tmp_id = {p1, p2};
    if (pair_hibrid.size() == 0) {
      pair_hibrid.push_back(tmp_id);
    } else {
      bool is_valid = true;
      for (uint j = 0; j < pair_hibrid.size(); j++) {
        if (tmp_id == pair_hibrid[j]) {
          is_valid = false;
          break;
        }
      }
      if (is_valid) {
        pair_hibrid.push_back(tmp_id);
      } else {
        continue;
      }
    }

    Individual new_individual;
    HibridIndividual(list_individual_[p1], list_individual_[p2],
                     new_individual);
    new_individual.VFAProduce();
    list_individual_.push_back(new_individual);

    if (list_individual_.size() == Configuration::GetInstance()->max_population_size) {
      break;
    }
  }

  for (uint i = 0; i < list_individual_.size(); i++) {
    list_individual_[i].CaclSensorsScore();
  }

  std::sort(list_individual_.begin(), list_individual_.end(),
            ComparingIndividual());

  uint num_generator = 0;

  while (num_generator < Configuration::GetInstance()->n_generation) {
    pair_hibrid.clear();

    while (1) {
      uint p1 = 0, p2 = 0; 
      while (1) {
        p1 = rand() % Configuration::GetInstance()->max_population_size;
        p2 = rand() % Configuration::GetInstance()->max_population_size;
        if (p1 != p2) {
          break;
        }
      }

      std::set<uint> tmp_id = {p1, p2};
      if (pair_hibrid.size() == 0) {
        pair_hibrid.push_back(tmp_id);
      } else {
        bool is_valid = true;
        for (uint j = 0; j < pair_hibrid.size(); j++) {
          if (tmp_id == pair_hibrid[j]) {
            is_valid = false;
            break;
          }
        }
        if (is_valid) {
          pair_hibrid.push_back(tmp_id);
        } else {
          continue;
        }
      }

      Individual new_individual;
      HibridIndividual(list_individual_[p1], list_individual_[p2],
                       new_individual);
      new_individual.VFAProduce();
      list_individual_.push_back(new_individual);

      if (list_individual_.size() == 2 * Configuration::GetInstance()->max_population_size) {
        break;
      }
    }

    for (uint i = 0; i < list_individual_.size(); i++) {
      list_individual_[i].CaclSensorsScore();
    }

    std::sort(list_individual_.begin(), list_individual_.end(),
            ComparingIndividual());

    list_individual_.erase (list_individual_.begin() + Configuration::GetInstance()->max_population_size,
                            list_individual_.end());

    num_generator++;
    std::cout << num_generator << std::endl;
  }

  char filename[100] = "\0";
  for (uint i = 0; i < list_individual_.size(); i++) {
    sprintf(filename, "results/image_%d.jpg", i);
    list_individual_[i].DrawResult(filename);
  }
}


#endif // POPULATION_H
