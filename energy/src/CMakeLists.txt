# Create a static library named "EnergyCalc"
add_library(EnergyCalc STATIC energycalc.cpp)

# If you want to use Eigen's headers only as a system library
# target_include_directories(EnergyCalc SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/eigen)

# If you want to use Eigen's headers as a non-system library
target_include_directories(EnergyCalc PRIVATE ${CMAKE_SOURCE_DIR}/lib/eigen)
