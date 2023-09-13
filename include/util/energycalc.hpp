#include <iostream>
#include <vector>
#include <map>

using namespace std;

/**
 * @brief The EnergyCalc class deals with the relationship between CPU usage and power consumption.
 */
class EnergyCalc{
private: 
    map<double, double> CPUPowerMap;///< Map storing CPU usage and corresponding power consumption values
    double nx3,nx2,nx1,nx0;///< Coefficients for polynomial interpolation
public:
    /**
     * @brief Default constructor for the EnergyCalc class.
     */
    EnergyCalc();
    /**
     * @brief Constructor for the EnergyCalc class that initializes the CPUPowerMap.
     * @param CPUPowerMap Map storing CPU usage and corresponding power consumption values
     */
    EnergyCalc(map<double, double> CPUPowerMap);
    /**
     * @brief Sets the CPUPowerMap with the provided map.
     * @param CPUPowerMap Map storing CPU usage and corresponding power consumption values
     */
    void SetCPUPower(map<double, double> CPUPowerMap);
    /**
     * @brief Calculates the power consumption for the given CPU usage.
     * @param cpuUsage CPU usage value
     * @return Power consumption value
     */
    double CalculatePower(double cpuUsage);
    /**
     * @brief Calculates the CPU usage for the given power consumption.
     * @param powerUsage Power consumption value
     * @return CPU usage value
     */
    double CalculateCPU(double powerUsage);
};