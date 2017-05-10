#include "individual.h"

Individual::Individual(const std::vector<std::string>& input_raw_sensor) {
  type_to_radius_converter["R1"] = 70;
  type_to_radius_converter["R2"] = 80;
  type_to_radius_converter["R3"] = 90;

  image_ = cv::Mat(WIDTH, HEIGHT, CV_8UC3, cv::Scalar(255,255,255));
  olap_area_square_ = 0;

  for (uint i = 0; i < TOTAL_SENSORS; i++) {
      list_sensor[i].x = 0;
      list_sensor[i].y = 0;
      list_sensor[i].r = type_to_radius_converter[input_raw_sensor[i]];
  }
}

void Individual::SetRandomIdx() {

  std::vector<uint> list_idx;
  for (uint i = 0; i < TOTAL_SENSORS; i++) {
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
  memcpy(input_list_idx, random_id, sizeof(uint) * TOTAL_SENSORS);
}


void Individual::DrawResult(const char* filename) {
  for (uint i = 0; i < TOTAL_SENSORS; i++) {
    cv::circle(image_, cv::Point2f(list_sensor[i].x, list_sensor[i].y), list_sensor[i].r,
           cv::Scalar( 0, 0, 255 ), 1, 8);
  }
  cv::imwrite(filename, image_);
}

void Individual::InitSensor() {
  float sensor_width_coverage_area = 0.f;
  float sensor_height_coverage_area = 0.f;
  float current_height_coverage_area = 0.f;

  for (uint i = 0; i < TOTAL_SENSORS; i++) {

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
    if (list_sensor[random_id[i]].y > HEIGHT) {
      list_sensor[random_id[i]].y = HEIGHT;
    }

    if (list_sensor[random_id[i]].y + list_sensor[random_id[i]].r > HEIGHT) {
      list_sensor[random_id[i]].y = HEIGHT - list_sensor[random_id[i]].r;
    }

    if (i < TOTAL_SENSORS - 1 &&
        sensor_width_coverage_area + 2 * list_sensor[random_id[i + 1]].r > WIDTH) {
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
  for (uint i = 0; i < TOTAL_SENSORS - 1; i++) {
    for (uint j = i + 1; j < TOTAL_SENSORS; j++) {
      olap_area_square_ += CaclOlapSensor(list_sensor[i], list_sensor[j]);
    }
  }
}

double Individual::GetSensorsScore() {
  return olap_area_square_;
}
