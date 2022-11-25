//
//  EuropeanOptionAnalyzer.hpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/24/22.
//

#ifndef EuropeanOptionAnalyzer_hpp
#define EuropeanOptionAnalyzer_hpp

#include "EuropeanOption.hpp"
#include <iostream>

struct EuropeanOptionResults {
    double Call;
    double DeltaCall;
    double VegaCall;
    double Put;
    double DeltaPut;
    double VegaPut;
    
    void Print() const {
        std::cout << Call << '\t' << DeltaCall << '\t' << VegaCall << '\t' << Put << '\t' << DeltaPut << '\t' << VegaPut << std::endl;
    }
};

class EuropeanOptionAnalyzer {
private:
    EuropeanOption option_;
    
public:
    EuropeanOptionAnalyzer(const EuropeanOption& option);
    
    EuropeanOptionResults Analyze(std::size_t N, unsigned long seed = 1) const;
    
    enum VarRed {
        vanilla,
        control_variate,
        antithetic_variables,
        moment_matching,
        MMCV,
    };
    
    enum OptionType {
        call,
        put,
    };
    
    double Price(std::size_t N, const OptionType& type, const VarRed& modifier = vanilla, unsigned long seed = 1) const;
    
private:
    double DiscountAndAverage(const std::vector<double>& vec) const;
    
    double PriceVanilla(std::size_t N, const std::function<double (double)>& payoff) const;
    // Control variate
    double PriceCV(std::size_t N, const std::function<double (double)>& payoff) const;
    // Antithetic variables
    double PriceAV(std::size_t N, const std::function<double (double)>& payoff) const;
    // Moment matching
    double PriceMM(std::size_t N, const std::function<double (double)>& payoff) const;
    // Moment matching and control variables
    double PriceMMCV(std::size_t N, const std::function<double (double)>& payoff) const;
    
    
};

#endif /* EuropeanOptionAnalyzer_hpp */
