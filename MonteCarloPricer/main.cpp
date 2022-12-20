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
#include <vector>

void TestAnalyzer() {
    EuropeanOption option(0., 41., 42, .75, .25, .03, 0.01);
    EuropeanOptionAnalyzer analyzer(option);
    
    analyzer.Analyze(10000, 1).Print();
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

double Final(std::size_t M, std::size_t N, unsigned long seed) {
    
    EuropeanOption option(0., 70., 80., .5, .5, .02, .02);
//    std::cout << option.Call() << std::endl;
    AsianOption asian(option, Call);
    
    LCE_uniform::reseed(seed);
    std::vector<std::vector<double>> Z(StandardGaussianMatrix::gen(N, M));
    std::vector<std::vector<double>> S(OneAssetWithPath_BS(option.S_, option.T_, option.sigma_, option.r_, option.q_, Z).S);
    
    std::vector<double> V(S.size());
    std::transform(S.cbegin(), S.cend(), V.begin(), asian);
    
    
    return std::accumulate(V.cbegin(), V.cend(), 0.) / V.size() * std::exp(-option.r_ * option.T_);
    
}

void Final(std::size_t M, std::size_t N) {
    EuropeanOption option(0., 70., 80., .5, .5, .02, .02);
//    std::cout << option.Call() << std::endl;
    AsianOption asian(option, Call);
    
    std::vector<double> val;
    
    for (unsigned long seed = 1; seed <= 100; seed++) {
        LCE_uniform::reseed(seed);
        std::vector<std::vector<double>> Z(StandardGaussianMatrix::gen(N, M));
        std::vector<std::vector<double>> S(OneAssetWithPath_BS(option.S_, option.T_, option.sigma_, option.r_, option.q_, Z).S);
        
        std::vector<double> V(S.size());
        std::transform(S.cbegin(), S.cend(), V.begin(), asian);
        
        
        val.push_back(std::accumulate(V.cbegin(), V.cend(), 0.) / V.size() * std::exp(-option.r_ * option.T_));
    }
    
    double sum = std::accumulate(val.cbegin(), val.cend(), 0.);
    double sumsq = std::inner_product(val.cbegin(), val.cend(), val.cbegin(), 0.);
    
    double avg = sum / val.size();
    double stdev = std::sqrt((sumsq - sum * avg) / (double(val.size()) - 1.));
    
    std::cout << avg << '\t' << stdev << std::endl;
    
}

int main(int argc, const char * argv[]) {
    
    std::cout << std::fixed << std::setprecision(8);
//    TestAnalyzer();
//    PriceAndGreek();
//    VarRed();
//    TestDividend();
//    TestBarrier();
    std::vector<std::size_t> Ms({100, 200, 300, 400, 500, 600});
    std::vector<std::size_t> Ns({250, 1000, 2250, 4000, 6250, 9000});
    
    
    for (int i = 0; i < 6; i++) {
        Final(Ms[i], Ns[i]);
    }
    
    return 0;
}
