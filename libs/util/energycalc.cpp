#include <util/energycalc.hpp>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

EnergyCalc::EnergyCalc() {
    nx3 = 0;
    nx2 = 0;
    nx1 = 0;
    nx0 = 0;
}

EnergyCalc::EnergyCalc(map<double, double> cpuPowerMap) {
    vector<double> data_x;
    vector<double> data_y;
    for (const auto& it : cpuPowerMap) {
        data_x.push_back(it.first);
        data_y.push_back(it.second);
    }
    // for(int i = 0; i < cpuPowerMap.size(); i++){        
    //     cout << data_x[i];
	// 	cout << " " << data_y[i] << endl;
    // }
    double Nx = 0, Nx2 = 0, Nx3 = 0, Nx4 = 0, Nx5 = 0, Nx6 = 0, y = 0, yx = 0, yx2 = 0, yx3 = 0;
    Matrix4d X;
    Matrix4d XI;
    Vector4d Y;
    Vector4d O;
    for (int i = 0; i < cpuPowerMap.size(); i++) {
        Nx += data_x[i];
        Nx2 += data_x[i] * data_x[i];
        Nx3 += data_x[i] * data_x[i] * data_x[i];
        Nx4 += data_x[i] * data_x[i] * data_x[i] * data_x[i];
        Nx5 += data_x[i] * data_x[i] * data_x[i] * data_x[i] * data_x[i];
        Nx6 += data_x[i] * data_x[i] * data_x[i] * data_x[i] * data_x[i] * data_x[i];
        y += data_y[i];
        yx += data_y[i] * data_x[i];
        yx2 += data_y[i] * data_x[i] * data_x[i];
        yx3 += data_y[i] * data_x[i] * data_x[i] * data_x[i];
    }
    X(0, 0) = Nx3;
    X(0, 1) = Nx2;
    X(0, 2) = Nx;
    X(0, 3) = cpuPowerMap.size();
    X(1, 0) = Nx4;
    X(1, 1) = Nx3;
    X(1, 2) = Nx2;
    X(1, 3) = Nx;
    X(2, 0) = Nx5;
    X(2, 1) = Nx4;
    X(2, 2) = Nx3;
    X(2, 3) = Nx2;
    X(3, 0) = Nx6;
    X(3, 1) = Nx5;
    X(3, 2) = Nx4;
    X(3, 3) = Nx3;

    XI = X.inverse();

    Y[0] = y;
    Y[1] = yx;
    Y[2] = yx2;
    Y[3] = yx3;
    // cout << X << endl;
    // cout << Y << endl;
    O = XI * Y;

    // cout << O[0] << "x3"
    //      << " + " << O[1] << "x2"
    //      << " + " << O[2] << "x1"
    //      << " + " << O[3] << endl;
    this->nx0 = O[3];
    this->nx1 = O[2];
    this->nx2 = O[1];
    this->nx3 = O[0];
}

void EnergyCalc::SetCPUPower(map<double, double> cpuPowerMap) {
    vector<double> data_x(cpuPowerMap.size());
    vector<double> data_y(cpuPowerMap.size());
    for (map<double, double>::iterator it = cpuPowerMap.begin(); it != cpuPowerMap.end(); it++) {
        data_x.push_back(it->first);
        data_y.push_back(it->second);
    }
    double Nx = 0, Nx2 = 0, Nx3 = 0, Nx4 = 0, Nx5 = 0, Nx6 = 0, y = 0, yx = 0, yx2 = 0, yx3 = 0;
    Matrix4d X;
    Matrix4d XI;
    Vector4d Y;
    Vector4d O;
    for (int i = 0; i < cpuPowerMap.size(); i++) {
        Nx += data_x[i];
        Nx2 += data_x[i] * data_x[i];
        Nx3 += data_x[i] * data_x[i] * data_x[i];
        Nx4 += data_x[i] * data_x[i] * data_x[i] * data_x[i];
        Nx5 += data_x[i] * data_x[i] * data_x[i] * data_x[i] * data_x[i];
        Nx6 += data_x[i] * data_x[i] * data_x[i] * data_x[i] * data_x[i] * data_x[i];
        y += data_y[i];
        yx += data_y[i] * data_x[i];
        yx2 += data_y[i] * data_x[i] * data_x[i];
        yx3 += data_y[i] * data_x[i] * data_x[i] * data_x[i];
    }
    X(0, 0) = Nx3;
    X(0, 1) = Nx2;
    X(0, 2) = Nx;
    X(0, 3) = cpuPowerMap.size();
    X(1, 0) = Nx4;
    X(1, 1) = Nx3;
    X(1, 2) = Nx2;
    X(1, 3) = Nx;
    X(2, 0) = Nx5;
    X(2, 1) = Nx4;
    X(2, 2) = Nx3;
    X(2, 3) = Nx2;
    X(3, 0) = Nx6;
    X(3, 1) = Nx5;
    X(3, 2) = Nx4;
    X(3, 3) = Nx3;

    XI = X.inverse();

    Y[0] = y;
    Y[1] = yx;
    Y[2] = yx2;
    Y[3] = yx3;
    // cout << X << endl;
    // cout << Y << endl;
    O = XI * Y;

    // cout << O[0] << "x3"
    //      << " + " << O[1] << "x2"
    //      << " + " << O[2] << "x1"
    //      << " + " << O[3] << endl;
    this->nx0 = O[3];
    this->nx1 = O[2];
    this->nx2 = O[1];
    this->nx3 = O[0];
}
double EnergyCalc::CalculatePower(double cpuUsages) {
    double x3, x2, x1; 
    x3 = cpuUsages*cpuUsages*cpuUsages;
    x2 = cpuUsages*cpuUsages;
    x1 = cpuUsages;
    return nx0+(x3*nx3)+(x2*nx2)+(x1*nx1);
}
double EnergyCalc::CalculateCPU(double powerUsage) {
    double x;
    
    // 방정식을 푸는 과정
    // Newton-Raphson 방법을 사용하여 수치적인 근사 해를 얻음
    double x0 = 0.0; // 초기값
    double f, fPrime;
    double epsilon = 1e-6; // 수렴 기준
    
    do {
        f = nx3 * pow(x0, 3) + nx2 * pow(x0, 2) + nx1 * x0 + nx0 - powerUsage;
        fPrime = 3 * nx3 * pow(x0, 2) + 2 * nx2 * x0 + nx1;
        x = x0 - f / fPrime;
        x0 = x;
    } while (fabs(f) > epsilon);

    return x;
}
