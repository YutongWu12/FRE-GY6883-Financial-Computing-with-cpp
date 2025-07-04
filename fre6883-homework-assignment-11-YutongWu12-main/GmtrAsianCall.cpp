#include "GmtrAsianCall.h"
#include "EurCall.h"
#include <cmath>
namespace fre
{

    double GmtrAsianCall::Payoff(const SamplePath &S) const
    {
        double Prod = 1.0;
        for (int i = 0; i < m; i++)
        {
            Prod = Prod * S[i];
        }
        if (pow(Prod, 1.0 / m) < K)
            return 0.0;
        return pow(Prod, 1.0 / m) - K;
    }

    double GmtrAsianCall::PriceByBSFormula(const MCModel &Model)
    {
        double a = exp(-Model.GetR() * T) * Model.GetS0() * exp((m + 1.0) * T / (2.0 * m) * (Model.GetR() + Model.GetSigma() * Model.GetSigma() * ((2.0 * m + 1.0) / (3.0 * m) - 1.0) / 2.0));
        double b = Model.GetSigma() * sqrt((m + 1.0) * (2.0 * m + 1.0) / (6.0 * m * m));
        EurCall G(T, K);
        Price = G.PriceByBSFormula(a, b, Model.GetR());
        return Price;
    }

    double GmtrAsianCall::DeltaByBSFormula(const MCModel &Model)
    {
        double T_ = T;
        double sigma = Model.GetSigma();
        double r = Model.GetR();
        double S0 = Model.GetS0();

        // a: adjusted S0
        double a = exp(-r * T_) * S0 * exp((m + 1.0) * T_ / (2.0 * m) * (r + sigma * sigma * ((2.0 * m + 1.0) / (3.0 * m) - 1.0) / 2.0));
        double b = sigma * sqrt((m + 1.0) * (2.0 * m + 1.0) / (6.0 * m * m));

        EurCall G(T_, K);
        return G.DeltaByBSFormula(a, b, r) * a / S0;
    }

}
