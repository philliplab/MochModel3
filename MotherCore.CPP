#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
//range LIFE
//{
//	0, 100
//};

//partition INFECTION_AGES
//{
//	0,1,2,3,4,5,10,20,30,40,50,75,100
//};

//parameters 
//{
//	int		StartingPopulationSize;
//};

//actor_set Mother asAllMother; //EN All Mother actors

//actor Mother            //EN Individual
//{
//	REPORT_TIME report_time = { 0 };
//	LIFE integer_age = self_scheduling_int(age);
//
//	void Start();
//	void Finish();
//};

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

//table Mother InfectionAge
//{
//	{
//		event(value_at_changes(infected, age)) / event(changes(infected))
//	}
//	*
//	infected
//};

//table Mother InfectionAgeHist
//{
//	self_scheduling_split(age, INFECTION_AGES)+
//	*
//	{
//		entrances(alive, FALSE),
//		entrances(infected, TRUE)
//	}
//};
} // namespace 
