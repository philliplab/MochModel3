#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MFertility.mpp"

//link Child.lMother Mother.mlChild[];

//parameters 
//{
//	double MinBirthWait;
//	double MaxBirthWait;
//	int MaxChildren;
//};

//actor Mother            //EN Individual
//{
//	int number_children = {0};
//
//	event timeBirthEvent, BirthEvent;
//};

TIME Mother::timeBirthEvent()
{
	TIME tEventTime = TIME_INFINITE;
	if ( number_children < gprParam->MaxChildren){
		double phi = RandUniform(1);
		tEventTime = phi * gprParam->MinBirthWait + (1-phi)*gprParam->MaxBirthWait;
	};
	tEventTime = WAIT(tEventTime);
	return(tEventTime);
};

void Mother::BirthEvent()
{
	poStateFunctions->PlusPlusSet_number_children();
	Child *prChild = new Child();
	mlChild->Add(prChild);
	prChild->UpdateTime()->Start();
	if ( infected == TRUE){
		prChild->UpdateTime()->Mother_Infect();
	}
};
} // namespace 
