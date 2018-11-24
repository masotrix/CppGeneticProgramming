#ifndef TSP_H
#define TSP_H
#include <utility>
#include <vector>
#include <string>

std::pair<std::vector<std::vector<float>>,float> hullRun(
    std::string filename);
std::pair<std::vector<std::vector<float>>,float> mstRun(
    std::string filename);

#endif /*TSP_H*/
