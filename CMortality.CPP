#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/CMortality.mpp"
//parameters 
//{
//	double		ChildMortParamMH;
//	double		ChildMortParamMI;
//};

//parameter_group ChildMortality
//{ //EN Mothers' Mortality Parameters
//	ChildMortParamMH, ChildMortParamMI
//};

//actor Child            //EN Individual
//{
//	logical alive = {TRUE};
//	event timeMortalityEvent, MortalityEvent;	//EN Mortality event
//};

TIME Child::timeMortalityEvent()
{
	TIME tEventTime = 50;
	double child_mort = {0.05};
	if ( mother_disease_status_fm == INFECTED){
		child_mort = gprParam->ChildMortParamMI;
	} else {
		child_mort = gprParam->ChildMortParamMH;
	};
	tEventTime = -TIME(log( RandUniform(5) ) / (child_mort));
	tEventTime = WAIT(tEventTime);
	return tEventTime;
};

// The implement function of MortalityEvent
void Child::MortalityEvent()
{
	 poStateFunctions->Set_alive(  FALSE );

	Finish();
}
} // namespace 
