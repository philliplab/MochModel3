#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MInfection.mpp"
//partition INCIDENCE_AGE_GROUPS
//{
//	0, 15, 20, 24, 29, 34, 39, 44, 49, 54, 59, 100
//};

//range INCIDENCE_RATES_SOURCE
//{
//	0,2
//};

//parameters
//{
//	INCIDENCE_RATES_SOURCE IncidenceRatesSource;
//	double IncidenceRates[INCIDENCE_RATES_SOURCE][LIFE];
//	double MinInfWait;
//	double MaxInfWait;
//};

//actor Mother            //EN Individual
//{
//	logical infected = {FALSE};
//	event timeInfectionEvent, InfectionEvent;
//};

TIME Mother::timeInfectionEvent()
{
	TIME tEventTime = TIME_INFINITE;
	if ( infected == FALSE){
		double curr_incidence = gprParam->IncidenceRates[CHKBND(gprParam->IncidenceRatesSource,3,"IncidenceRates",0)][CHKBND(integer_age ,101,"IncidenceRates",1)];
		if (curr_incidence >= 1){
			tEventTime = WAIT(0);
		} else if (curr_incidence > 0){
			tEventTime = WAIT(-log(RandUniform(2)) / -log(1 - curr_incidence));
		};
	};
	return tEventTime;
};

void Mother::InfectionEvent()
{
	 poStateFunctions->Set_infected(  TRUE );
	int nIndex = {-1};
	Child *prChild = {NULL};
	while (prChild = mlChild->GetNext(nIndex+1, &nIndex))
	{
		prChild->UpdateTime()->Mother_Infect();
	}
};
} // namespace 
