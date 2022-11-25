//
//  main.cpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/23/22.
//

#include <iostream>
#include "EuropeanOptionAnalyzer.hpp"

void PriceAndGreek() {
    EuropeanOption option(0., 50., 55.55, 7. / 12., .2, .02, 0.);
    EuropeanOptionAnalyzer analyzer(option);
    
    analyzer.Analyze(100000).Print();
    std::cout << option.Call() << '\t' << option.DeltaCall() << '\t' << option.VegaCall() << '\t' << option.Put() << '\t' << option.DeltaPut() << '\t' << option.VegaPut() << std::endl;
}

void VarRed() {
    EuropeanOption option(0., 50., 55.55, 7. / 12., .2, .02, 0.);
    EuropeanOptionAnalyzer analyzer(option);
    
    std::cout << analyzer.Price(1000000, EuropeanOptionAnalyzer::put, EuropeanOptionAnalyzer::vanilla) << std::endl;
    std::cout << analyzer.Price(1000000, EuropeanOptionAnalyzer::put, EuropeanOptionAnalyzer::control_variate) << std::endl;
    std::cout << analyzer.Price(1000000, EuropeanOptionAnalyzer::put, EuropeanOptionAnalyzer::antithetic_variables) << std::endl;
    std::cout << analyzer.Price(1000000, EuropeanOptionAnalyzer::put, EuropeanOptionAnalyzer::moment_matching) << std::endl;
    std::cout << analyzer.Price(1000000, EuropeanOptionAnalyzer::put, EuropeanOptionAnalyzer::MMCV) << std::endl;
    std::cout << option.Put() << std::endl;
}

int main(int argc, const char * argv[]) {
    
//    PriceAndGreek();
    VarRed();
    
    return 0;
}
