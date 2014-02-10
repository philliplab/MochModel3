#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MInf_Mortality.mpp"
//actor Mother
//{
//	logical inf_mortality_rng = {FALSE};
//	double inf_mortality_draw = {-1};
//
//	event timeInfMortRNGEvent, InfMortRNGEvent;
//	event timeInfMortEvent, InfMortEvent;
//};

TIME Mother::timeInfMortRNGEvent()
{
	TIME tEventTime = TIME_INFINITE;

	if ( inf_mortality_rng == FALSE){
		tEventTime = 0;
	};
	tEventTime = WAIT(tEventTime);
	return(tEventTime);
};

void Mother::InfMortRNGEvent()
{
	 poStateFunctions->Set_inf_mortality_rng(  TRUE );
	 poStateFunctions->Set_inf_mortality_draw(  RandUniform(3) );
};

double InvWeibull(double y, double kappa, double lambda, double phi)
{
	double x = 0;
	x = (phi * lambda) * pow(-log(y), 1/kappa);
	return(x);
};

TIME Mother::timeInfMortEvent()
{
	TIME tEventTime = TIME_INFINITE;
	if ( disease_status == INFECTED &&  alive == TRUE){
		tEventTime = InvWeibull(inf_mortality_draw , 2, 10, 1);
	};
	tEventTime = WAIT(tEventTime);
	return(tEventTime);
};

void Mother::InfMortEvent()
{
	 poStateFunctions->Set_alive(  FALSE );
	Finish();
};
} // namespace 
