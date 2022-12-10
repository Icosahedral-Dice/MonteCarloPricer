//
//  BarrierOptionAnalyzer.hpp
//  MonteCarloPricer
//
//  Created by 王明森 on 12/9/22.
//

#ifndef BarrierOptionAnalyzer_hpp
#define BarrierOptionAnalyzer_hpp

#include "PathDependentOption.hpp"

class BarrierOptionAnalyzer {
private:
    BarrierOption barrier_option_;
    EuropeanOption option_;
    
public:
    BarrierOptionAnalyzer(const BarrierOption& barrier_option);
    
    double Price(std::size_t path_length, std::size_t num_paths, unsigned seed = 1) const;
    
    double DiscountAndAverage(const std::vector<double>& vec) const;
};

#endif /* BarrierOptionAnalyzer_hpp */
