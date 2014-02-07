#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"

//actor_set Child asAllChild; //EN All Child actors

//actor Child            //EN Individual
//{
//	REPORT_TIME report_time = { 0 };
//	logical alive = {TRUE};
//	logical mother_infected_dl = lMother->infected;
//	logical mother_infected_fm = {FALSE};
//
//	void Mother_Infect();
//	void Start();
//	void Finish();
//};

void Child::Mother_Infect()
{
	 poStateFunctions->Set_mother_infected_fm(  TRUE );
};

void Child::Start()
{ if (__finished) {StartAfterFinish(); return; }; StartSpecial1();
	 poStateFunctions->Set_age(  0, true );
StartSpecial2();}

void Child::Finish()
{
if (__finished) {	DoubleFinish(case_seed);	return; } else __finished = TRUE;FinishSpecial();}

//table Child DurationOfLifea //EN Duration of Lifea
//{
//	{
//		value_in(alive),				//EN Population size
//		min_value_out(duration()),		//EN Minimum duration of life decimals=4
//		max_value_out(duration()),		//EN Maximum duration of life decimals=4
//		duration() / value_in(alive)	//EN Life expectancy decimals=4
//	}	//EN Demographic characteristics
//};

//table Child InfectionAgeHist_dl
//{
//	self_scheduling_split(age, INFECTION_AGES)+
//	*
//	{
//		entrances(alive, FALSE),
//		entrances(mother_infected_dl, TRUE)
//	}
//};

//table Child Infection_count_dl
//{
//	mother_infected_dl
//	*
//	{
//		unit
//	}
//};

//table Child InfectionAgeHist_fm
//{
//	self_scheduling_split(age, INFECTION_AGES)+
//	*
//	{
//		entrances(alive, FALSE),
//		entrances(mother_infected_fm, TRUE)
//	}
//};

//table Child Infection_count_fm
//{
//	mother_infected_fm
//	*
//	{
//		unit
//	}
//};


} // namespace 
