/* file individual.h
  author
  date
*/
#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H


#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "configuration.h"

#define _USE_MATH_DEFINES
/*
#define TOTAL_SENSORS 18
#define WIDTH 800
#define HEIGHT 800
*/
struct Sensor {
  float x;
  float y;
  float r;
};

typedef unsigned uint;

double CaclSensorDistance(const Sensor& sr1, const Sensor& sr2);

class Individual {
 public:
  Individual();
  ~Individual();
  void InitSensor();
  void SetRandomIdx();
  void GetRandomIdx(uint input_list_idx[]);
  void DrawResult(const char*);
  void CaclSensorsScore();
  double GetSensorsScore();
  void VFAProduce();
  public:
   double olap_area_square_;
   std::vector<Sensor> list_sensor;

   std::vector<uint> random_id;
 private:
  double CaclOlapSensor(const Sensor& sr1, const Sensor& sr2);

 private:
  std::map<std::string, float> type_to_radius_converter;


  cv::Mat image_;

};
#endif //end file
