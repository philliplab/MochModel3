#ifndef CustomH
#define CustomH

#include <boost/math/tools/roots.hpp>

class InfMortSurvCurve {
public:
	double kappa;
	double lambda;
	double phi;
	double operator() (const double x);
};

#endif