#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MMortality.mpp"
//parameters
//{
//	double MaxLife;
//	double MInfectedMortShape;
//	double MInfectedMortScale;
//	double MInfectedMortPhi[VAN_DER_PAAL_INDX];
//	double DefaultTimeToDeath;
//
//	double MSymptomsMortShape;
//	double MSymptomsMortScale;
//};

//parameter_group MotherMortality
//{ //EN Mothers' Mortality Parameters
//	MaxLife, MInfectedMortShape, MInfectedMortScale
//};

//actor Mother            //EN Individual
//{
//	logical inf_mortality_rng = {FALSE};
//	double inf_mortality_draw = {-1};
//
//	event timeInfMortRNGEvent, InfMortRNGEvent;
//	event timeMortalityEvent, MortalityEvent;	//EN Mortality event
//};

TIME Mother::timeInfMortRNGEvent()
{
	TIME tEventTime = TIME_INFINITE;
	if ( inf_mortality_rng == FALSE){
		tEventTime = 0;
	};
	tEventTime = WAIT(tEventTime);
	return(tEventTime);
};

void Mother::InfMortRNGEvent()
{
	 poStateFunctions->Set_inf_mortality_rng(  TRUE );
	 poStateFunctions->Set_inf_mortality_draw(  RandUniform(3) );
};

double InvWeibull(double y, double kappa, double lambda, double phi)
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

TIME Mother::timeMortalityEvent()
{
	TIME tEventTime = TIME_INFINITE;
	double draw = RandUniform(7);
	
	switch(disease_status )
	{
	case HEALTHY:
		{
			tEventTime = gprParam->MaxLife;
		}
		break;
	case INFECTED:
		{
			tEventTime = InvWeibull(inf_mortality_draw , gprParam->MInfectedMortShape, gprParam->MInfectedMortScale, gprParam->MInfectedMortPhi[CHKBND(vdp_age_at_infection ,5,"MInfectedMortPhi",0)]);
		}
		break;
	case SYMPTOMS:
		{
			tEventTime = InvWeibull(draw, gprParam->MSymptomsMortShape, gprParam->MSymptomsMortScale, gprParam->MInfectedMortPhi[CHKBND(vdp_age_at_infection ,5,"MInfectedMortPhi",0)]);
		}
	default:
		{
			tEventTime = gprParam->DefaultTimeToDeath;
		}
		break;
	};
	tEventTime = WAIT(tEventTime);
	return tEventTime;
};

void Mother::MortalityEvent()
{
	 poStateFunctions->Set_alive(  FALSE );
	 poStateFunctions->Set_disease_status(  DEAD );
	int nIndex = {-1};
	Child *prChild = {NULL};
	while (prChild = mlChild->GetNext(nIndex+1, &nIndex))
	{
		prChild->UpdateTime()->Mother_Die();
	}
	Finish();
};
} // namespace 
