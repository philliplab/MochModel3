#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/CMortality.mpp"
//actor Child            //EN Individual
//{
//	logical alive = {TRUE};
//
//	event timeMortalityEvent, MortalityEvent;	//EN Mortality event
//};

TIME Child::timeMortalityEvent()
{
	TIME tEventTime = 50;
	double child_mort = {0.05};
	if ( mother_infected_fm == TRUE){
		child_mort = 0.2;
	} else {
		child_mort = 0.05;
	};
	tEventTime = -TIME(log( RandUniform(5) ) / (child_mort));
	tEventTime = WAIT(tEventTime);
	return tEventTime;
}

// The implement function of MortalityEvent
void Child::MortalityEvent()
{
	 poStateFunctions->Set_alive(  FALSE );

	Finish();
}
} // namespace 
