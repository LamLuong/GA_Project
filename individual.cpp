#include "individual.h"

double CaclSensorDistance(const Sensor& sr1, const Sensor& sr2) {
  return sqrt(pow(sr1.x - sr2.x, 2) + pow(sr1.y - sr2.y, 2));
}


Individual::Individual() {
  uint total_sensors = Configuration::GetInstance()->n_sensor_per_indiv;
  list_sensor.resize(total_sensors);
  random_id.resize(total_sensors);
/*
  type_to_radius_converter["R1"] = 70;
  type_to_radius_converter["R2"] = 80;
  type_to_radius_converter["R3"] = 90;*/

  image_ = cv::Mat(Configuration::GetInstance()->width_area,
                   Configuration::GetInstance()->height_area,
                   CV_8UC3, cv::Scalar(255,255,255));
  olap_area_square_ = 0;

/*  for (uint i = 0; i < total_sensor; i++) {
      list_sensor[i].x = 0;
      list_sensor[i].y = 0;
      list_sensor[i].r = type_to_radius_converter[input_raw_sensor[i]];
  }
*/
  uint offset = 0;
  for (uint i = 0; i < Configuration::GetInstance()->n_sensor_type; i++) {
    if (i > 0) {
      offset += Configuration::GetInstance()->n_sensor_per_type[i - 1].first;
    }
    for (uint j = 0; j < Configuration::GetInstance()->n_sensor_per_type[i].first; j++) {
      list_sensor[j + offset].x = 0;
      list_sensor[j + offset].y = 0;
      list_sensor[j + offset].r = Configuration::GetInstance()->n_sensor_per_type[i].second;
    }
  }
}

Individual::~Individual() {
  list_sensor.clear();
  random_id.clear(); 
}

void Individual::SetRandomIdx() {

  std::vector<uint> list_idx;
  for (uint i = 0; i < Configuration::GetInstance()->n_sensor_per_indiv; i++) {
      list_idx.push_back(i);
  }

  uint iter = 0;
  while (list_idx.size() > 0) {
      uint foo = rand() % list_idx.size();
      random_id[iter] = list_idx[foo];
      list_idx.erase(list_idx.begin() + foo);
      iter++;
  }
}

void Individual::GetRandomIdx(uint input_list_idx[]) {
  memcpy(input_list_idx, random_id.data(),
         sizeof(uint) * Configuration::GetInstance()->n_sensor_per_indiv);
}


void Individual::DrawResult(const char* filename) {
  for (uint i = 0; i < Configuration::GetInstance()->n_sensor_per_indiv; i++) {
    cv::circle(image_, cv::Point2f(list_sensor[i].x, list_sensor[i].y), list_sensor[i].r,
           cv::Scalar( 0, 0, 255 ), 1, 8);
  }
  cv::imwrite(filename, image_);
}

void Individual::InitSensor() {
  float sensor_width_coverage_area = 0.f;
  float sensor_height_coverage_area = 0.f;
  float current_height_coverage_area = 0.f;
  uint height = Configuration::GetInstance()->height_area;
  uint width = Configuration::GetInstance()->width_area;

  for (uint i = 0; i < Configuration::GetInstance()->n_sensor_per_indiv; i++) {

    list_sensor[random_id[i]].x = list_sensor[random_id[i]].r +
                                  sensor_width_coverage_area;

    list_sensor[random_id[i]].y = list_sensor[random_id[i]].r +
                                  sensor_height_coverage_area;

    sensor_width_coverage_area = list_sensor[random_id[i]].x +
                                  list_sensor[random_id[i]].r;

    float sensor_height = list_sensor[random_id[i]].y + list_sensor[random_id[i]].r;

    if (current_height_coverage_area < sensor_height) {
      current_height_coverage_area = sensor_height;
    }

/*    if (list_sensor[random_id[i]].x > WIDTH) {
      list_sensor[random_id[i]].x = WIDTH;
    }
*/
    if (list_sensor[random_id[i]].y > height) {
      list_sensor[random_id[i]].y = height;
    }

    if (list_sensor[random_id[i]].y + list_sensor[random_id[i]].r > height) {
      list_sensor[random_id[i]].y = height - list_sensor[random_id[i]].r;
    }

    if (i < Configuration::GetInstance()->n_sensor_per_indiv - 1 &&
        sensor_width_coverage_area + 2 * list_sensor[random_id[i + 1]].r > width) {
      sensor_width_coverage_area = 0;
      sensor_height_coverage_area = current_height_coverage_area;
    }
  }
}

double Individual::CaclOlapSensor(const Sensor& sr1, const Sensor& sr2) {
  double center_distance = sqrt(pow(sr1.x - sr2.x, 2) + pow(sr1.y - sr2.y, 2));

  if (center_distance >= (sr1.r + sr2.r)) {
    return 0.0;
  }

  if (center_distance <=   fabs(sr1.r - sr2.r)) {
    return M_PI * pow(fmin(sr1.r, sr2.r), 2);
  }

  if (center_distance > fmax(sr1.r, sr2.r) || center_distance < fmin(sr1.r, sr2.r)) {

    float cosin_conner1 = (pow(center_distance, 2) + pow(sr1.r, 2) - pow(sr2.r, 2)) / 
                          (2 * center_distance * sr1.r);

    float cosin_conner2 = (pow(center_distance, 2) + pow(sr2.r, 2) - pow(sr1.r, 2)) / 
                          (2 * center_distance * sr2.r);

    float conner1 = 2 * acos(cosin_conner1);
    float conner2 = 2 * acos(cosin_conner2);

    double square_vp1 = M_PI * pow(sr1.r, 2) * conner1 / (2 * M_PI) - 
                        pow(sr1.r, 2) * sin(conner1) / 2.0f;
    double square_vp2 = M_PI * pow(sr2.r, 2) * conner2 / (2 * M_PI) - 
                        pow(sr2.r, 2) * sin(conner2) / 2.0f;

    return square_vp1 + square_vp2;
  }

  if (center_distance < fmax(sr1.r, sr2.r) && center_distance > fmin(sr1.r, sr2.r)) {
    float cosin_conner1 = (pow(center_distance, 2) + pow(sr1.r, 2) - pow(sr2.r, 2)) / 
                          (2 * center_distance * sr1.r);

    float cosin_conner2 = (pow(center_distance, 2) + pow(sr2.r, 2) - pow(sr1.r, 2)) / 
                          (2 * center_distance * sr2.r);
    
    float conner1 = 2 * acos(cosin_conner1);
    float conner2 = 2 * acos(cosin_conner2);

    float r_max = fmax(sr1.r, sr2.r);
    float r_min = fmin(sr1.r, sr2.r);

    double square_vp1 = M_PI * pow(r_max, 2) * conner1 / (2 * M_PI) - 
                        pow(r_max, 2) * sin(conner1) / 2.0f;

    double square_vp2 = M_PI * pow(r_min, 2) * conner2 / (2 * M_PI) +
                        pow(r_min, 2) * sin(conner2) / 2.0f;

    return square_vp1 + square_vp2;
  }
}

void Individual::CaclSensorsScore() {
  uint total_sensors =  Configuration::GetInstance()->n_sensor_per_indiv;
  for (uint i = 0; i < total_sensors - 1; i++) {
    for (uint j = i + 1; j < total_sensors; j++) {
      olap_area_square_ += CaclOlapSensor(list_sensor[i], list_sensor[j]);
    }
  }
}

void Individual::VFAProduce() {
  uint total_sensors =  Configuration::GetInstance()->n_sensor_per_indiv;
  float height = (float)Configuration::GetInstance()->height_area;
  float width = (float)Configuration::GetInstance()->width_area;

  for (uint i = 0; i < total_sensors; i++) {

    std::vector<Sensor> bound = {{list_sensor[i].x, 0, 0},
                                 {list_sensor[i].x, height, 0},
                                 {0, list_sensor[i].y, 0},
                                 {width, list_sensor[i].x, 0}};

    float f_r_x = 0.f, f_r_y = 0.f; 
    float f_a_x = 0.f, f_a_y = 0.f;
    uint n_r = 0, n_a = 0;

    for (uint j = 0; j < total_sensors; j++) {
      if (j == i) {
        continue;
      }

      double sr_distance = CaclSensorDistance(list_sensor[i], list_sensor[j]);
      float total_radius = list_sensor[i].r + list_sensor[i].r;

      if (sr_distance < total_radius) {
        f_r_x += (1 - total_radius / sr_distance)
                  * (list_sensor[j].x - list_sensor[i].x);

        f_r_y += (1 - total_radius / sr_distance)
                  * (list_sensor[j].y - list_sensor[i].y);
        n_r++;
      }

      if (sr_distance > total_radius) {
        f_a_x += (1 - total_radius / sr_distance)
                  * (list_sensor[j].x - list_sensor[i].x);

        f_a_y += (1 - total_radius / sr_distance)
                  * (list_sensor[j].y - list_sensor[i].y);
        n_a++;
      }
    }

    for (uint j = 0; j < bound.size(); j++) {
      double sr_distance = CaclSensorDistance(list_sensor[i], bound[j]);
      if (sr_distance < list_sensor[i].r) {
        f_r_x += (1 - list_sensor[i].r / sr_distance)
                  * (bound[j].x - list_sensor[i].x);

        f_r_y += (1 - list_sensor[i].r / sr_distance)
                  * (bound[j].y - list_sensor[i].y);
        n_r++;
      }

      if (sr_distance > list_sensor[i].r) {
        f_a_x += (1 - list_sensor[i].r / sr_distance)
                  * (bound[j].x - list_sensor[i].x);

        f_a_y += (1 - list_sensor[i].r / sr_distance)
                  * (bound[j].y - list_sensor[i].y);
        n_a++;
      }
    }

    list_sensor[i].x += (0.5f * f_r_x / n_r + 0.5f * f_a_x / n_a);
    list_sensor[i].y += (0.5f * f_r_y / n_r + 0.5f * f_a_y / n_a);

  }
}

double Individual::GetSensorsScore() {
  return olap_area_square_;
}
