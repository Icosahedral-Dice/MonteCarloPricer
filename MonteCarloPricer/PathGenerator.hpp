//
//  PathGenerator.hpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/24/22.
//

#ifndef PathGenerator_hpp
#define PathGenerator_hpp

#include <vector>
#include <iostream>

class OneAssetNoPath {
    // One asset
    // Only endpoints are generated
public:
    std::vector<double> S;
    
    OneAssetNoPath(std::size_t size);
    ~OneAssetNoPath() = default;
};

struct Dividend {
    std::vector<double> dates;
    std::vector<double> dividends;
    
    void Print() const {
        std::cout << "t: ";
        for (double t : dates) {
            std::cout << t << ' ';
        }
        std::cout << "\nd: ";
        for (double d : dividends) {
            std::cout << d << ' ';
        }
        std::cout << std::endl;
    }
};

class OneAssetNoPath_BS : public OneAssetNoPath {
    // One asset
    // Log-normal model (Black-Scholes model)
public:
    OneAssetNoPath_BS(double S0, double T, double sigma, double r, double q, const std::vector<double>& z_arr);
    ~OneAssetNoPath_BS() = default;
};

class OneAssetWithPath {
    // One Asset
    // The whole path is generated
public:
    std::vector<std::vector<double>> S;
    
    OneAssetWithPath(const std::vector<std::vector<double>>& z_arr);
    ~OneAssetWithPath() = default;
};

class OneAssetWithPath_BS : public OneAssetWithPath {
    // One Asset
    // The whole path is generated
    // Log-normal model (Black-Scholes model)
public:
    OneAssetWithPath_BS(double S0, double T, double sigma, double r, double q, const std::vector<std::vector<double>>& z_arr);
    OneAssetWithPath_BS(double S0, double T, double sigma, double r, double q, const std::vector<std::vector<double>>& z_arr, const Dividend& proportional, const Dividend& fixed);
    ~OneAssetWithPath_BS() = default;
};

#endif /* PathGenerator_hpp */
