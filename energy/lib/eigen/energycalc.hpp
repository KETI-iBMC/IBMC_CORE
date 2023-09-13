#include <iostream>
#include <vector>
#include <map>
#include <Eigen/Dense>
#include <Eigen/LU>

using namespace std;

class EnergyCalc{
private: 
    map<double, double> CPUPowerMap;
public:
    void SetCPUPower(map<double, double> CPUPowerMap);
    double CalculatePower(double cpuUsages);

};