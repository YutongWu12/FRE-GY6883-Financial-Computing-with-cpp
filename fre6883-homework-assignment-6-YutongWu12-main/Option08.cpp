#include "Option08.h"
#include "BinomialTreeModel02.h"
#include "BinLattice02.h"
#include <iostream>
#include <cmath>
using namespace std;
namespace fre
{
    Option::~Option() {}
    double Call::Payoff(double z) const
    {
        if (z > K)
            return z - K;
        return 0.0;
    }
    double Put::Payoff(double z) const
    {
        if (z < K)
            return K - z;
        return 0.0;
    }
    double OptionCalculation::PriceByCRR(const BinomialTreeModel &Model)
    {
        double q = Model.RiskNeutProb();
        int N = pOption->GetN();
        vector<double> Price(N + 1);
        for (int i = 0; i <= N; i++)
        {
            Price[i] = pOption->Payoff(Model.CalculateAssetPrice(N, i));
        }
        for (int n = N - 1; n >= 0; n--)
        {
            for (int i = 0; i <= n; i++)
            {
                Price[i] = (q * Price[i + 1] + (1 - q) * Price[i]) / Model.GetR();
            }
        }
        return Price[0];
    }

    double OptionCalculation::PriceByCRR(const BinomialTreeModel &Model,
                                         BinLattice<double> &PriceTree,
                                         BinLattice<double> &XTree,
                                         BinLattice<double> &YTree)
    {
        double q = Model.RiskNeutProb();
        int N = pOption->GetN();
        vector<double> Price(N + 1);

        for (int i = 0; i <= N; i++)
        {
            Price[i] = pOption->Payoff(Model.CalculateAssetPrice(N, i));
        }

        PriceTree.SetN(N);
        XTree.SetN(N);
        YTree.SetN(N);

        for (int i = 0; i <= N; i++)
        {
            PriceTree.SetNode(N, i, Price[i]);
        }

        for (int n = N - 1; n >= 0; n--)
        {
            for (int i = 0; i <= n; i++)
            {
                double Su = Model.CalculateAssetPrice(n + 1, i + 1);
                double Sd = Model.CalculateAssetPrice(n + 1, i);
                double Hu = Price[i + 1];
                double Hd = Price[i];

                double delta = (Hu - Hd) / (Su - Sd);
                // double moneyMarket = (Su * delta - Hu) / Model.GetR();
                double moneyMarket = (Hu - Su * delta) / Model.GetR();

                XTree.SetNode(n, i, delta);
                YTree.SetNode(n, i, moneyMarket);

                Price[i] = (q * Hu + (1 - q) * Hd) / Model.GetR();
                PriceTree.SetNode(n, i, Price[i]);
            }
        }
        return PriceTree.GetNode(0, 0);
    }

    double OptionCalculation::PriceBySnell(const BinomialTreeModel &Model,
                                           BinLattice<double> &PriceTree, BinLattice<bool> &StoppingTree)
    {
        double q = Model.RiskNeutProb();
        int N = pOption->GetN();
        PriceTree.SetN(N);
        StoppingTree.SetN(N);
        double ContVal = 0;
        for (int i = 0; i <= N; i++)
        {
            PriceTree.SetNode(N, i, pOption->Payoff(Model.CalculateAssetPrice(N, i)));
            StoppingTree.SetNode(N, i, 1);
        }
        for (int n = N - 1; n >= 0; n--)
        {
            for (int i = 0; i <= n; i++)
            {
                ContVal = (q * PriceTree.GetNode(n + 1, i + 1) + (1 - q) * PriceTree.GetNode(n + 1, i)) / Model.GetR();
                PriceTree.SetNode(n, i, pOption->Payoff(Model.CalculateAssetPrice(n, i)));
                StoppingTree.SetNode(n, i, 1);
                if (ContVal >= PriceTree.GetNode(n, i))
                {
                    PriceTree.SetNode(n, i, ContVal);
                    StoppingTree.SetNode(n, i, 0);
                }
            }
            return PriceTree.GetNode(0, 0);
        }
    }
}