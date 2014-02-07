#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
//parameters 
//{
//	int		StartingPopulationSize;
//	double MortalityHazard;
//};

//actor_set Mother asAllMother; //EN All Mother actors

//actor Mother            //EN Individual
//{
//	REPORT_TIME report_time = { 0 };
//	logical alive = {TRUE};
//	event timeMortalityEvent, MortalityEvent;	//EN Mortality event
//	void Start();
//	void Finish();
//};

TIME Mother::timeMortalityEvent()
{
	TIME tEventTime = TIME_INFINITE;
	tEventTime = WAIT( - TIME( log( RandUniform(1) ) / gprParam->MortalityHazard ) );
	return tEventTime;
}

void Mother::MortalityEvent()
{
	 poStateFunctions->Set_alive(  FALSE );
	Finish();
}

void Mother::Start()
{ if (__finished) {StartAfterFinish(); return; }; StartSpecial1();
	 poStateFunctions->Set_age(  0, true );
	 poStateFunctions->Set_time(  0, true );
StartSpecial2();}

void Mother::Finish()
{
if (__finished) {	DoubleFinish(case_seed);	return; } else __finished = TRUE;FinishSpecial();}

//table Mother DurationOfLife //EN Duration of Life
//{
//	{
//		value_in(alive),				//EN Population size
//		min_value_out(duration()),		//EN Minimum duration of life decimals=4
//		max_value_out(duration()),		//EN Maximum duration of life decimals=4
//		duration() / value_in(alive)	//EN Life expectancy decimals=4
//	}	//EN Demographic characteristics
//};
} // namespace 
