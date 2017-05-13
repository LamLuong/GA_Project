
#include "population.h"

int main(int argc, char *argv[]) {
  Configuration::GetInstance()->ReadConfig("config.ini");
  Population population;
  population.LoopHibrid();
  return 0;

}
