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
