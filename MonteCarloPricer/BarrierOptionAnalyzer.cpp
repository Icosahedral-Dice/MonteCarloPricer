//
//  BarrierOptionAnalyzer.cpp
//  MonteCarloPricer
//
//  Created by 王明森 on 12/9/22.
//

#include "BarrierOptionAnalyzer.hpp"
#include "RNG.hpp"
#include "PathGenerator.hpp"

BarrierOptionAnalyzer::BarrierOptionAnalyzer(const BarrierOption& barrier_option) : barrier_option_(barrier_option), option_(barrier_option_.GetVanillaOption()) {}

double BarrierOptionAnalyzer::DiscountAndAverage(const std::vector<double>& vec) const {
    double res = std::accumulate(vec.cbegin(), vec.cend(), 0.);
    res /= vec.size();
    res *= std::exp(-option_.r_ * option_.T_);
    return res;
}

double BarrierOptionAnalyzer::Price(std::size_t path_length, std::size_t num_paths, unsigned seed) const {
    
    // Reseed generator
    LCE_uniform::reseed(seed);
    
    // Generate vector of standard Gaussian
    std::vector<std::vector<double>> Z(StandardGaussianMatrix::gen(num_paths, path_length));
    
    // From standard Gaussian get asset endpoint prices
    std::vector<std::vector<double>> S(OneAssetWithPath_BS(option_.S_, option_.T_, option_.sigma_, option_.r_, option_.q_, Z).S);
    
    // Get payoff
    std::vector<double> V(S.size());
    std::transform(S.cbegin(), S.cend(), V.begin(), barrier_option_);
    
    double value = this->DiscountAndAverage(V);
    
    return value;
}
