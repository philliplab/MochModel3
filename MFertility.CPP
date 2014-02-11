#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MFertility.mpp"
//link Child.lMother Mother.mlChild[];

//parameters 
//{
//	double MinBirthWait;
//	double MaxBirthWait;
//	int MaxChildren;
//	double FertilityRates[LIFE];
//};

//parameter_group MotherFertility
//{ //EN Mothers' Mortality Parameters
//	MinBirthWait, MaxBirthWait, MaxChildren, FertilityRates
//};

//partition TIME_PART
//{
//	0,10,20,30,40,50,60,70,100
//};

//actor Mother            //EN Individual
//{
//	LIFE number_children = {0};
//	LIFE number_children_link = count(mlChild);
//
//	event timeBirthEvent, BirthEvent;
//};

TIME Mother::timeBirthEvent()
{
	TIME tEventTime = TIME_INFINITE;
	double curr_fertility = gprParam->FertilityRates[CHKBND(integer_age ,101,"FertilityRates",0)];
	if (curr_fertility >= 1){
		tEventTime = 0;
	} else if (curr_fertility > 0)
	{
		tEventTime = -log(RandUniform(4)) / -log(1 - curr_fertility);
	};
	tEventTime = WAIT(tEventTime);
	return tEventTime;
};

void Mother::BirthEvent()
{
	poStateFunctions->PlusPlusSet_number_children();
	Child *prChild = new Child();
	mlChild->Add(prChild);
	prChild->UpdateTime()->Start();
	if ( disease_status == INFECTED){
		prChild->UpdateTime()->Mother_Infect();
	}
};

//table Mother ageAtBirth
//{
//	self_scheduling_split(age, INFECTION_AGES)+
//	*
//	{
//		changes(number_children)
//	}
//};


//table Mother FertDetails
//[alive == FALSE]
//{
//	number_children+
//	*
//	{
//		unit
//	}	//EN Demographic characteristics
//};

//table Mother FertDetailsLink
//{
//	{
//		value_at_entrances(alive, FALSE, number_children_link)
//	}	//EN Demographic characteristics
//};

//table Mother FertDetailsSum
//{
//	{
//		value_at_entrances(alive, FALSE, number_children)
//	}	//EN Demographic characteristics
//};
} // namespace 
