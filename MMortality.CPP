#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MMortality.mpp"

//parameters{
//	double MaxLife;
//};

//actor Mother            //EN Individual
//{
//	logical alive = {TRUE};
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
