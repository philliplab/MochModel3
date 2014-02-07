#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MInfection.mpp"

//parameters 
//{
//	double MinInfWait;
//	double MaxInfWait;
//};

//actor Mother            //EN Individual
//{
//	logical infected = {FALSE};
//
//	event timeInfectionEvent, InfectionEvent;
//};

TIME Mother::timeInfectionEvent()
{
	TIME tEventTime = TIME_INFINITE;
	if ( infected == FALSE){
		double phi = RandUniform(2);
		tEventTime = phi * gprParam->MinInfWait + (1-phi)*gprParam->MaxInfWait;
	};
	tEventTime = WAIT(tEventTime);
	return(tEventTime);
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
