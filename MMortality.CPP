#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MMortality.mpp"
//parameters{
//	double MaxLife;
//};

//classification DISEASE_STATE
//{
//	HEALTHY,
//	INFECTED,
//	SYMPTOMS,
//	TREATED,
//	DEAD
//};

//actor Mother            //EN Individual
//{
//	logical alive = {TRUE};
//	DISEASE_STATE disease_status = {HEALTHY};
//
//	event timeMortalityEvent, MortalityEvent;	//EN Mortality event
//};

TIME Mother::timeMortalityEvent()
{
	TIME tEventTime = TIME_INFINITE;
	tEventTime = WAIT(gprParam->MaxLife);
	return tEventTime;
};

void Mother::MortalityEvent()
{
	 poStateFunctions->Set_alive(  FALSE );
	Finish();
};

} // namespace 
