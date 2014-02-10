#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
//actor_set Child asAllChild; //EN All Child actors

//actor Child            //EN Individual
//{
//	REPORT_TIME report_time = { 0 };
//	DISEASE_STATE mother_disease_status_dl = lMother->disease_status;
//	DISEASE_STATE mother_disease_status_fm = {HEALTHY};
//
//	void Mother_Infect();
//	void Mother_Die();
//	void Start();
//	void Finish();
//};

void Child::Mother_Infect()
{
	 poStateFunctions->Set_mother_disease_status_fm(  INFECTED );
};

void Child::Mother_Die()
{
	 poStateFunctions->Set_mother_disease_status_fm(  DEAD );
};

void Child::Start()
{ if (__finished) {StartAfterFinish(); return; }; StartSpecial1();
	 poStateFunctions->Set_age(  0, true );
StartSpecial2();}

void Child::Finish()
{
if (__finished) {	DoubleFinish(case_seed);	return; } else __finished = TRUE;FinishSpecial();}

//table Child DurationOfLifeChild //EN Duration of Life Child
//{
//	{
//		value_in(alive),				//EN Population size
//		min_value_out(duration()),		//EN Minimum duration of life decimals=4
//		max_value_out(duration()),		//EN Maximum duration of life decimals=4
//		duration() / value_in(alive)	//EN Life expectancy decimals=4
//	}	//EN Demographic characteristics
//};

//table Child DurationOfLifeChildHM //EN Duration of Life ChildHM
//[mother_disease_status_dl == FALSE]
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
//		entrances(mother_disease_status_dl, INFECTED)
//	}
//};

//table Child Infection_count_dl
//{
//	mother_disease_status_dl
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
//		entrances(mother_disease_status_fm, INFECTED)
//	}
//};

//table Child Infection_count_fm
//{
//	mother_disease_status_fm
//	*
//	{
//		unit
//	}
//};
} // namespace 
