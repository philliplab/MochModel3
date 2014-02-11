#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MSymptoms.mpp"
//parameters
//{
//	double MinSymptomWait;
//	double MaxSymptomWait;
//};

//parameter_group MotherSymptoms
//{ //EN Mothers' Symptom Parameters
//	MinSymptomWait, MaxSymptomWait	
//};

//actor Mother            //EN Individual
//{
//	event timeSymptomEvent, SymptomEvent;
//};

TIME Mother::timeSymptomEvent()
{
	TIME tEventTime = TIME_INFINITE;
	double draw = RandUniform(6);
	tEventTime = gprParam->MinSymptomWait + (gprParam->MaxSymptomWait - gprParam->MinSymptomWait) * draw;
	tEventTime = WAIT(tEventTime);
	return tEventTime;
};

void Mother::SymptomEvent()
{
	 poStateFunctions->Set_disease_status(  SYMPTOMS );
	 poStateFunctions->Set_vdp_age_at_symptoms(  vdp_age_int  );
	int nIndex = {-1};
	Child *prChild = {NULL};
	while (prChild = mlChild->GetNext(nIndex+1, &nIndex))
	{
		prChild->UpdateTime()->Mother_Symptoms();
	}
};
} // namespace 
