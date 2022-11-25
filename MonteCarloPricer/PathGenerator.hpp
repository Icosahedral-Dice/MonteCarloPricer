//
//  PathGenerator.hpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/24/22.
//

#ifndef PathGenerator_hpp
#define PathGenerator_hpp

#include <vector>

class OneAssetNoPath {
    // One asset
    // Only endpoints are generated
public:
    std::vector<double> S;
    
    OneAssetNoPath(std::size_t size);
    ~OneAssetNoPath() = default;
};

class OneAssetNoPath_BS : public OneAssetNoPath {
    // One asset
    // Log-normal model (Black-Scholes model)
public:
    OneAssetNoPath_BS(double S0, double T, double sigma, double r, double q, const std::vector<double>& z_arr);
    ~OneAssetNoPath_BS() = default;
};

#endif /* PathGenerator_hpp */
