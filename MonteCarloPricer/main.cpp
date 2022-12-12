//
//  main.cpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/23/22.
//

#include <iostream>
#include "EuropeanOptionAnalyzer.hpp"
#include "RNG.hpp"
#include "PathDependentOption.hpp"
#include "BarrierOptionAnalyzer.hpp"
#include <iomanip>

void TestAnalyzer() {
    EuropeanOption option(0., 41., 42, .75, .25, .03, 0.01);
    EuropeanOptionAnalyzer analyzer(option);
    
    analyzer.Analyze(1280000).Print();
    std::cout << option.Call() << '\t' << option.DeltaCall() << '\t' << option.VegaCall() << '\t' << option.Put() << '\t' << option.DeltaPut() << '\t' << option.VegaPut() << std::endl;
}

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

void TestDividend() {
    EuropeanOption option(0., 50., 55.55, 7. / 12., .2, .02, 0.);
    Dividend proportional({{4. / 12.}, {.02}});
    Dividend fixed({{2. / 12., 6. / 12.}, {.75, .25}});
    
    auto payoff = std::bind(option.PutPayoff(), std::placeholders::_1, 7. / 12.);
    
    EuropeanOptionAnalyzer analyzer(option);
    for (std::size_t n = 1; n <= 256; n <<= 1) {
        LCE_uniform::reseed(1);
        auto res = analyzer.Price(n * 10000, payoff, proportional, fixed);
        std::cout << res[0] << '\t' << res[1] << '\t' << res[2] << '\t' << res[3] << '\t' << res[4] << std::endl;
    }
}

void TestBarrier() {
    EuropeanOption option(0., 42., 40., 7. / 12., .25, .03, .015);
    double B = 35.;
    
    BarrierOption barrier_option(option, B, Call, DownAndOut);
    
    BarrierOptionAnalyzer analyzer(barrier_option);
    
    double BS_price = barrier_option.BSPrice();
    
    for (std::size_t n = 50; n < 51200; n <<= 1) {
        double MC_price = analyzer.Price(200, n, 1);
        std::cout << MC_price << '\t' << std::abs(MC_price - BS_price) << std::endl;
    }
    
    for (std::size_t N = 10000; N <= 5120000; N <<= 1) {
        std::size_t m = std::ceil(std::pow(N, 1. / 3.) * std::pow(7. / 12., 2. / 3.));
        std::size_t n = std::floor(1. * N / m);
        double MC_price = analyzer.Price(m, n, 1);
        std::cout << MC_price << '\t' << std::abs(MC_price - BS_price) << std::endl;
    }
    
}

int main(int argc, const char * argv[]) {
    
    std::cout << std::fixed << std::setprecision(8);
//    TestAnalyzer();
//    PriceAndGreek();
//    VarRed();
    TestDividend();
//    TestBarrier();
    
    return 0;
}
