#include "Pid.h"

#include <cmath>
#include <iostream>

namespace
{

    bool expectNear(const char *testName, const float actual, const float expected)
    {
        constexpr float tolerance = 0.0001f;
        if (std::fabs(actual - expected) <= tolerance)
        {
            return true;
        } /* if */

        std::cerr << testName << " failed: expected " << expected << ", got " << actual << '\n';
        return false;
    } /* expectNear */

    bool testProportionalTerm()
    {
        Pid pid(2.0f);
        return expectNear("proportional term", pid.getPidOutput(3.0f), 6.0f);
    } /* testProportionalTerm */

    bool testIntegralTerm()
    {
        Pid pid(0.0f, 0.5f);
        const bool firstOutputIsCorrect =
            expectNear("integral term, first step", pid.getPidOutput(2.0f), 1.0f);
        const bool secondOutputIsCorrect =
            expectNear("integral term, accumulated step", pid.getPidOutput(2.0f), 2.0f);
        return firstOutputIsCorrect && secondOutputIsCorrect;
    } /* testIntegralTerm */

    bool testDerivativeTerm()
    {
        Pid pid(0.0f, 0.0f, 3.0f);
        const bool firstOutputIsCorrect =
            expectNear("derivative term, first step", pid.getPidOutput(2.0f), 6.0f);
        const bool secondOutputIsCorrect =
            expectNear("derivative term, error change", pid.getPidOutput(5.0f), 9.0f);
        return firstOutputIsCorrect && secondOutputIsCorrect;
    } /* testDerivativeTerm */

    bool testOutputLimits()
    {
        Pid pid(2.0f);
        const bool upperLimitIsApplied =
            expectNear("upper output limit", pid.getPidOutput(100.0f), 10.0f);
        const bool lowerLimitIsApplied =
            expectNear("lower output limit", pid.getPidOutput(-100.0f), -10.0f);

        pid.setOutputLimit(4.0f);
        const bool customLimitIsApplied =
            expectNear("custom output limit", pid.getPidOutput(100.0f), 4.0f);
        return upperLimitIsApplied && lowerLimitIsApplied && customLimitIsApplied;
    } /* testOutputLimits */

    bool testIntegralAntiWindup()
    {
        Pid pid(0.0f, 1.0f);
        pid.setOutputLimit(100.0f);

        const bool upperBoundIsApplied =
            expectNear("integral upper bound", pid.getPidOutput(60.0f), 50.0f);
        const bool upperBoundIsRetained =
            expectNear("integral remains bounded", pid.getPidOutput(0.0f), 50.0f);
        const bool lowerBoundIsApplied =
            expectNear("integral lower bound", pid.getPidOutput(-120.0f), -50.0f);
        return upperBoundIsApplied && upperBoundIsRetained && lowerBoundIsApplied;
    } /* testIntegralAntiWindup */

    bool testGainSetters()
    {
        Pid pid(0.0f);
        pid.setKp(1.0f);
        pid.setKi(1.0f);
        pid.setKd(1.0f);
        return expectNear("gain setters", pid.getPidOutput(2.0f), 6.0f);
    } /* testGainSetters */

} // namespace

int main()
{
    bool passed = true;
    passed = testProportionalTerm() && passed;
    passed = testIntegralTerm() && passed;
    passed = testDerivativeTerm() && passed;
    passed = testOutputLimits() && passed;
    passed = testIntegralAntiWindup() && passed;
    passed = testGainSetters() && passed;
    if (!passed)
    {
        return 1;
    } /* if */

    std::cout << "All PID tests passed\n";
    return 0;
} /* main */
