#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MSymptoms.mpp"
//parameters
//{
//	double MinSymptomWait;
//	double MaxSymptomWait;
//	double MSymptomsShape;
//	double MSymptomsScale;
//};

//parameter_group MotherSymptoms
//{ //EN Mothers' Symptom Parameters
//	MinSymptomWait, MaxSymptomWait	
//};

//actor Mother            //EN Individual
//{
//	event timeSymptomEvent, SymptomEvent;
//};

double InvWeibull2(double y, double kappa, double lambda, double phi)
{
	// Lambda is Scale and Kappa is Shape
	double x = 0;
	if (phi == 1000){ // SIGH - this is not a pretty hack - no construct like R's Inf in C++?
		x = pow(10, 10);
	} else
	{
		x = (phi * lambda) * pow(-log(y), 1/kappa);
	};
	return(x);
};

TIME Mother::timeSymptomEvent()
{
	TIME tEventTime = TIME_INFINITE;
	double draw = RandUniform(6);
	if ( disease_status == INFECTED)
	{
		tEventTime = InvWeibull2(draw, gprParam->MSymptomsShape, gprParam->MSymptomsScale, gprParam->MInfectedMortPhi[CHKBND(vdp_age_at_infection ,5,"MInfectedMortPhi",0)]);
	}
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
