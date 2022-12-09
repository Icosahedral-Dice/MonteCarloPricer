//
//  PathGenerator.cpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/24/22.
//

#include "PathGenerator.hpp"
#include <algorithm>
#include <cmath>

OneAssetNoPath::OneAssetNoPath(std::size_t size) : S(size) {}

OneAssetNoPath_BS::OneAssetNoPath_BS(double S0, double T, double sigma, double r, double q, const std::vector<double>& z_arr) : OneAssetNoPath(z_arr.size()) {
    
    auto z_to_s = [&](double z)->double {
        return S0 * std::exp((r - q - sigma * sigma / 2.) * T + sigma * std::sqrt(T) * z);
    };
    
    std::transform(z_arr.cbegin(), z_arr.cend(), S.begin(), z_to_s);
}

OneAssetWithPath::OneAssetWithPath(const std::vector<std::vector<double>>& z_arr) : S(z_arr) {}

OneAssetWithPath_BS::OneAssetWithPath_BS(double S0, double T, double sigma, double r, double q, const std::vector<std::vector<double>>& z_arr) : OneAssetWithPath(z_arr) {
    
    auto z_to_s = [&](double z)->double {
        return S0 * std::exp((r - q - sigma * sigma / 2.) * T + sigma * std::sqrt(T) * z);
    };
    
    for (std::vector<double>& path : S) {
        std::transform(path.cbegin(), path.cend(), path.begin(), z_to_s);
    }
}

OneAssetWithPath_BS::OneAssetWithPath_BS(double S0, double T, double sigma, double r, double q, const std::vector<std::vector<double>>& z_arr, const Dividend& proportional, const Dividend& fixed) : OneAssetWithPath(z_arr) {
    // !!!: This stuff is hardwired. TERRIBLE CODE.
    assert(z_arr[0].size() == 4);
    
    double t1 = 2. / 12.;
    double t2 = 4. / 12.;
    double t3 = 6. / 12.;
    
    for (std::vector<double>& path : S) {
        path[0] = S0 * std::exp((r - sigma * sigma / 2.) * t1 + sigma * std::sqrt(t1) * path[0]) - 0.75;
        path[1] = path[0] * std::exp((r - sigma * sigma / 2.) * (t2 - t1) + sigma * std::sqrt(t2 - t1) * path[1]) * (1. - 0.02);
        path[2] = path[1] * std::exp((r - sigma * sigma / 2.) * (t3 - t2) + sigma * std::sqrt(t3 - t2) * path[2]) - 0.25;
        path[3] = path[2] * std::exp((r - sigma * sigma / 2.) * (T - t3) + sigma * std::sqrt(T - t3) * path[3]);
    }
}
