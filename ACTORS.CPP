#define	MAIN_MODULE
#include "model.h"

namespace mm {

#if defined(MG_CHKSUM)
	bool ::gbCheckSumActivated = true;
#else
	bool ::gbCheckSumActivated = false;
#endif

#pragma warning(disable : 4723)

ModelApp gApp( Simulation, InitParameters, DeleteParameters,
	ProcessModelGeneratedParameters, InitActors, DeleteActors,
	InitTables, PreSimulation, PostSimulation, UserTables,
	VerifyParameters, ValidateParameters, InitActorSets, DeleteActorSets );

void AFXAPI AfxAbort()
{
	TRACE0("AfxAbort called\n");
	DebugBreak();
#ifdef _WINDOWS
	AfxWinTerm();
#endif
	abort();
}

Thread EventQueue *gpoEventQueue;

Thread double ::gdThreadCurrentTime = 0;

Thread double ::gdThreadEvents = 0;

Thread double ::gdEventsForTabulation = 0;

Parameters	*gprParam;

BOOL VerifyParameters( CString *pszError)
{
	BOOL	bResult = TRUE;

	(*pszError).Empty();
	if ( !VerifyParameter( pszError, "ChildMortParamMH" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "ChildMortParamMI" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "FertilityRates" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "IncidenceRates" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "IncidenceRatesSource" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MaxBirthWait" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MaxChildren" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MaxInfWait" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MaxLife" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MinBirthWait" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MInfectedMortScale" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MInfectedMortShape" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MinInfWait" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "StartingPopulationSize" ) ) bResult = FALSE;
	return bResult;
}

void InitParameters()
{
	gprParam = new Parameters();
}

void DeleteParameters()
{
	delete gprParam;
}

void SetCaseWeight( double dCaseWeight, double dCaseSubsampleWeight )
{
	gpoEventQueue->SetCaseWeight( dCaseWeight, dCaseSubsampleWeight );
}

// partitions
real p_points_INCIDENCE_AGE_GROUPS[] = { 0, 15, 20, 24, 29, 34, 39, 44, 49, 54,
	 59, 100 };
real p_points_INFECTION_AGES[] = { 0, 1, 2, 3, 4, 5, 10, 20, 30, 40,
	 50, 75, 100 };
real p_points_TIME_PART[] = { 0, 10, 20, 30, 40, 50, 60, 70, 100 };

Parameters::Parameters()
{
	Initializer( &ChildMortParamMH, "ChildMortParamMH" );
	Initializer( &ChildMortParamMI, "ChildMortParamMI" );
	Initializer( &FertilityRates, "FertilityRates" );
	Initializer( &IncidenceRates, "IncidenceRates" );
	Initializer( &IncidenceRatesSource, "IncidenceRatesSource" );
	Initializer( &MaxBirthWait, "MaxBirthWait" );
	Initializer( &MaxChildren, "MaxChildren" );
	Initializer( &MaxInfWait, "MaxInfWait" );
	Initializer( &MaxLife, "MaxLife" );
	Initializer( &MinBirthWait, "MinBirthWait" );
	Initializer( &MInfectedMortScale, "MInfectedMortScale" );
	Initializer( &MInfectedMortShape, "MInfectedMortShape" );
	Initializer( &MinInfWait, "MinInfWait" );
	Initializer( &StartingPopulationSize, "StartingPopulationSize" );
}

void ProcessModelGeneratedParameters()
{
}

void InitActors()
{
	gpoEventQueue = new EventQueue();
	gpoEmptyChild = NULL;
	gpoEmptyMother = NULL;
	gpoEmptyTicker = NULL;
}

void DeleteActors()
{
	delete gpoEventQueue;
	gctrlSection.Lock();
	gctrlSection.Unlock();



	Child *poChild = gpoEmptyChild;
	while ( poChild != NULL ) {
		Child *poChildTemp = (Child *) poChild->poNext;
		::delete poChild;
		poChild = poChildTemp;
	}


	Mother *poMother = gpoEmptyMother;
	while ( poMother != NULL ) {
		Mother *poMotherTemp = (Mother *) poMother->poNext;
		::delete poMother;
		poMother = poMotherTemp;
	}


	Ticker *poTicker = gpoEmptyTicker;
	while ( poTicker != NULL ) {
		Ticker *poTickerTemp = (Ticker *) poTicker->poNext;
		::delete poTicker;
		poTicker = poTickerTemp;
	}
}

// sizeof actors
int ::ganSizeOfActors[3] = {
	sizeof(Child) + sizeof(ChildDerivedStates) + sizeof(ChildTable),
	sizeof(Mother) + sizeof(MotherDerivedStates) + sizeof(MotherTable),
	sizeof(Ticker) + sizeof(TickerDerivedStates),
};

void *Child::operator new( size_t count )
{
	Child *poActor;
	if ( gpoEmptyChild != NULL ) {
		poActor = gpoEmptyChild;
		gpoEmptyChild = (Child *) gpoEmptyChild->poNext;
	}
	else {
		poActor = ::new Child( TRUE ) ;
	}
	poActor->__finished = false;
	return poActor;
}

void Child::Set_lMother( Mother *value )
{
	Mother *poOldLink;

	if ( lMother != value ) {
		if (lMother != NULL)
		{
			lMother->UpdateTime();
		}
		if (value != NULL)
		{
			value->UpdateTime();
		}
		poOldLink = lMother;
		lMother = value;
		UpdateLinkedStates();
		if ( poOldLink != NULL ) {
			poOldLink->mlChild->Remove( this );
		}
		if ( value != NULL ) {
			value->mlChild->Add( this );
		}
	}
}

void Child::InitActor()
{
	poDerivedStates = new ChildDerivedStates();
	poDerivedStates->poParent = this;
	poStateFunctions = new ChildStateFunctions();
	poStateFunctions->poParent = this;
	poStateFunctions->poDerivedStates = poDerivedStates;
	poDerivedStates->poStateFunctions = poStateFunctions;
	bUpdates = FALSE;
	nActorNumber = 0;
	InitializeStates();
	lMother = NULL;
}

void Child::DeleteActor()
{
	delete poDerivedStates;
	delete poStateFunctions;
}

#pragma optimize( "", off )

void Child::InitializeStates()
{
	// initialization of states
	actor_id = (long) 0;
	actor_subsample_weight = (double) 0;
	actor_weight = (double) 0;
	age = (TIME) 0;
#line 14 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/CMortality.mpp"
	alive = TRUE != 0;
#line 223 "ACTORS.CPP"
	case_id = (long) 0;
	case_seed = (double) 0;
#line 7 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	mother_disease_status_fm = HEALTHY;
#line 228 "ACTORS.CPP"
#line 5 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	CHKLMT(0, report_time, r_min_REPORT_TIME, r_max_REPORT_TIME);
	report_time = 0 < r_min_REPORT_TIME ? r_min_REPORT_TIME : 0 > r_max_REPORT_TIME ? r_max_REPORT_TIME : (REPORT_TIME) 0;
#line 232 "ACTORS.CPP"
	table_unit_Infection_count_dl = (counter) 0;
	table_unit_Infection_count_fm = (counter) 0;
	time = (TIME) gdThreadCurrentTime;
	actorset_filter_asAllChild = FALSE;
	alive_value_in = FALSE;
	poDerivedStates->duration_ = (TIME) 0;
	poDerivedStates->duration__time = (TIME) gdThreadCurrentTime;
	duration__max_value_out = (TIME) 0;
	duration__min_value_out = (TIME) 0;
	poDerivedStates->entrances_alive_FALSE = (counter) 0;
	poDerivedStates->entrances_mother_disease_status_dl_INFECTED = (counter) 0;
	poDerivedStates->entrances_mother_disease_status_fm_INFECTED = (counter) 0;
	mother_disease_status_dl = (DISEASE_STATE) 0;
	poDerivedStates->sssplit_age_INFECTION_AGES = (INFECTION_AGES) 0;
	poDerivedStates->sssplit_age_INFECTION_AGES_time = (TIME) TIME_INFINITE;
	table_filter_DurationOfLifeChild = FALSE;
	table_filter_DurationOfLifeChildHM = FALSE;
	table_filter_Infection_count_dl = FALSE;
	table_filter_Infection_count_fm = FALSE;
	table_filter_InfectionAgeHist_dl = FALSE;
	table_filter_InfectionAgeHist_fm = FALSE;
	lMother_disease_status = (DISEASE_STATE) 0;
	__finished = FALSE;
	__time = (TIME) gdThreadCurrentTime;
	__events = 0;
}

#pragma optimize( "", on )

void Child::UpdateLinkedStates()
{
	if ( lMother != NULL ) {
		poStateFunctions->Set_lMother_disease_status( lMother->disease_status );
	}
	else {
		poStateFunctions->Set_lMother_disease_status( DISEASE_STATE( 0 ) );
	}
}

void Child::StartSpecial1()
{
	nSubSample = GetSubSample();
	InitializeStates();
	case_seed = GetCaseSeed();
	case_id = GetCaseID();
	lActorId = actor_id = GetObjectID();
	GetCaseWeight( &actor_weight, &actor_subsample_weight );
	lMother = NULL;

	// initialization of actor set nodes
	poasAllChild = NULL;

	// initialization of events
	poMortalityEvent = gpoEventQueue->NewEvent( this, 0, 0 );
	poScheduleEvent = gpoEventQueue->NewEvent( this, 1, 255 );
	schedule_event_time = TIME_INFINITE;
	//Initalize derived states
#line 40 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_alive_value_in( logical( alive ) );
#line 292 "ACTORS.CPP"
#line 40 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_duration__max_value_out( TIME( poDerivedStates->duration_ ) );
#line 295 "ACTORS.CPP"
#line 40 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_duration__min_value_out( TIME( poDerivedStates->duration_ ) );
#line 298 "ACTORS.CPP"
#line 6 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_mother_disease_status_dl( DISEASE_STATE( lMother_disease_status ) );
#line 301 "ACTORS.CPP"
	CountActor(0);
}

#pragma optimize( "", off )

void Child::StartSpecial2()
{
	// initialization of expression and history states
	poStateFunctions->Set_time(TIME( CoarsenMantissa( time ) ), true);
	poStateFunctions->Set_age(TIME( CoarsenMantissa( age ) ), true);
	gdThreadCurrentTime = time;
	__time = time;
#line 1 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_actorset_filter_asAllChild( logical( 1 ) );
#line 316 "ACTORS.CPP"
	poDerivedStates->Set_duration_( (TIME) 0 );
	poDerivedStates->duration__time = time;
	poDerivedStates->Set_entrances_alive_FALSE( (counter) 0 );
	poDerivedStates->Set_entrances_mother_disease_status_dl_INFECTED( (counter) 0 );
	poDerivedStates->Set_entrances_mother_disease_status_fm_INFECTED( (counter) 0 );
#line 34 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_DurationOfLifeChild( logical( 1 ) );
#line 324 "ACTORS.CPP"
#line 45 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_DurationOfLifeChildHM( logical( mother_disease_status_dl== FALSE ) );
#line 327 "ACTORS.CPP"
#line 65 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_Infection_count_dl( logical( 1 ) );
#line 330 "ACTORS.CPP"
#line 84 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_Infection_count_fm( logical( 1 ) );
#line 333 "ACTORS.CPP"
#line 55 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_InfectionAgeHist_dl( logical( 1 ) );
#line 336 "ACTORS.CPP"
#line 74 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_InfectionAgeHist_fm( logical( 1 ) );
#line 339 "ACTORS.CPP"

	// initialization of cross-tabulation updates
	tableinfo.tab0events = -1;
	tableinfo.tab0open0 = (real) ( alive_value_in );
	tableinfo.tab0open1 = (real) ( poDerivedStates->duration_ );
	tableinfo.tab0open2 = (real) ( duration__max_value_out );
	tableinfo.tab0open3 = (real) ( duration__min_value_out );
	tableinfo.tab1events = -1;
	tableinfo.tab1open0 = (real) ( alive_value_in );
	tableinfo.tab1open1 = (real) ( poDerivedStates->duration_ );
	tableinfo.tab1open2 = (real) ( duration__max_value_out );
	tableinfo.tab1open3 = (real) ( duration__min_value_out );
	tableinfo.tab2events = -1;
	tableinfo.tab2open0 = (real) ( poDerivedStates->entrances_alive_FALSE );
	tableinfo.tab2open1 = (real) ( poDerivedStates->entrances_mother_disease_status_dl_INFECTED );
	tableinfo.tab3events = -1;
	tableinfo.tab3open0 = (real) ( table_unit_Infection_count_dl );
	tableinfo.tab4events = -1;
	tableinfo.tab4open0 = (real) ( poDerivedStates->entrances_alive_FALSE );
	tableinfo.tab4open1 = (real) ( poDerivedStates->entrances_mother_disease_status_fm_INFECTED );
	tableinfo.tab5events = -1;
	tableinfo.tab5open0 = (real) ( table_unit_Infection_count_fm );
	poPrev = NULL;
	poNext = gpoEventQueue->poHeadActor;
	if ( gpoEventQueue->poHeadActor != NULL ) {
		gpoEventQueue->poHeadActor->poPrev = this;
	}
	gpoEventQueue->poHeadActor = this;
	bUpdates = TRUE;
}

void Child::FinishSpecial()
{
	CountFinishedActor(0);
	bUpdates = FALSE;
	gpoEventQueue->DestroyEvent( poMortalityEvent );
	gpoEventQueue->DestroyEvent(poScheduleEvent);

	// close all updates
	if ( gplTableIndexes[0] >= 0 && ( tableinfo.tab0dim_filter || table_filter_DurationOfLifeChild ) ) {
		if ( tableinfo.tab0events != -1 ) {
			tableinfo.tab0open0 = (real) CloseUpdate( 0, 0, tableinfo.tab0open0, tableinfo.tab0close0, tableinfo.tab0dim_filter, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab0open1 = (real) CloseUpdate( 0, 1, tableinfo.tab0open1, tableinfo.tab0close1, tableinfo.tab0dim_filter, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab0open2 = (real) CloseUpdate( 0, 2, tableinfo.tab0open2, tableinfo.tab0close2, tableinfo.tab0dim_filter, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab0open3 = (real) CloseUpdate( 0, 3, tableinfo.tab0open3, tableinfo.tab0close3, tableinfo.tab0dim_filter, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab0cell = (index) TableCellIndex( 0, 0 );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab0total_cell = (index) tableinfo.tab0cell;
		}
		else {
			tableinfo.tab0total_cell = (index) TableTotalCellIndex( 0, 0 );
		}
		CloseUpdate( 0, 0, tableinfo.tab0open0, alive_value_in, table_filter_DurationOfLifeChild, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 0, 1, tableinfo.tab0open1, poDerivedStates->Get_duration_(), table_filter_DurationOfLifeChild, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 0, 2, tableinfo.tab0open2, poStateFunctions->Get_duration__max_value_out(), table_filter_DurationOfLifeChild, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 0, 3, tableinfo.tab0open3, poStateFunctions->Get_duration__min_value_out(), table_filter_DurationOfLifeChild, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[1] >= 0 && ( tableinfo.tab1dim_filter || table_filter_DurationOfLifeChildHM ) ) {
		if ( tableinfo.tab1events != -1 ) {
			tableinfo.tab1open0 = (real) CloseUpdate( 1, 0, tableinfo.tab1open0, tableinfo.tab1close0, tableinfo.tab1dim_filter, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab1open1 = (real) CloseUpdate( 1, 1, tableinfo.tab1open1, tableinfo.tab1close1, tableinfo.tab1dim_filter, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab1open2 = (real) CloseUpdate( 1, 2, tableinfo.tab1open2, tableinfo.tab1close2, tableinfo.tab1dim_filter, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab1open3 = (real) CloseUpdate( 1, 3, tableinfo.tab1open3, tableinfo.tab1close3, tableinfo.tab1dim_filter, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab1cell = (index) TableCellIndex( 1, 0 );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab1total_cell = (index) tableinfo.tab1cell;
		}
		else {
			tableinfo.tab1total_cell = (index) TableTotalCellIndex( 1, 0 );
		}
		CloseUpdate( 1, 0, tableinfo.tab1open0, alive_value_in, table_filter_DurationOfLifeChildHM, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 1, 1, tableinfo.tab1open1, poDerivedStates->Get_duration_(), table_filter_DurationOfLifeChildHM, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 1, 2, tableinfo.tab1open2, poStateFunctions->Get_duration__max_value_out(), table_filter_DurationOfLifeChildHM, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 1, 3, tableinfo.tab1open3, poStateFunctions->Get_duration__min_value_out(), table_filter_DurationOfLifeChildHM, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[2] >= 0 && ( tableinfo.tab2dim_filter || table_filter_InfectionAgeHist_dl ) ) {
		if ( tableinfo.tab2events != -1 ) {
			tableinfo.tab2open0 = (real) CloseUpdate( 2, 0, tableinfo.tab2open0, tableinfo.tab2close0, tableinfo.tab2dim_filter, tableinfo.tab2cell, tableinfo.tab2total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab2open1 = (real) CloseUpdate( 2, 1, tableinfo.tab2open1, tableinfo.tab2close1, tableinfo.tab2dim_filter, tableinfo.tab2cell, tableinfo.tab2total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab2cell = (index) TableCellIndex( 2, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab2total_cell = (index) tableinfo.tab2cell;
		}
		else {
			tableinfo.tab2total_cell = (index) TableTotalCellIndex( 2, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		}
		CloseUpdate( 2, 0, tableinfo.tab2open0, poDerivedStates->entrances_alive_FALSE, table_filter_InfectionAgeHist_dl, tableinfo.tab2cell, tableinfo.tab2total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 2, 1, tableinfo.tab2open1, poDerivedStates->entrances_mother_disease_status_dl_INFECTED, table_filter_InfectionAgeHist_dl, tableinfo.tab2cell, tableinfo.tab2total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[3] >= 0 && ( tableinfo.tab3dim_filter || table_filter_Infection_count_dl ) ) {
		if ( tableinfo.tab3events != -1 ) {
			poStateFunctions->Set_table_unit_Infection_count_dl(table_unit_Infection_count_dl+1);
			tableinfo.tab3close0 = (real) table_unit_Infection_count_dl;
			tableinfo.tab3open0 = (real) CloseUpdate( 3, 0, tableinfo.tab3open0, tableinfo.tab3close0, tableinfo.tab3dim_filter, tableinfo.tab3cell, tableinfo.tab3total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab3cell = (index) TableCellIndex( 3, 1,
			mother_disease_status_dl );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab3total_cell = (index) tableinfo.tab3cell;
		}
		else {
			tableinfo.tab3total_cell = (index) TableTotalCellIndex( 3, 1,
			mother_disease_status_dl );
		}
		poStateFunctions->Set_table_unit_Infection_count_dl(table_unit_Infection_count_dl+1);
		CloseUpdate( 3, 0, tableinfo.tab3open0, table_unit_Infection_count_dl, table_filter_Infection_count_dl, tableinfo.tab3cell, tableinfo.tab3total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[4] >= 0 && ( tableinfo.tab4dim_filter || table_filter_InfectionAgeHist_fm ) ) {
		if ( tableinfo.tab4events != -1 ) {
			tableinfo.tab4open0 = (real) CloseUpdate( 4, 0, tableinfo.tab4open0, tableinfo.tab4close0, tableinfo.tab4dim_filter, tableinfo.tab4cell, tableinfo.tab4total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab4open1 = (real) CloseUpdate( 4, 1, tableinfo.tab4open1, tableinfo.tab4close1, tableinfo.tab4dim_filter, tableinfo.tab4cell, tableinfo.tab4total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab4cell = (index) TableCellIndex( 4, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab4total_cell = (index) tableinfo.tab4cell;
		}
		else {
			tableinfo.tab4total_cell = (index) TableTotalCellIndex( 4, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		}
		CloseUpdate( 4, 0, tableinfo.tab4open0, poDerivedStates->entrances_alive_FALSE, table_filter_InfectionAgeHist_fm, tableinfo.tab4cell, tableinfo.tab4total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 4, 1, tableinfo.tab4open1, poDerivedStates->entrances_mother_disease_status_fm_INFECTED, table_filter_InfectionAgeHist_fm, tableinfo.tab4cell, tableinfo.tab4total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[5] >= 0 && ( tableinfo.tab5dim_filter || table_filter_Infection_count_fm ) ) {
		if ( tableinfo.tab5events != -1 ) {
			poStateFunctions->Set_table_unit_Infection_count_fm(table_unit_Infection_count_fm+1);
			tableinfo.tab5close0 = (real) table_unit_Infection_count_fm;
			tableinfo.tab5open0 = (real) CloseUpdate( 5, 0, tableinfo.tab5open0, tableinfo.tab5close0, tableinfo.tab5dim_filter, tableinfo.tab5cell, tableinfo.tab5total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab5cell = (index) TableCellIndex( 5, 1,
			mother_disease_status_fm );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab5total_cell = (index) tableinfo.tab5cell;
		}
		else {
			tableinfo.tab5total_cell = (index) TableTotalCellIndex( 5, 1,
			mother_disease_status_fm );
		}
		poStateFunctions->Set_table_unit_Infection_count_fm(table_unit_Infection_count_fm+1);
		CloseUpdate( 5, 0, tableinfo.tab5open0, table_unit_Infection_count_fm, table_filter_Infection_count_fm, tableinfo.tab5cell, tableinfo.tab5total_cell, actor_weight, actor_subsample_weight );
	}
	Set_lMother( NULL );
	if (poasAllChild != NULL)
	{	asAllChild->Remove(poasAllChild);
		poasAllChild = NULL;
	}

	// Remove actor from master list of active actors
	if ( poPrev != NULL )
	{
		poPrev->poNext = poNext;
	}
	else
	{
		if (gpoEventQueue->poHeadActor != NULL && gpoEventQueue->poHeadActor == this)
		{
			gpoEventQueue->poHeadActor = poNext;
		}
	}
	if ( poNext != NULL )
	{
		poNext->poPrev = poPrev;
	}
	poPrev = NULL;

	// Add actor to head of empty list of actors of this type
	poNext = gpoEmptyChild;
	gpoEmptyChild = this;
}

#pragma optimize( "", on )

void Child::WaitUntil( double target_time )
{
	//don't increase gdEventsForTabulation if special events because they are all treated as one event for tabulation purposes
	if (!gpoEventQueue->m_bSpecialEvents)
	{
		gdEventsForTabulation++;
	}
	__events = gdEventsForTabulation;
	if ( time < (TIME) target_time )
	{
		__time = (TIME) target_time;
		WaitSpecial( (TIME) target_time - time );
		poStateFunctions->Set_time( (TIME) target_time );
	}
}

Child *Child::UpdateTime(  )
{
	__events = gdEventsForTabulation;
	if ( time < (TIME) gdThreadCurrentTime ) {
		__time = (TIME) gdThreadCurrentTime;
		WaitSpecial( (TIME) gdThreadCurrentTime - time );
		poStateFunctions->Set_time( (TIME) gdThreadCurrentTime );
	}
	return this;
}

void Child::WaitUntilThisActor( double target_time )
{
	WaitUntil(target_time);
}

void Child::WaitSpecial( TIME wait_time )
{

	// increment the age
		poStateFunctions->Set_age( age + wait_time );
}

TIME Child::timeScheduleEvent()
{
	return schedule_event_time;
}

void Child::ScheduleEvent()
{

	if (poDerivedStates->sssplit_age_INFECTION_AGES_time == schedule_event_time)
	{
		CHKSUM(time, 10);
		TRCEVNT(case_seed, "Child", actor_id, "scheduled - 0", time);
		gszRuntimeEventName = "Child.scheduled - 0";
		poDerivedStates->Set_sssplit_age_INFECTION_AGES( poDerivedStates->sssplit_age_INFECTION_AGES + 1 );
		if ( poDerivedStates->sssplit_age_INFECTION_AGES < 13 ) {
		poDerivedStates->sssplit_age_INFECTION_AGES_time = WAIT(TIME(POINTS(INFECTION_AGES)[poDerivedStates->sssplit_age_INFECTION_AGES] - POINTS(INFECTION_AGES)[poDerivedStates->sssplit_age_INFECTION_AGES - 1]));
		}
		else
		{		poDerivedStates->sssplit_age_INFECTION_AGES_time = TIME_INFINITE;
		}
		schedule_event_time = poDerivedStates->sssplit_age_INFECTION_AGES_time;
	}
}

double Child::EventTime( int nEventNum, int *pnEventInfo )
{
	double	event_time = double( TIME_INFINITE );

	gdRuntimeTime = (double) time;
	gnRuntimeActorId = actor_id;
	switch( nEventNum ) {
		case 0:
			gszRuntimeEventName = "Child.MortalityEvent(time)";
			event_time = (double) timeMortalityEvent();
			TRCEVNT(case_seed, "Child", actor_id, "timeMortalityEvent", event_time);
			break;
		case 1:
			gszRuntimeEventName = "ScheduleEvent(time)";
			event_time = (double) timeScheduleEvent();
			break;
		case -1:
		default:;
	}
	return TIME( CoarsenMantissa( (TIME) event_time ) );
}

void Child::Implement( int nEventNum, int nEventInfo )
{
	gdThreadEvents += 1;
	gdRuntimeTime = (double) time;
	gnRuntimeActorId = actor_id;
	switch( nEventNum )
	{
		case 0:
			CHKSUM(time, 0);
			TRCEVNT(case_seed, "Child", actor_id, "Child.MortalityEvent", time);
			gszRuntimeEventName = "Child.MortalityEvent(implement)";
			MortalityEvent();
			break;
		case 1:
			ScheduleEvent();
			break;
		case -1:
		default:;
	}
}

double Child::Set_actor_subsample_weight( double value )
{
	return poStateFunctions->Set_actor_subsample_weight( value);
}

double Child::Set_actor_weight( double value )
{
	return poStateFunctions->Set_actor_weight( value);
}

TIME ChildStateFunctions::Set_age( TIME value, bool bStartFunction )
{

	if ( poParent->age != value || bStartFunction )
	{
		if (bStartFunction)
		{
			poDerivedStates->Set_sssplit_age_INFECTION_AGES( SPLIT(value, INFECTION_AGES) );
			if ( poDerivedStates->sssplit_age_INFECTION_AGES < 13 )
			{
				poDerivedStates->sssplit_age_INFECTION_AGES_time = poParent->time +  POINTS(INFECTION_AGES)[poDerivedStates->sssplit_age_INFECTION_AGES] - value ;

			}
			else
			{
				poDerivedStates->sssplit_age_INFECTION_AGES_time = TIME_INFINITE;
			}
			poParent->schedule_event_time = poDerivedStates->sssplit_age_INFECTION_AGES_time;
		}


		TIME	OldValue;
		OldValue = poParent->age;
		poParent->age = value;

	}
	return poParent->age = value;
}

logical ChildStateFunctions::Set_alive( logical value )
{

	value = value != 0;
	if ( poParent->alive != value )
	{
		if ( value == FALSE ) {
			poDerivedStates->Set_entrances_alive_FALSE( poDerivedStates->entrances_alive_FALSE + 1 );
		}


		logical	OldValue;
		OldValue = poParent->alive;
		poParent->alive = value;

#line 40 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
		Set_alive_value_in( poParent->alive );
#line 680 "ACTORS.CPP"
	}
	return poParent->alive = value;
}

DISEASE_STATE ChildStateFunctions::Set_mother_disease_status_fm( DISEASE_STATE value )
{

	CHKLMT(value, mother_disease_status_fm, 0, rc_size_DISEASE_STATE - 1);
	if ( value < 0 ) {
		value = (DISEASE_STATE) 0;
	}
	else if ( value >= rc_size_DISEASE_STATE ) {
		value = (DISEASE_STATE) ( rc_size_DISEASE_STATE - 1 );
	}
	if ( poParent->mother_disease_status_fm != value )
	{
		if ( value == INFECTED ) {
			poDerivedStates->Set_entrances_mother_disease_status_fm_INFECTED( poDerivedStates->entrances_mother_disease_status_fm_INFECTED + 1 );
		}


		DISEASE_STATE	OldValue;
		OldValue = poParent->mother_disease_status_fm;
		poParent->mother_disease_status_fm = value;

		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[5] >= 0 && ( poParent->tableinfo.tab5dim_filter || poParent->table_filter_Infection_count_fm ) ) {
				if ( poParent->tableinfo.tab5events == -1 || poParent->tableinfo.tab5events < poParent->__events ) {
					if ( poParent->tableinfo.tab5events != -1 ) {
						Set_table_unit_Infection_count_fm(poParent->table_unit_Infection_count_fm+1);
						poParent->tableinfo.tab5close0 = (real) poParent->table_unit_Infection_count_fm;
						poParent->tableinfo.tab5open0 = (real) poParent->CloseUpdate( 5, 0, poParent->tableinfo.tab5open0, poParent->tableinfo.tab5close0, poParent->tableinfo.tab5dim_filter, poParent->tableinfo.tab5cell, poParent->tableinfo.tab5total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab5events = poParent->__events;
					poParent->tableinfo.tab5close0 = (real) poParent->table_unit_Infection_count_fm;
					poParent->tableinfo.tab5cell = (index) poParent->TableCellIndex( 5, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab5total_cell = (index) poParent->tableinfo.tab5cell;
					}
					else {
						poParent->tableinfo.tab5total_cell = (index) poParent->TableTotalCellIndex( 5, 1,
						OldValue );
					}
					poParent->tableinfo.tab5dim_filter = poParent->table_filter_Infection_count_fm;
				}
			}
		}
		if ( poParent->poMortalityEvent->cRecalc == 0 ) {
			poParent->poMortalityEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poMortalityEvent );
		};
	}
	return poParent->mother_disease_status_fm = value;
}

REPORT_TIME ChildStateFunctions::Set_report_time( REPORT_TIME value )
{

	CHKLMT(value, report_time, r_min_REPORT_TIME, r_max_REPORT_TIME);
	if ( value < r_min_REPORT_TIME ) {
		value = r_min_REPORT_TIME;
	}
	else if ( value > r_max_REPORT_TIME ) {
		value = r_max_REPORT_TIME;
	}
	if ( poParent->report_time != value )
	{

		REPORT_TIME	OldValue;
		OldValue = poParent->report_time;
		poParent->report_time = value;
	}
	return poParent->report_time = value;
}

counter ChildStateFunctions::Set_table_unit_Infection_count_dl( counter value )
{

	if ( poParent->table_unit_Infection_count_dl != value )
	{

		counter	OldValue;
		OldValue = poParent->table_unit_Infection_count_dl;
		poParent->table_unit_Infection_count_dl = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[3] >= 0 ) {
				if ( poParent->tableinfo.tab3events == poParent->__events ) {
					poParent->tableinfo.tab3close0 = (real) poParent->table_unit_Infection_count_dl;
				}
			}
		}
	}
	return poParent->table_unit_Infection_count_dl = value;
}

counter ChildStateFunctions::Set_table_unit_Infection_count_fm( counter value )
{

	if ( poParent->table_unit_Infection_count_fm != value )
	{

		counter	OldValue;
		OldValue = poParent->table_unit_Infection_count_fm;
		poParent->table_unit_Infection_count_fm = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[5] >= 0 ) {
				if ( poParent->tableinfo.tab5events == poParent->__events ) {
					poParent->tableinfo.tab5close0 = (real) poParent->table_unit_Infection_count_fm;
				}
			}
		}
	}
	return poParent->table_unit_Infection_count_fm = value;
}

logical ChildStateFunctions::Set_actorset_filter_asAllChild( logical value )
{

	value = value != 0;
	if ( poParent->actorset_filter_asAllChild != value )
	{

		logical	OldValue;
		OldValue = poParent->actorset_filter_asAllChild;
		poParent->actorset_filter_asAllChild = value;
		// actorset filter
		if (poParent->actorset_filter_asAllChild)
		{
			poParent->poasAllChild = asAllChild->NewNode(poParent);
		}
		else
		{
			asAllChild->Remove(poParent->poasAllChild);
			poParent->poasAllChild = NULL;
		}
	}
	return poParent->actorset_filter_asAllChild = value;
}

logical ChildStateFunctions::Set_alive_value_in( logical value )
{

	value = value != 0;
	if ( poParent->alive_value_in != value )
	{

		logical	OldValue;
		OldValue = poParent->alive_value_in;
		poParent->alive_value_in = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[0] >= 0 ) {
				if ( poParent->tableinfo.tab0events == poParent->__events ) {
					poParent->tableinfo.tab0close0 = (real) poParent->alive_value_in;
				}
			}
			if ( gplTableIndexes[1] >= 0 ) {
				if ( poParent->tableinfo.tab1events == poParent->__events ) {
					poParent->tableinfo.tab1close0 = (real) poParent->alive_value_in;
				}
			}
		}
	}
	return poParent->alive_value_in = value;
}

TIME ChildStateFunctions::Set_duration__max_value_out( TIME value )
{

	if ( poParent->duration__max_value_out != value )
	{

		TIME	OldValue;
		OldValue = poParent->duration__max_value_out;
		poParent->duration__max_value_out = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[0] >= 0 ) {
				if ( poParent->tableinfo.tab0events == poParent->__events ) {
					poParent->tableinfo.tab0close2 = (real) poParent->duration__max_value_out;
				}
			}
			if ( gplTableIndexes[1] >= 0 ) {
				if ( poParent->tableinfo.tab1events == poParent->__events ) {
					poParent->tableinfo.tab1close2 = (real) poParent->duration__max_value_out;
				}
			}
		}
	}
	return poParent->duration__max_value_out = value;
}

TIME	ChildStateFunctions::Get_duration__max_value_out(){
return poDerivedStates->Get_duration_();
};

TIME ChildStateFunctions::Set_duration__min_value_out( TIME value )
{

	if ( poParent->duration__min_value_out != value )
	{

		TIME	OldValue;
		OldValue = poParent->duration__min_value_out;
		poParent->duration__min_value_out = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[0] >= 0 ) {
				if ( poParent->tableinfo.tab0events == poParent->__events ) {
					poParent->tableinfo.tab0close3 = (real) poParent->duration__min_value_out;
				}
			}
			if ( gplTableIndexes[1] >= 0 ) {
				if ( poParent->tableinfo.tab1events == poParent->__events ) {
					poParent->tableinfo.tab1close3 = (real) poParent->duration__min_value_out;
				}
			}
		}
	}
	return poParent->duration__min_value_out = value;
}

TIME	ChildStateFunctions::Get_duration__min_value_out(){
return poDerivedStates->Get_duration_();
};

DISEASE_STATE ChildStateFunctions::Set_mother_disease_status_dl( DISEASE_STATE value )
{

	CHKLMT(value, mother_disease_status_dl, 0, rc_size_DISEASE_STATE - 1);
	if ( value < 0 ) {
		value = (DISEASE_STATE) 0;
	}
	else if ( value >= rc_size_DISEASE_STATE ) {
		value = (DISEASE_STATE) ( rc_size_DISEASE_STATE - 1 );
	}
	if ( poParent->mother_disease_status_dl != value )
	{
		if ( value == INFECTED ) {
			poDerivedStates->Set_entrances_mother_disease_status_dl_INFECTED( poDerivedStates->entrances_mother_disease_status_dl_INFECTED + 1 );
		}
#line 45 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
		Set_table_filter_DurationOfLifeChildHM( value== FALSE );
#line 922 "ACTORS.CPP"


		DISEASE_STATE	OldValue;
		OldValue = poParent->mother_disease_status_dl;
		poParent->mother_disease_status_dl = value;

		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[3] >= 0 && ( poParent->tableinfo.tab3dim_filter || poParent->table_filter_Infection_count_dl ) ) {
				if ( poParent->tableinfo.tab3events == -1 || poParent->tableinfo.tab3events < poParent->__events ) {
					if ( poParent->tableinfo.tab3events != -1 ) {
						Set_table_unit_Infection_count_dl(poParent->table_unit_Infection_count_dl+1);
						poParent->tableinfo.tab3close0 = (real) poParent->table_unit_Infection_count_dl;
						poParent->tableinfo.tab3open0 = (real) poParent->CloseUpdate( 3, 0, poParent->tableinfo.tab3open0, poParent->tableinfo.tab3close0, poParent->tableinfo.tab3dim_filter, poParent->tableinfo.tab3cell, poParent->tableinfo.tab3total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab3events = poParent->__events;
					poParent->tableinfo.tab3close0 = (real) poParent->table_unit_Infection_count_dl;
					poParent->tableinfo.tab3cell = (index) poParent->TableCellIndex( 3, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab3total_cell = (index) poParent->tableinfo.tab3cell;
					}
					else {
						poParent->tableinfo.tab3total_cell = (index) poParent->TableTotalCellIndex( 3, 1,
						OldValue );
					}
					poParent->tableinfo.tab3dim_filter = poParent->table_filter_Infection_count_dl;
				}
			}
		}
	}
	return poParent->mother_disease_status_dl = value;
}

logical ChildStateFunctions::Set_table_filter_DurationOfLifeChild( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_DurationOfLifeChild != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_DurationOfLifeChild;
		poParent->table_filter_DurationOfLifeChild = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[0] >= 0 ) {
				if ( poParent->tableinfo.tab0events == -1 || poParent->tableinfo.tab0events < poParent->__events ) {
					if ( poParent->tableinfo.tab0events != -1 ) {
						poParent->tableinfo.tab0open0 = (real) poParent->CloseUpdate( 0, 0, poParent->tableinfo.tab0open0, poParent->tableinfo.tab0close0, poParent->tableinfo.tab0dim_filter, poParent->tableinfo.tab0cell, poParent->tableinfo.tab0total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab0open1 = (real) poParent->CloseUpdate( 0, 1, poParent->tableinfo.tab0open1, poParent->tableinfo.tab0close1, poParent->tableinfo.tab0dim_filter, poParent->tableinfo.tab0cell, poParent->tableinfo.tab0total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab0open2 = (real) poParent->CloseUpdate( 0, 2, poParent->tableinfo.tab0open2, poParent->tableinfo.tab0close2, poParent->tableinfo.tab0dim_filter, poParent->tableinfo.tab0cell, poParent->tableinfo.tab0total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab0open3 = (real) poParent->CloseUpdate( 0, 3, poParent->tableinfo.tab0open3, poParent->tableinfo.tab0close3, poParent->tableinfo.tab0dim_filter, poParent->tableinfo.tab0cell, poParent->tableinfo.tab0total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab0events = poParent->__events;
					poParent->tableinfo.tab0close0 = (real) poParent->alive_value_in;
					poParent->tableinfo.tab0close1 = (real) poDerivedStates->Get_duration_();
					poParent->tableinfo.tab0close2 = (real) Get_duration__max_value_out();
					poParent->tableinfo.tab0close3 = (real) Get_duration__min_value_out();
					poParent->tableinfo.tab0cell = (index) poParent->TableCellIndex( 0, 0 );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab0total_cell = (index) poParent->tableinfo.tab0cell;
					}
					else {
						poParent->tableinfo.tab0total_cell = (index) poParent->TableTotalCellIndex( 0, 0 );
					}
					poParent->tableinfo.tab0dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_DurationOfLifeChild = value;
}

logical ChildStateFunctions::Set_table_filter_DurationOfLifeChildHM( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_DurationOfLifeChildHM != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_DurationOfLifeChildHM;
		poParent->table_filter_DurationOfLifeChildHM = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[1] >= 0 ) {
				if ( poParent->tableinfo.tab1events == -1 || poParent->tableinfo.tab1events < poParent->__events ) {
					if ( poParent->tableinfo.tab1events != -1 ) {
						poParent->tableinfo.tab1open0 = (real) poParent->CloseUpdate( 1, 0, poParent->tableinfo.tab1open0, poParent->tableinfo.tab1close0, poParent->tableinfo.tab1dim_filter, poParent->tableinfo.tab1cell, poParent->tableinfo.tab1total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab1open1 = (real) poParent->CloseUpdate( 1, 1, poParent->tableinfo.tab1open1, poParent->tableinfo.tab1close1, poParent->tableinfo.tab1dim_filter, poParent->tableinfo.tab1cell, poParent->tableinfo.tab1total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab1open2 = (real) poParent->CloseUpdate( 1, 2, poParent->tableinfo.tab1open2, poParent->tableinfo.tab1close2, poParent->tableinfo.tab1dim_filter, poParent->tableinfo.tab1cell, poParent->tableinfo.tab1total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab1open3 = (real) poParent->CloseUpdate( 1, 3, poParent->tableinfo.tab1open3, poParent->tableinfo.tab1close3, poParent->tableinfo.tab1dim_filter, poParent->tableinfo.tab1cell, poParent->tableinfo.tab1total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab1events = poParent->__events;
					poParent->tableinfo.tab1close0 = (real) poParent->alive_value_in;
					poParent->tableinfo.tab1close1 = (real) poDerivedStates->Get_duration_();
					poParent->tableinfo.tab1close2 = (real) Get_duration__max_value_out();
					poParent->tableinfo.tab1close3 = (real) Get_duration__min_value_out();
					poParent->tableinfo.tab1cell = (index) poParent->TableCellIndex( 1, 0 );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab1total_cell = (index) poParent->tableinfo.tab1cell;
					}
					else {
						poParent->tableinfo.tab1total_cell = (index) poParent->TableTotalCellIndex( 1, 0 );
					}
					poParent->tableinfo.tab1dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_DurationOfLifeChildHM = value;
}

logical ChildStateFunctions::Set_table_filter_Infection_count_dl( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_Infection_count_dl != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_Infection_count_dl;
		poParent->table_filter_Infection_count_dl = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[3] >= 0 ) {
				if ( poParent->tableinfo.tab3events == -1 || poParent->tableinfo.tab3events < poParent->__events ) {
					if ( poParent->tableinfo.tab3events != -1 ) {
						Set_table_unit_Infection_count_dl(poParent->table_unit_Infection_count_dl+1);
						poParent->tableinfo.tab3close0 = (real) poParent->table_unit_Infection_count_dl;
						poParent->tableinfo.tab3open0 = (real) poParent->CloseUpdate( 3, 0, poParent->tableinfo.tab3open0, poParent->tableinfo.tab3close0, poParent->tableinfo.tab3dim_filter, poParent->tableinfo.tab3cell, poParent->tableinfo.tab3total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab3events = poParent->__events;
					poParent->tableinfo.tab3close0 = (real) poParent->table_unit_Infection_count_dl;
					poParent->tableinfo.tab3cell = (index) poParent->TableCellIndex( 3, 1,
						poParent->mother_disease_status_dl );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab3total_cell = (index) poParent->tableinfo.tab3cell;
					}
					else {
						poParent->tableinfo.tab3total_cell = (index) poParent->TableTotalCellIndex( 3, 1,
						poParent->mother_disease_status_dl );
					}
					poParent->tableinfo.tab3dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_Infection_count_dl = value;
}

logical ChildStateFunctions::Set_table_filter_Infection_count_fm( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_Infection_count_fm != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_Infection_count_fm;
		poParent->table_filter_Infection_count_fm = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[5] >= 0 ) {
				if ( poParent->tableinfo.tab5events == -1 || poParent->tableinfo.tab5events < poParent->__events ) {
					if ( poParent->tableinfo.tab5events != -1 ) {
						Set_table_unit_Infection_count_fm(poParent->table_unit_Infection_count_fm+1);
						poParent->tableinfo.tab5close0 = (real) poParent->table_unit_Infection_count_fm;
						poParent->tableinfo.tab5open0 = (real) poParent->CloseUpdate( 5, 0, poParent->tableinfo.tab5open0, poParent->tableinfo.tab5close0, poParent->tableinfo.tab5dim_filter, poParent->tableinfo.tab5cell, poParent->tableinfo.tab5total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab5events = poParent->__events;
					poParent->tableinfo.tab5close0 = (real) poParent->table_unit_Infection_count_fm;
					poParent->tableinfo.tab5cell = (index) poParent->TableCellIndex( 5, 1,
						poParent->mother_disease_status_fm );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab5total_cell = (index) poParent->tableinfo.tab5cell;
					}
					else {
						poParent->tableinfo.tab5total_cell = (index) poParent->TableTotalCellIndex( 5, 1,
						poParent->mother_disease_status_fm );
					}
					poParent->tableinfo.tab5dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_Infection_count_fm = value;
}

logical ChildStateFunctions::Set_table_filter_InfectionAgeHist_dl( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_InfectionAgeHist_dl != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_InfectionAgeHist_dl;
		poParent->table_filter_InfectionAgeHist_dl = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[2] >= 0 ) {
				if ( poParent->tableinfo.tab2events == -1 || poParent->tableinfo.tab2events < poParent->__events ) {
					if ( poParent->tableinfo.tab2events != -1 ) {
						poParent->tableinfo.tab2open0 = (real) poParent->CloseUpdate( 2, 0, poParent->tableinfo.tab2open0, poParent->tableinfo.tab2close0, poParent->tableinfo.tab2dim_filter, poParent->tableinfo.tab2cell, poParent->tableinfo.tab2total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab2open1 = (real) poParent->CloseUpdate( 2, 1, poParent->tableinfo.tab2open1, poParent->tableinfo.tab2close1, poParent->tableinfo.tab2dim_filter, poParent->tableinfo.tab2cell, poParent->tableinfo.tab2total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab2events = poParent->__events;
					poParent->tableinfo.tab2close0 = (real) poDerivedStates->entrances_alive_FALSE;
					poParent->tableinfo.tab2close1 = (real) poDerivedStates->entrances_mother_disease_status_dl_INFECTED;
					poParent->tableinfo.tab2cell = (index) poParent->TableCellIndex( 2, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab2total_cell = (index) poParent->tableinfo.tab2cell;
					}
					else {
						poParent->tableinfo.tab2total_cell = (index) poParent->TableTotalCellIndex( 2, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					}
					poParent->tableinfo.tab2dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_InfectionAgeHist_dl = value;
}

logical ChildStateFunctions::Set_table_filter_InfectionAgeHist_fm( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_InfectionAgeHist_fm != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_InfectionAgeHist_fm;
		poParent->table_filter_InfectionAgeHist_fm = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[4] >= 0 ) {
				if ( poParent->tableinfo.tab4events == -1 || poParent->tableinfo.tab4events < poParent->__events ) {
					if ( poParent->tableinfo.tab4events != -1 ) {
						poParent->tableinfo.tab4open0 = (real) poParent->CloseUpdate( 4, 0, poParent->tableinfo.tab4open0, poParent->tableinfo.tab4close0, poParent->tableinfo.tab4dim_filter, poParent->tableinfo.tab4cell, poParent->tableinfo.tab4total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab4open1 = (real) poParent->CloseUpdate( 4, 1, poParent->tableinfo.tab4open1, poParent->tableinfo.tab4close1, poParent->tableinfo.tab4dim_filter, poParent->tableinfo.tab4cell, poParent->tableinfo.tab4total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab4events = poParent->__events;
					poParent->tableinfo.tab4close0 = (real) poDerivedStates->entrances_alive_FALSE;
					poParent->tableinfo.tab4close1 = (real) poDerivedStates->entrances_mother_disease_status_fm_INFECTED;
					poParent->tableinfo.tab4cell = (index) poParent->TableCellIndex( 4, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab4total_cell = (index) poParent->tableinfo.tab4cell;
					}
					else {
						poParent->tableinfo.tab4total_cell = (index) poParent->TableTotalCellIndex( 4, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					}
					poParent->tableinfo.tab4dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_InfectionAgeHist_fm = value;
}

DISEASE_STATE ChildStateFunctions::Set_lMother_disease_status( DISEASE_STATE value )
{

	CHKLMT(value, lMother_disease_status, 0, rc_size_DISEASE_STATE - 1);
	if ( value < 0 ) {
		value = (DISEASE_STATE) 0;
	}
	else if ( value >= rc_size_DISEASE_STATE ) {
		value = (DISEASE_STATE) ( rc_size_DISEASE_STATE - 1 );
	}
	if ( poParent->lMother_disease_status != value )
	{


		DISEASE_STATE	OldValue;
		OldValue = poParent->lMother_disease_status;
		poParent->lMother_disease_status = value;

#line 6 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/ChildCore.mpp"
		Set_mother_disease_status_dl( poParent->lMother_disease_status );
#line 1202 "ACTORS.CPP"
	}
	return poParent->lMother_disease_status = value;
}

TIME ChildDerivedStates::Set_duration_( TIME value )
{

	if ( duration_ != value ) {

		TIME OldValue;
		OldValue = duration_;
		duration_ = value;
		poStateFunctions->Set_duration__max_value_out( duration_ );
		poStateFunctions->Set_duration__min_value_out( duration_ );
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[0] >= 0 ) {
				if ( poParent->tableinfo.tab0events == poParent->__events ) {
					poParent->tableinfo.tab0close1 = (real) duration_;
				}
			}
			if ( gplTableIndexes[1] >= 0 ) {
				if ( poParent->tableinfo.tab1events == poParent->__events ) {
					poParent->tableinfo.tab1close1 = (real) duration_;
				}
			}
		}
	}
	return duration_ = value;
}

counter ChildDerivedStates::Set_entrances_alive_FALSE( counter value )
{

	if ( entrances_alive_FALSE != value ) {

counter		OldValue;
		OldValue = entrances_alive_FALSE;
		entrances_alive_FALSE = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[2] >= 0 ) {
				if ( poParent->tableinfo.tab2events == poParent->__events ) {
					poParent->tableinfo.tab2close0 = (real) entrances_alive_FALSE;
				}
			}
			if ( gplTableIndexes[4] >= 0 ) {
				if ( poParent->tableinfo.tab4events == poParent->__events ) {
					poParent->tableinfo.tab4close0 = (real) entrances_alive_FALSE;
				}
			}
		}
	}
	return entrances_alive_FALSE = value;
}

counter ChildDerivedStates::Set_entrances_mother_disease_status_dl_INFECTED( counter value )
{

	if ( entrances_mother_disease_status_dl_INFECTED != value ) {

counter		OldValue;
		OldValue = entrances_mother_disease_status_dl_INFECTED;
		entrances_mother_disease_status_dl_INFECTED = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[2] >= 0 ) {
				if ( poParent->tableinfo.tab2events == poParent->__events ) {
					poParent->tableinfo.tab2close1 = (real) entrances_mother_disease_status_dl_INFECTED;
				}
			}
		}
	}
	return entrances_mother_disease_status_dl_INFECTED = value;
}

counter ChildDerivedStates::Set_entrances_mother_disease_status_fm_INFECTED( counter value )
{

	if ( entrances_mother_disease_status_fm_INFECTED != value ) {

counter		OldValue;
		OldValue = entrances_mother_disease_status_fm_INFECTED;
		entrances_mother_disease_status_fm_INFECTED = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[4] >= 0 ) {
				if ( poParent->tableinfo.tab4events == poParent->__events ) {
					poParent->tableinfo.tab4close1 = (real) entrances_mother_disease_status_fm_INFECTED;
				}
			}
		}
	}
	return entrances_mother_disease_status_fm_INFECTED = value;
}

INFECTION_AGES ChildDerivedStates::Set_sssplit_age_INFECTION_AGES( INFECTION_AGES value )
{

	if ( sssplit_age_INFECTION_AGES != value ) {

INFECTION_AGES		OldValue;
		OldValue = sssplit_age_INFECTION_AGES;
		sssplit_age_INFECTION_AGES = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[2] >= 0 && ( poParent->tableinfo.tab2dim_filter || poParent->table_filter_InfectionAgeHist_dl ) ) {
				if ( poParent->tableinfo.tab2events == -1 || poParent->tableinfo.tab2events < poParent->__events ) {
					if ( poParent->tableinfo.tab2events != -1 ) {
						poParent->tableinfo.tab2open0 = (real) poParent->CloseUpdate( 2, 0, poParent->tableinfo.tab2open0, poParent->tableinfo.tab2close0, poParent->tableinfo.tab2dim_filter, poParent->tableinfo.tab2cell, poParent->tableinfo.tab2total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab2open1 = (real) poParent->CloseUpdate( 2, 1, poParent->tableinfo.tab2open1, poParent->tableinfo.tab2close1, poParent->tableinfo.tab2dim_filter, poParent->tableinfo.tab2cell, poParent->tableinfo.tab2total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab2events = poParent->__events;
					poParent->tableinfo.tab2close0 = (real) entrances_alive_FALSE;
					poParent->tableinfo.tab2close1 = (real) entrances_mother_disease_status_dl_INFECTED;
					poParent->tableinfo.tab2cell = (index) poParent->TableCellIndex( 2, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab2total_cell = (index) poParent->tableinfo.tab2cell;
					}
					else {
						poParent->tableinfo.tab2total_cell = (index) poParent->TableTotalCellIndex( 2, 1,
						OldValue );
					}
					poParent->tableinfo.tab2dim_filter = poParent->table_filter_InfectionAgeHist_dl;
				}
			}
			if ( gplTableIndexes[4] >= 0 && ( poParent->tableinfo.tab4dim_filter || poParent->table_filter_InfectionAgeHist_fm ) ) {
				if ( poParent->tableinfo.tab4events == -1 || poParent->tableinfo.tab4events < poParent->__events ) {
					if ( poParent->tableinfo.tab4events != -1 ) {
						poParent->tableinfo.tab4open0 = (real) poParent->CloseUpdate( 4, 0, poParent->tableinfo.tab4open0, poParent->tableinfo.tab4close0, poParent->tableinfo.tab4dim_filter, poParent->tableinfo.tab4cell, poParent->tableinfo.tab4total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab4open1 = (real) poParent->CloseUpdate( 4, 1, poParent->tableinfo.tab4open1, poParent->tableinfo.tab4close1, poParent->tableinfo.tab4dim_filter, poParent->tableinfo.tab4cell, poParent->tableinfo.tab4total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab4events = poParent->__events;
					poParent->tableinfo.tab4close0 = (real) entrances_alive_FALSE;
					poParent->tableinfo.tab4close1 = (real) entrances_mother_disease_status_fm_INFECTED;
					poParent->tableinfo.tab4cell = (index) poParent->TableCellIndex( 4, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab4total_cell = (index) poParent->tableinfo.tab4cell;
					}
					else {
						poParent->tableinfo.tab4total_cell = (index) poParent->TableTotalCellIndex( 4, 1,
						OldValue );
					}
					poParent->tableinfo.tab4dim_filter = poParent->table_filter_InfectionAgeHist_fm;
				}
			}
		}
	}
	return sssplit_age_INFECTION_AGES = value;
}

void *Mother::operator new( size_t count )
{
	Mother *poActor;
	if ( gpoEmptyMother != NULL ) {
		poActor = gpoEmptyMother;
		gpoEmptyMother = (Mother *) gpoEmptyMother->poNext;
	}
	else {
		poActor = ::new Mother( TRUE ) ;
	}
	poActor->__finished = false;
	return poActor;
}

void Mother::InitActor()
{
	poDerivedStates = new MotherDerivedStates();
	poDerivedStates->poParent = this;
	poStateFunctions = new MotherStateFunctions();
	poStateFunctions->poParent = this;
	poStateFunctions->poDerivedStates = poDerivedStates;
	poDerivedStates->poStateFunctions = poStateFunctions;
	bUpdates = FALSE;
	nActorNumber = 1;
	InitializeStates();
	mlChild = new Mother_mlChild( this );
}

void Mother::DeleteActor()
{
	delete poDerivedStates;
	delete poStateFunctions;
	delete mlChild;
}

#pragma optimize( "", off )

void Mother::InitializeStates()
{
	// initialization of states
	actor_id = (long) 0;
	actor_subsample_weight = (double) 0;
	actor_weight = (double) 0;
	age = (TIME) 0;
#line 37 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	alive = TRUE != 0;
#line 1397 "ACTORS.CPP"
	case_id = (long) 0;
	case_seed = (double) 0;
#line 34 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	disease_status = HEALTHY;
#line 1402 "ACTORS.CPP"
#line 16 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MMortality.mpp"
	inf_mortality_draw = (double) -1;
#line 1405 "ACTORS.CPP"
#line 15 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MMortality.mpp"
	inf_mortality_rng = FALSE != 0;
#line 1408 "ACTORS.CPP"
#line 23 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MFertility.mpp"
	CHKLMT(0, number_children, r_min_LIFE, r_max_LIFE);
	number_children = 0 < r_min_LIFE ? r_min_LIFE : 0 > r_max_LIFE ? r_max_LIFE : (LIFE) 0;
#line 1412 "ACTORS.CPP"
#line 35 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	CHKLMT(0, report_time, r_min_REPORT_TIME, r_max_REPORT_TIME);
	report_time = 0 < r_min_REPORT_TIME ? r_min_REPORT_TIME : 0 > r_max_REPORT_TIME ? r_max_REPORT_TIME : (REPORT_TIME) 0;
#line 1416 "ACTORS.CPP"
	table_unit_FertDetails = (counter) 0;
	time = (TIME) gdThreadCurrentTime;
	actorset_filter_asAllMother = FALSE;
	alive_value_in = FALSE;
	poDerivedStates->changes_number_children = (counter) 0;
	poDerivedStates->duration_ = (TIME) 0;
	poDerivedStates->duration__time = (TIME) gdThreadCurrentTime;
	duration__max_value_out = (TIME) 0;
	duration__min_value_out = (TIME) 0;
	poDerivedStates->entrances_alive_FALSE = (counter) 0;
	poDerivedStates->entrances_disease_status_INFECTED = (counter) 0;
	entrances_disease_status_INFECTED_event = (counter) 0;
	final_tracking = FALSE;
	integer_age = r_min_LIFE;
	number_children_link = r_min_LIFE;
	poDerivedStates->ssint_age = (counter) 0;
	poDerivedStates->ssint_age_time = (TIME) TIME_INFINITE;
	poDerivedStates->sssplit_age_INFECTION_AGES = (INFECTION_AGES) 0;
	poDerivedStates->sssplit_age_INFECTION_AGES_time = (TIME) TIME_INFINITE;
	table_filter_ageAtBirth = FALSE;
	table_filter_DurationOfLife = FALSE;
	table_filter_FertDetails = FALSE;
	table_filter_FertDetailsLink = FALSE;
	table_filter_FertDetailsSum = FALSE;
	table_filter_InfectionAge = FALSE;
	table_filter_InfectionAgeHist = FALSE;
	tracking = FALSE;
	poDerivedStates->value_at_entrances_alive_FALSE_number_children = (integer) 0;
	poDerivedStates->value_at_entrances_alive_FALSE_number_children_link = (integer) 0;
	poDerivedStates->value_at_entrances_disease_status_INFECTED_age = (real) 0;
	value_at_entrances_disease_status_INFECTED_age_event = (real) 0;
	count_mlChild = (counter) 0;
	__finished = FALSE;
	__time = (TIME) gdThreadCurrentTime;
	__events = 0;
}

#pragma optimize( "", on )

void Mother::UpdateLinkedStates()
{
	if ( mlChild != NULL ) {
		poStateFunctions->Set_count_mlChild( mlChild->Count() );
	}
	else {
		poStateFunctions->Set_count_mlChild( counter( 0 ) );
	}
}

void Mother::StartSpecial1()
{
	nSubSample = GetSubSample();
	InitializeStates();
	case_seed = GetCaseSeed();
	case_id = GetCaseID();
	lActorId = actor_id = GetObjectID();
	GetCaseWeight( &actor_weight, &actor_subsample_weight );
	mlChild->RemoveAll();

	// initialization of actor set nodes
	poasAllMother = NULL;

	// initialization of events
	poBirthEvent = gpoEventQueue->NewEvent( this, 0, 0 );
	poInfectionEvent = gpoEventQueue->NewEvent( this, 1, 0 );
	poInfMortRNGEvent = gpoEventQueue->NewEvent( this, 2, 0 );
	poMortalityEvent = gpoEventQueue->NewEvent( this, 3, 0 );
	poScheduleEvent = gpoEventQueue->NewEvent( this, 4, 255 );
	schedule_event_time = TIME_INFINITE;
	//Initalize derived states
#line 59 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_alive_value_in( logical( alive ) );
#line 1489 "ACTORS.CPP"
#line 59 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_duration__max_value_out( TIME( poDerivedStates->duration_ ) );
#line 1492 "ACTORS.CPP"
#line 59 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_duration__min_value_out( TIME( poDerivedStates->duration_ ) );
#line 1495 "ACTORS.CPP"
#line 69 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_entrances_disease_status_INFECTED_event( counter( poDerivedStates->entrances_disease_status_INFECTED ) );
#line 1498 "ACTORS.CPP"
#line 83 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_final_tracking( logical( GetReplicate()== 0 ) );
#line 1501 "ACTORS.CPP"
#line 36 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_integer_age( LIFE( poDerivedStates->ssint_age ) );
#line 1504 "ACTORS.CPP"
	integer_age = integer_age < r_min_LIFE ? r_min_LIFE : integer_age > r_max_LIFE ? r_max_LIFE : integer_age;
#line 24 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MFertility.mpp"
	poStateFunctions->Set_number_children_link( LIFE( count_mlChild ) );
#line 1508 "ACTORS.CPP"
	number_children_link = number_children_link < r_min_LIFE ? r_min_LIFE : number_children_link > r_max_LIFE ? r_max_LIFE : number_children_link;
#line 83 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_tracking( logical( actor_id% 50== 0 ) );
#line 1512 "ACTORS.CPP"
#line 69 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_value_at_entrances_disease_status_INFECTED_age_event( real( poDerivedStates->value_at_entrances_disease_status_INFECTED_age ) );
#line 1515 "ACTORS.CPP"
	CountActor(1);
}

#pragma optimize( "", off )

void Mother::StartSpecial2()
{
	// initialization of expression and history states
	poStateFunctions->Set_time(TIME( CoarsenMantissa( time ) ), true);
	poStateFunctions->Set_age(TIME( CoarsenMantissa( age ) ), true);
	gdThreadCurrentTime = time;
	__time = time;
#line 30 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_actorset_filter_asAllMother( logical( 1 ) );
#line 1530 "ACTORS.CPP"
	poDerivedStates->Set_changes_number_children( (counter) 0 );
	poDerivedStates->Set_duration_( (TIME) 0 );
	poDerivedStates->duration__time = time;
	poDerivedStates->Set_entrances_alive_FALSE( (counter) 0 );
	poDerivedStates->Set_entrances_disease_status_INFECTED( (counter) 0 );
#line 54 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MFertility.mpp"
	poStateFunctions->Set_table_filter_ageAtBirth( logical( 1 ) );
#line 1538 "ACTORS.CPP"
#line 53 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_table_filter_DurationOfLife( logical( 1 ) );
#line 1541 "ACTORS.CPP"
#line 65 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MFertility.mpp"
	poStateFunctions->Set_table_filter_FertDetails( logical( alive== FALSE ) );
#line 1544 "ACTORS.CPP"
#line 74 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MFertility.mpp"
	poStateFunctions->Set_table_filter_FertDetailsLink( logical( 1 ) );
#line 1547 "ACTORS.CPP"
#line 81 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MFertility.mpp"
	poStateFunctions->Set_table_filter_FertDetailsSum( logical( 1 ) );
#line 1550 "ACTORS.CPP"
#line 63 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_table_filter_InfectionAge( logical( 1 ) );
#line 1553 "ACTORS.CPP"
#line 72 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_table_filter_InfectionAgeHist( logical( 1 ) );
#line 1556 "ACTORS.CPP"
	poDerivedStates->Set_value_at_entrances_alive_FALSE_number_children( (integer) 0 );
	poDerivedStates->Set_value_at_entrances_alive_FALSE_number_children_link( (integer) 0 );
	poDerivedStates->Set_value_at_entrances_disease_status_INFECTED_age( (real) 0 );

	// initialization of cross-tabulation updates
	tableinfo.tab6events = -1;
	tableinfo.tab6open0 = (real) ( poDerivedStates->changes_number_children );
	tableinfo.tab7events = -1;
	tableinfo.tab7open0 = (real) ( table_unit_FertDetails );
	tableinfo.tab8events = -1;
	tableinfo.tab8open0 = (real) ( poDerivedStates->value_at_entrances_alive_FALSE_number_children_link );
	tableinfo.tab9events = -1;
	tableinfo.tab9open0 = (real) ( poDerivedStates->value_at_entrances_alive_FALSE_number_children );
	tableinfo.tab10events = -1;
	tableinfo.tab10open0 = (real) ( alive_value_in );
	tableinfo.tab10open1 = (real) ( poDerivedStates->duration_ );
	tableinfo.tab10open2 = (real) ( duration__max_value_out );
	tableinfo.tab10open3 = (real) ( duration__min_value_out );
	tableinfo.tab11events = -1;
	tableinfo.tab11open0 = (real) ( entrances_disease_status_INFECTED_event );
	tableinfo.tab11prev0 = (real) ( entrances_disease_status_INFECTED_event );
	tableinfo.tab11events0 = -1;
	tableinfo.tab11open1 = (real) ( value_at_entrances_disease_status_INFECTED_age_event );
	tableinfo.tab11prev1 = (real) ( value_at_entrances_disease_status_INFECTED_age_event );
	tableinfo.tab11events1 = -1;
	tableinfo.tab12events = -1;
	tableinfo.tab12open0 = (real) ( poDerivedStates->entrances_alive_FALSE );
	tableinfo.tab12open1 = (real) ( poDerivedStates->entrances_disease_status_INFECTED );

	// output initial values of tracked states and links
	if ( tracking ) {
		TrackChange( actor_id, (double) __time, 35, case_id );
		TrackChange( actor_id, (double) __time, 65, 1.0 );
		TrackChange( actor_id, (double) __time, 30, actor_id );
		TrackChange( actor_id, (double) __time, 31, actor_subsample_weight );
		TrackChange( actor_id, (double) __time, 32, actor_weight );
		TrackChange( actor_id, (double) __time, 33, age );
		TrackChange( actor_id, (double) __time, 34, alive );
		TrackChange( actor_id, (double) __time, 36, case_seed );
		TrackChange( actor_id, (double) __time, 37, disease_status );
		TrackChange( actor_id, (double) __time, 38, inf_mortality_draw );
		TrackChange( actor_id, (double) __time, 39, inf_mortality_rng );
		TrackChange( actor_id, (double) __time, 40, number_children );
		TrackChange( actor_id, (double) __time, 41, report_time );
		TrackChange( actor_id, (double) __time, 43, time );
		TrackChange( actor_id, (double) __time, 53, final_tracking );
		TrackChange( actor_id, (double) __time, 54, integer_age );
		TrackChange( actor_id, (double) __time, 55, number_children_link );
		TrackChange( actor_id, (double) __time, 65, tracking );
	}
	poPrev = NULL;
	poNext = gpoEventQueue->poHeadActor;
	if ( gpoEventQueue->poHeadActor != NULL ) {
		gpoEventQueue->poHeadActor->poPrev = this;
	}
	gpoEventQueue->poHeadActor = this;
	bUpdates = TRUE;
}

void Mother::FinishSpecial()
{
	CountFinishedActor(1);
	poStateFunctions->Set_tracking( FALSE );
	if ( final_tracking && gpModelApp->m_dSimulationEnd == UNUSED_VALUE ) {
			MarkTrackedObject( actor_id );
	}
	bUpdates = FALSE;
	gpoEventQueue->DestroyEvent( poBirthEvent );
	gpoEventQueue->DestroyEvent( poInfectionEvent );
	gpoEventQueue->DestroyEvent( poInfMortRNGEvent );
	gpoEventQueue->DestroyEvent( poMortalityEvent );
	gpoEventQueue->DestroyEvent(poScheduleEvent);

	// close all updates
	if ( gplTableIndexes[6] >= 0 && ( tableinfo.tab6dim_filter || table_filter_ageAtBirth ) ) {
		if ( tableinfo.tab6events != -1 ) {
			tableinfo.tab6open0 = (real) CloseUpdate( 6, 0, tableinfo.tab6open0, tableinfo.tab6close0, tableinfo.tab6dim_filter, tableinfo.tab6cell, tableinfo.tab6total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab6cell = (index) TableCellIndex( 6, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab6total_cell = (index) tableinfo.tab6cell;
		}
		else {
			tableinfo.tab6total_cell = (index) TableTotalCellIndex( 6, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		}
		CloseUpdate( 6, 0, tableinfo.tab6open0, poDerivedStates->changes_number_children, table_filter_ageAtBirth, tableinfo.tab6cell, tableinfo.tab6total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[7] >= 0 && ( tableinfo.tab7dim_filter || table_filter_FertDetails ) ) {
		if ( tableinfo.tab7events != -1 ) {
			poStateFunctions->Set_table_unit_FertDetails(table_unit_FertDetails+1);
			tableinfo.tab7close0 = (real) table_unit_FertDetails;
			tableinfo.tab7open0 = (real) CloseUpdate( 7, 0, tableinfo.tab7open0, tableinfo.tab7close0, tableinfo.tab7dim_filter, tableinfo.tab7cell, tableinfo.tab7total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab7cell = (index) TableCellIndex( 7, 1,
			number_children - r_min_LIFE );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab7total_cell = (index) tableinfo.tab7cell;
		}
		else {
			tableinfo.tab7total_cell = (index) TableTotalCellIndex( 7, 1,
			number_children - r_min_LIFE );
		}
		poStateFunctions->Set_table_unit_FertDetails(table_unit_FertDetails+1);
		CloseUpdate( 7, 0, tableinfo.tab7open0, table_unit_FertDetails, table_filter_FertDetails, tableinfo.tab7cell, tableinfo.tab7total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[8] >= 0 && ( tableinfo.tab8dim_filter || table_filter_FertDetailsLink ) ) {
		if ( tableinfo.tab8events != -1 ) {
			tableinfo.tab8open0 = (real) CloseUpdate( 8, 0, tableinfo.tab8open0, tableinfo.tab8close0, tableinfo.tab8dim_filter, tableinfo.tab8cell, tableinfo.tab8total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab8cell = (index) TableCellIndex( 8, 0 );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab8total_cell = (index) tableinfo.tab8cell;
		}
		else {
			tableinfo.tab8total_cell = (index) TableTotalCellIndex( 8, 0 );
		}
		CloseUpdate( 8, 0, tableinfo.tab8open0, poDerivedStates->value_at_entrances_alive_FALSE_number_children_link, table_filter_FertDetailsLink, tableinfo.tab8cell, tableinfo.tab8total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[9] >= 0 && ( tableinfo.tab9dim_filter || table_filter_FertDetailsSum ) ) {
		if ( tableinfo.tab9events != -1 ) {
			tableinfo.tab9open0 = (real) CloseUpdate( 9, 0, tableinfo.tab9open0, tableinfo.tab9close0, tableinfo.tab9dim_filter, tableinfo.tab9cell, tableinfo.tab9total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab9cell = (index) TableCellIndex( 9, 0 );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab9total_cell = (index) tableinfo.tab9cell;
		}
		else {
			tableinfo.tab9total_cell = (index) TableTotalCellIndex( 9, 0 );
		}
		CloseUpdate( 9, 0, tableinfo.tab9open0, poDerivedStates->value_at_entrances_alive_FALSE_number_children, table_filter_FertDetailsSum, tableinfo.tab9cell, tableinfo.tab9total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[10] >= 0 && ( tableinfo.tab10dim_filter || table_filter_DurationOfLife ) ) {
		if ( tableinfo.tab10events != -1 ) {
			tableinfo.tab10open0 = (real) CloseUpdate( 10, 0, tableinfo.tab10open0, tableinfo.tab10close0, tableinfo.tab10dim_filter, tableinfo.tab10cell, tableinfo.tab10total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab10open1 = (real) CloseUpdate( 10, 1, tableinfo.tab10open1, tableinfo.tab10close1, tableinfo.tab10dim_filter, tableinfo.tab10cell, tableinfo.tab10total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab10open2 = (real) CloseUpdate( 10, 2, tableinfo.tab10open2, tableinfo.tab10close2, tableinfo.tab10dim_filter, tableinfo.tab10cell, tableinfo.tab10total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab10open3 = (real) CloseUpdate( 10, 3, tableinfo.tab10open3, tableinfo.tab10close3, tableinfo.tab10dim_filter, tableinfo.tab10cell, tableinfo.tab10total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab10cell = (index) TableCellIndex( 10, 0 );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab10total_cell = (index) tableinfo.tab10cell;
		}
		else {
			tableinfo.tab10total_cell = (index) TableTotalCellIndex( 10, 0 );
		}
		CloseUpdate( 10, 0, tableinfo.tab10open0, alive_value_in, table_filter_DurationOfLife, tableinfo.tab10cell, tableinfo.tab10total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 10, 1, tableinfo.tab10open1, poDerivedStates->Get_duration_(), table_filter_DurationOfLife, tableinfo.tab10cell, tableinfo.tab10total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 10, 2, tableinfo.tab10open2, poStateFunctions->Get_duration__max_value_out(), table_filter_DurationOfLife, tableinfo.tab10cell, tableinfo.tab10total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 10, 3, tableinfo.tab10open3, poStateFunctions->Get_duration__min_value_out(), table_filter_DurationOfLife, tableinfo.tab10cell, tableinfo.tab10total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[11] >= 0 && ( tableinfo.tab11dim_filter || table_filter_InfectionAge ) ) {
		if ( tableinfo.tab11events != -1 ) {
			tableinfo.tab11open0 = (real) CloseUpdate( 11, 0, tableinfo.tab11open0, tableinfo.tab11close0, tableinfo.tab11dim_filter, tableinfo.tab11cell, tableinfo.tab11total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab11open1 = (real) CloseUpdate( 11, 1, tableinfo.tab11open1, tableinfo.tab11close1, tableinfo.tab11dim_filter, tableinfo.tab11cell, tableinfo.tab11total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab11cell = (index) TableCellIndex( 11, 1,
			disease_status );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab11total_cell = (index) tableinfo.tab11cell;
		}
		else {
			tableinfo.tab11total_cell = (index) TableTotalCellIndex( 11, 1,
			disease_status );
		}
		CloseUpdate( 11, 0, tableinfo.tab11open0, entrances_disease_status_INFECTED_event, table_filter_InfectionAge, tableinfo.tab11cell, tableinfo.tab11total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 11, 1, tableinfo.tab11open1, value_at_entrances_disease_status_INFECTED_age_event, table_filter_InfectionAge, tableinfo.tab11cell, tableinfo.tab11total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[12] >= 0 && ( tableinfo.tab12dim_filter || table_filter_InfectionAgeHist ) ) {
		if ( tableinfo.tab12events != -1 ) {
			tableinfo.tab12open0 = (real) CloseUpdate( 12, 0, tableinfo.tab12open0, tableinfo.tab12close0, tableinfo.tab12dim_filter, tableinfo.tab12cell, tableinfo.tab12total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab12open1 = (real) CloseUpdate( 12, 1, tableinfo.tab12open1, tableinfo.tab12close1, tableinfo.tab12dim_filter, tableinfo.tab12cell, tableinfo.tab12total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab12cell = (index) TableCellIndex( 12, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab12total_cell = (index) tableinfo.tab12cell;
		}
		else {
			tableinfo.tab12total_cell = (index) TableTotalCellIndex( 12, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		}
		CloseUpdate( 12, 0, tableinfo.tab12open0, poDerivedStates->entrances_alive_FALSE, table_filter_InfectionAgeHist, tableinfo.tab12cell, tableinfo.tab12total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 12, 1, tableinfo.tab12open1, poDerivedStates->entrances_disease_status_INFECTED, table_filter_InfectionAgeHist, tableinfo.tab12cell, tableinfo.tab12total_cell, actor_weight, actor_subsample_weight );
	}
	mlChild->Finish();
	if (poasAllMother != NULL)
	{	asAllMother->Remove(poasAllMother);
		poasAllMother = NULL;
	}

	// Remove actor from master list of active actors
	if ( poPrev != NULL )
	{
		poPrev->poNext = poNext;
	}
	else
	{
		if (gpoEventQueue->poHeadActor != NULL && gpoEventQueue->poHeadActor == this)
		{
			gpoEventQueue->poHeadActor = poNext;
		}
	}
	if ( poNext != NULL )
	{
		poNext->poPrev = poPrev;
	}
	poPrev = NULL;

	// Add actor to head of empty list of actors of this type
	poNext = gpoEmptyMother;
	gpoEmptyMother = this;
}

#pragma optimize( "", on )

void Mother::WaitUntil( double target_time )
{
	//don't increase gdEventsForTabulation if special events because they are all treated as one event for tabulation purposes
	if (!gpoEventQueue->m_bSpecialEvents)
	{
		gdEventsForTabulation++;
	}
	__events = gdEventsForTabulation;
	if ( time < (TIME) target_time )
	{
		__time = (TIME) target_time;
		WaitSpecial( (TIME) target_time - time );
		poStateFunctions->Set_time( (TIME) target_time );
	}
}

Mother *Mother::UpdateTime(  )
{
	__events = gdEventsForTabulation;
	if ( time < (TIME) gdThreadCurrentTime ) {
		__time = (TIME) gdThreadCurrentTime;
		WaitSpecial( (TIME) gdThreadCurrentTime - time );
		poStateFunctions->Set_time( (TIME) gdThreadCurrentTime );
	}
	return this;
}

void Mother::WaitUntilThisActor( double target_time )
{
	WaitUntil(target_time);
}

void Mother::WaitSpecial( TIME wait_time )
{

	// increment the age
		poStateFunctions->Set_age( age + wait_time );
}

TIME Mother::timeScheduleEvent()
{
	if (schedule_event_time == TIME_INFINITE)
	{
		TIME event_time = TIME_INFINITE;
		event_time = min( poDerivedStates->ssint_age_time, poDerivedStates->sssplit_age_INFECTION_AGES_time );
		schedule_event_time = event_time;
	}
	return schedule_event_time;
}

void Mother::ScheduleEvent()
{

	if (poDerivedStates->ssint_age_time == schedule_event_time)
	{
		CHKSUM(time, 16);
		TRCEVNT(case_seed, "Mother", actor_id, "scheduled - 0", time);
		gszRuntimeEventName = "Mother.scheduled - 0";
		poDerivedStates->Set_ssint_age( poDerivedStates->ssint_age + 1 );
		poDerivedStates->ssint_age_time = WAIT( 1 );
	}

	if (poDerivedStates->sssplit_age_INFECTION_AGES_time == schedule_event_time)
	{
		CHKSUM(time, 17);
		TRCEVNT(case_seed, "Mother", actor_id, "scheduled - 1", time);
		gszRuntimeEventName = "Mother.scheduled - 1";
		poDerivedStates->Set_sssplit_age_INFECTION_AGES( poDerivedStates->sssplit_age_INFECTION_AGES + 1 );
		if ( poDerivedStates->sssplit_age_INFECTION_AGES < 13 ) {
		poDerivedStates->sssplit_age_INFECTION_AGES_time = WAIT(TIME(POINTS(INFECTION_AGES)[poDerivedStates->sssplit_age_INFECTION_AGES] - POINTS(INFECTION_AGES)[poDerivedStates->sssplit_age_INFECTION_AGES - 1]));
		}
		else
		{		poDerivedStates->sssplit_age_INFECTION_AGES_time = TIME_INFINITE;
		}
	}
	schedule_event_time = TIME_INFINITE;
}

double Mother::EventTime( int nEventNum, int *pnEventInfo )
{
	double	event_time = double( TIME_INFINITE );

	gdRuntimeTime = (double) time;
	gnRuntimeActorId = actor_id;
	switch( nEventNum ) {
		case 0:
			gszRuntimeEventName = "Mother.BirthEvent(time)";
			event_time = (double) timeBirthEvent();
			TRCEVNT(case_seed, "Mother", actor_id, "timeBirthEvent", event_time);
			break;
		case 1:
			gszRuntimeEventName = "Mother.InfectionEvent(time)";
			event_time = (double) timeInfectionEvent();
			TRCEVNT(case_seed, "Mother", actor_id, "timeInfectionEvent", event_time);
			break;
		case 2:
			gszRuntimeEventName = "Mother.InfMortRNGEvent(time)";
			event_time = (double) timeInfMortRNGEvent();
			TRCEVNT(case_seed, "Mother", actor_id, "timeInfMortRNGEvent", event_time);
			break;
		case 3:
			gszRuntimeEventName = "Mother.MortalityEvent(time)";
			event_time = (double) timeMortalityEvent();
			TRCEVNT(case_seed, "Mother", actor_id, "timeMortalityEvent", event_time);
			break;
		case 4:
			gszRuntimeEventName = "ScheduleEvent(time)";
			event_time = (double) timeScheduleEvent();
			break;
		case -1:
		default:;
	}
	return TIME( CoarsenMantissa( (TIME) event_time ) );
}

void Mother::Implement( int nEventNum, int nEventInfo )
{
	gdThreadEvents += 1;
	gdRuntimeTime = (double) time;
	gnRuntimeActorId = actor_id;
	switch( nEventNum )
	{
		case 0:
			CHKSUM(time, 0);
			TRCEVNT(case_seed, "Mother", actor_id, "Mother.BirthEvent", time);
			gszRuntimeEventName = "Mother.BirthEvent(implement)";
			BirthEvent();
			break;
		case 1:
			CHKSUM(time, 1);
			TRCEVNT(case_seed, "Mother", actor_id, "Mother.InfectionEvent", time);
			gszRuntimeEventName = "Mother.InfectionEvent(implement)";
			InfectionEvent();
			break;
		case 2:
			CHKSUM(time, 2);
			TRCEVNT(case_seed, "Mother", actor_id, "Mother.InfMortRNGEvent", time);
			gszRuntimeEventName = "Mother.InfMortRNGEvent(implement)";
			InfMortRNGEvent();
			break;
		case 3:
			CHKSUM(time, 3);
			TRCEVNT(case_seed, "Mother", actor_id, "Mother.MortalityEvent", time);
			gszRuntimeEventName = "Mother.MortalityEvent(implement)";
			MortalityEvent();
			break;
		case 4:
			ScheduleEvent();
			break;
		case -1:
		default:;
	}
}

double Mother::Set_actor_subsample_weight( double value )
{
	return poStateFunctions->Set_actor_subsample_weight( value);
}

double Mother::Set_actor_weight( double value )
{
	return poStateFunctions->Set_actor_weight( value);
}

long MotherStateFunctions::Set_actor_id( long value )
{

	if ( poParent->actor_id != value )
	{


		long	OldValue;
		OldValue = poParent->actor_id;
		poParent->actor_id = value;

#line 83 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
		Set_tracking( poParent->actor_id% 50== 0 );
#line 1951 "ACTORS.CPP"
		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 30, poParent->actor_id );
		}
	}
	return poParent->actor_id = value;
}

TIME MotherStateFunctions::Set_age( TIME value, bool bStartFunction )
{

	if ( poParent->age != value || bStartFunction )
	{
		if (bStartFunction)
		{
			poDerivedStates->Set_ssint_age( int(value) );
			poDerivedStates->ssint_age_time = poParent->time + poDerivedStates->ssint_age + 1 - value;
			if (poParent->schedule_event_time > poDerivedStates->ssint_age_time)
			{
				poParent->schedule_event_time = poDerivedStates->ssint_age_time;
			}
		}
		if (bStartFunction)
		{
			poDerivedStates->Set_sssplit_age_INFECTION_AGES( SPLIT(value, INFECTION_AGES) );
			if ( poDerivedStates->sssplit_age_INFECTION_AGES < 13 )
			{
				poDerivedStates->sssplit_age_INFECTION_AGES_time = poParent->time +  POINTS(INFECTION_AGES)[poDerivedStates->sssplit_age_INFECTION_AGES] - value ;

			}
			else
			{
				poDerivedStates->sssplit_age_INFECTION_AGES_time = TIME_INFINITE;
			}
			if (poParent->schedule_event_time > poDerivedStates->sssplit_age_INFECTION_AGES_time)
			{
				poParent->schedule_event_time = poDerivedStates->sssplit_age_INFECTION_AGES_time;
			}
		}


		TIME	OldValue;
		OldValue = poParent->age;
		poParent->age = value;

		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 33, poParent->age );
		}
	}
	return poParent->age = value;
}

logical MotherStateFunctions::Set_alive( logical value )
{

	value = value != 0;
	if ( poParent->alive != value )
	{
		if ( value == FALSE ) {
			poDerivedStates->Set_entrances_alive_FALSE( poDerivedStates->entrances_alive_FALSE + 1 );
		}
#line 65 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MFertility.mpp"
		Set_table_filter_FertDetails( value== FALSE );
#line 2014 "ACTORS.CPP"
		if ( value == FALSE ) {
			poDerivedStates->Set_value_at_entrances_alive_FALSE_number_children( poDerivedStates->value_at_entrances_alive_FALSE_number_children + poParent->number_children );
		}
		if ( value == FALSE ) {
			poDerivedStates->Set_value_at_entrances_alive_FALSE_number_children_link( poDerivedStates->value_at_entrances_alive_FALSE_number_children_link + poParent->number_children_link );
		}


		logical	OldValue;
		OldValue = poParent->alive;
		poParent->alive = value;

#line 59 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MotherCore.mpp"
		Set_alive_value_in( poParent->alive );
#line 2029 "ACTORS.CPP"
		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 34, poParent->alive );
		}
	}
	return poParent->alive = value;
}

DISEASE_STATE MotherStateFunctions::Set_disease_status( DISEASE_STATE value )
{

	CHKLMT(value, disease_status, 0, rc_size_DISEASE_STATE - 1);
	if ( value < 0 ) {
		value = (DISEASE_STATE) 0;
	}
	else if ( value >= rc_size_DISEASE_STATE ) {
		value = (DISEASE_STATE) ( rc_size_DISEASE_STATE - 1 );
	}
	if ( poParent->disease_status != value )
	{
		if ( value == INFECTED ) {
			poDerivedStates->Set_entrances_disease_status_INFECTED( poDerivedStates->entrances_disease_status_INFECTED + 1 );
		}
		if ( value == INFECTED ) {
			poDerivedStates->Set_value_at_entrances_disease_status_INFECTED_age( poDerivedStates->value_at_entrances_disease_status_INFECTED_age + poParent->age );
		}


		DISEASE_STATE	OldValue;
		OldValue = poParent->disease_status;
		poParent->disease_status = value;

		{
			int	nIndex;
			int	nSize;
			Child	*prChild;

			nSize = poParent->mlChild->arData.GetSize();
			for ( nIndex = 0; nIndex < nSize; nIndex++ ) {
				prChild = poParent->mlChild->arData[nIndex];
				if ( prChild != NULL ) {
					prChild->UpdateTime();
					prChild->poStateFunctions->Set_lMother_disease_status( poParent->disease_status );
				}
			}
		}
		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 37, poParent->disease_status );
		}
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[11] >= 0 && ( poParent->tableinfo.tab11dim_filter || poParent->table_filter_InfectionAge ) ) {
				if ( poParent->tableinfo.tab11events == -1 || poParent->tableinfo.tab11events < poParent->__events ) {
					if ( poParent->tableinfo.tab11events != -1 ) {
						poParent->tableinfo.tab11open0 = (real) poParent->CloseUpdate( 11, 0, poParent->tableinfo.tab11open0, poParent->tableinfo.tab11close0, poParent->tableinfo.tab11dim_filter, poParent->tableinfo.tab11cell, poParent->tableinfo.tab11total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab11open1 = (real) poParent->CloseUpdate( 11, 1, poParent->tableinfo.tab11open1, poParent->tableinfo.tab11close1, poParent->tableinfo.tab11dim_filter, poParent->tableinfo.tab11cell, poParent->tableinfo.tab11total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab11events = poParent->__events;
					if ( poParent->tableinfo.tab11events0 == poParent->__events ) {
						poParent->tableinfo.tab11close0 = (real) poParent->tableinfo.tab11prev0;
					}
					else {
						poParent->tableinfo.tab11close0 = (real) poParent->entrances_disease_status_INFECTED_event;
					}
					if ( poParent->tableinfo.tab11events1 == poParent->__events ) {
						poParent->tableinfo.tab11close1 = (real) poParent->tableinfo.tab11prev1;
					}
					else {
						poParent->tableinfo.tab11close1 = (real) poParent->value_at_entrances_disease_status_INFECTED_age_event;
					}
					poParent->tableinfo.tab11cell = (index) poParent->TableCellIndex( 11, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab11total_cell = (index) poParent->tableinfo.tab11cell;
					}
					else {
						poParent->tableinfo.tab11total_cell = (index) poParent->TableTotalCellIndex( 11, 1,
						OldValue );
					}
					poParent->tableinfo.tab11dim_filter = poParent->table_filter_InfectionAge;
				}
			}
		}
		if ( poParent->poInfectionEvent->cRecalc == 0 ) {
			poParent->poInfectionEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poInfectionEvent );
		};
		if ( poParent->poMortalityEvent->cRecalc == 0 ) {
			poParent->poMortalityEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poMortalityEvent );
		};
	}
	return poParent->disease_status = value;
}

double MotherStateFunctions::Set_inf_mortality_draw( double value )
{

	if ( poParent->inf_mortality_draw != value )
	{

		double	OldValue;
		OldValue = poParent->inf_mortality_draw;
		poParent->inf_mortality_draw = value;
		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 38, poParent->inf_mortality_draw );
		}
		if ( poParent->poMortalityEvent->cRecalc == 0 ) {
			poParent->poMortalityEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poMortalityEvent );
		};
	}
	return poParent->inf_mortality_draw = value;
}

logical MotherStateFunctions::Set_inf_mortality_rng( logical value )
{

	value = value != 0;
	if ( poParent->inf_mortality_rng != value )
	{

		logical	OldValue;
		OldValue = poParent->inf_mortality_rng;
		poParent->inf_mortality_rng = value;
		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 39, poParent->inf_mortality_rng );
		}
		if ( poParent->poInfMortRNGEvent->cRecalc == 0 ) {
			poParent->poInfMortRNGEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poInfMortRNGEvent );
		};
	}
	return poParent->inf_mortality_rng = value;
}

LIFE MotherStateFunctions::Set_number_children( LIFE value )
{

	CHKLMT(value, number_children, r_min_LIFE, r_max_LIFE);
	if ( value < r_min_LIFE ) {
		value = r_min_LIFE;
	}
	else if ( value > r_max_LIFE ) {
		value = r_max_LIFE;
	}
	if ( poParent->number_children != value )
	{
		poDerivedStates->Set_changes_number_children( poDerivedStates->changes_number_children + 1 );


		LIFE	OldValue;
		OldValue = poParent->number_children;
		poParent->number_children = value;

		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 40, poParent->number_children );
		}
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[7] >= 0 && ( poParent->tableinfo.tab7dim_filter || poParent->table_filter_FertDetails ) ) {
				if ( poParent->tableinfo.tab7events == -1 || poParent->tableinfo.tab7events < poParent->__events ) {
					if ( poParent->tableinfo.tab7events != -1 ) {
						Set_table_unit_FertDetails(poParent->table_unit_FertDetails+1);
						poParent->tableinfo.tab7close0 = (real) poParent->table_unit_FertDetails;
						poParent->tableinfo.tab7open0 = (real) poParent->CloseUpdate( 7, 0, poParent->tableinfo.tab7open0, poParent->tableinfo.tab7close0, poParent->tableinfo.tab7dim_filter, poParent->tableinfo.tab7cell, poParent->tableinfo.tab7total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab7events = poParent->__events;
					poParent->tableinfo.tab7close0 = (real) poParent->table_unit_FertDetails;
					poParent->tableinfo.tab7cell = (index) poParent->TableCellIndex( 7, 1,
						OldValue - r_min_LIFE );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab7total_cell = (index) poParent->tableinfo.tab7cell;
					}
					else {
						poParent->tableinfo.tab7total_cell = (index) poParent->TableTotalCellIndex( 7, 1,
						OldValue - r_min_LIFE );
					}
					poParent->tableinfo.tab7dim_filter = poParent->table_filter_FertDetails;
				}
			}
		}
	}
	return poParent->number_children = value;
}

REPORT_TIME MotherStateFunctions::Set_report_time( REPORT_TIME value )
{

	CHKLMT(value, report_time, r_min_REPORT_TIME, r_max_REPORT_TIME);
	if ( value < r_min_REPORT_TIME ) {
		value = r_min_REPORT_TIME;
	}
	else if ( value > r_max_REPORT_TIME ) {
		value = r_max_REPORT_TIME;
	}
	if ( poParent->report_time != value )
	{

		REPORT_TIME	OldValue;
		OldValue = poParent->report_time;
		poParent->report_time = value;
		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 41, poParent->report_time );
		}
	}
	return poParent->report_time = value;
}

counter MotherStateFunctions::Set_table_unit_FertDetails( counter value )
{

	if ( poParent->table_unit_FertDetails != value )
	{

		counter	OldValue;
		OldValue = poParent->table_unit_FertDetails;
		poParent->table_unit_FertDetails = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[7] >= 0 ) {
				if ( poParent->tableinfo.tab7events == poParent->__events ) {
					poParent->tableinfo.tab7close0 = (real) poParent->table_unit_FertDetails;
				}
			}
		}
	}
	return poParent->table_unit_FertDetails = value;
}

logical MotherStateFunctions::Set_actorset_filter_asAllMother( logical value )
{

	value = value != 0;
	if ( poParent->actorset_filter_asAllMother != value )
	{

		logical	OldValue;
		OldValue = poParent->actorset_filter_asAllMother;
		poParent->actorset_filter_asAllMother = value;
		// actorset filter
		if (poParent->actorset_filter_asAllMother)
		{
			poParent->poasAllMother = asAllMother->NewNode(poParent);
		}
		else
		{
			asAllMother->Remove(poParent->poasAllMother);
			poParent->poasAllMother = NULL;
		}
	}
	return poParent->actorset_filter_asAllMother = value;
}

logical MotherStateFunctions::Set_alive_value_in( logical value )
{

	value = value != 0;
	if ( poParent->alive_value_in != value )
	{

		logical	OldValue;
		OldValue = poParent->alive_value_in;
		poParent->alive_value_in = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[10] >= 0 ) {
				if ( poParent->tableinfo.tab10events == poParent->__events ) {
					poParent->tableinfo.tab10close0 = (real) poParent->alive_value_in;
				}
			}
		}
	}
	return poParent->alive_value_in = value;
}

TIME MotherStateFunctions::Set_duration__max_value_out( TIME value )
{

	if ( poParent->duration__max_value_out != value )
	{

		TIME	OldValue;
		OldValue = poParent->duration__max_value_out;
		poParent->duration__max_value_out = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[10] >= 0 ) {
				if ( poParent->tableinfo.tab10events == poParent->__events ) {
					poParent->tableinfo.tab10close2 = (real) poParent->duration__max_value_out;
				}
			}
		}
	}
	return poParent->duration__max_value_out = value;
}

TIME	MotherStateFunctions::Get_duration__max_value_out(){
return poDerivedStates->Get_duration_();
};

TIME MotherStateFunctions::Set_duration__min_value_out( TIME value )
{

	if ( poParent->duration__min_value_out != value )
	{

		TIME	OldValue;
		OldValue = poParent->duration__min_value_out;
		poParent->duration__min_value_out = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[10] >= 0 ) {
				if ( poParent->tableinfo.tab10events == poParent->__events ) {
					poParent->tableinfo.tab10close3 = (real) poParent->duration__min_value_out;
				}
			}
		}
	}
	return poParent->duration__min_value_out = value;
}

TIME	MotherStateFunctions::Get_duration__min_value_out(){
return poDerivedStates->Get_duration_();
};

counter MotherStateFunctions::Set_entrances_disease_status_INFECTED_event( counter value )
{

	if ( poParent->entrances_disease_status_INFECTED_event != value )
	{

		counter	OldValue;
		OldValue = poParent->entrances_disease_status_INFECTED_event;
		poParent->entrances_disease_status_INFECTED_event = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[11] >= 0 ) {
				poParent->tableinfo.tab11prev0 = (real) OldValue;
				poParent->tableinfo.tab11events0 = poParent->__events;
			}
		}
	}
	return poParent->entrances_disease_status_INFECTED_event = value;
}

LIFE MotherStateFunctions::Set_integer_age( LIFE value )
{

	CHKLMT(value, integer_age, r_min_LIFE, r_max_LIFE);
	if ( value < r_min_LIFE ) {
		value = r_min_LIFE;
	}
	else if ( value > r_max_LIFE ) {
		value = r_max_LIFE;
	}
	if ( poParent->integer_age != value )
	{

		LIFE	OldValue;
		OldValue = poParent->integer_age;
		poParent->integer_age = value;
		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 54, poParent->integer_age );
		}
		if ( poParent->poBirthEvent->cRecalc == 0 ) {
			poParent->poBirthEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poBirthEvent );
		};
		if ( poParent->poInfectionEvent->cRecalc == 0 ) {
			poParent->poInfectionEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poInfectionEvent );
		};
	}
	return poParent->integer_age = value;
}

LIFE MotherStateFunctions::Set_number_children_link( LIFE value )
{

	CHKLMT(value, number_children_link, r_min_LIFE, r_max_LIFE);
	if ( value < r_min_LIFE ) {
		value = r_min_LIFE;
	}
	else if ( value > r_max_LIFE ) {
		value = r_max_LIFE;
	}
	if ( poParent->number_children_link != value )
	{

		LIFE	OldValue;
		OldValue = poParent->number_children_link;
		poParent->number_children_link = value;
		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 55, poParent->number_children_link );
		}
	}
	return poParent->number_children_link = value;
}

logical MotherStateFunctions::Set_table_filter_ageAtBirth( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_ageAtBirth != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_ageAtBirth;
		poParent->table_filter_ageAtBirth = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[6] >= 0 ) {
				if ( poParent->tableinfo.tab6events == -1 || poParent->tableinfo.tab6events < poParent->__events ) {
					if ( poParent->tableinfo.tab6events != -1 ) {
						poParent->tableinfo.tab6open0 = (real) poParent->CloseUpdate( 6, 0, poParent->tableinfo.tab6open0, poParent->tableinfo.tab6close0, poParent->tableinfo.tab6dim_filter, poParent->tableinfo.tab6cell, poParent->tableinfo.tab6total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab6events = poParent->__events;
					poParent->tableinfo.tab6close0 = (real) poDerivedStates->changes_number_children;
					poParent->tableinfo.tab6cell = (index) poParent->TableCellIndex( 6, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab6total_cell = (index) poParent->tableinfo.tab6cell;
					}
					else {
						poParent->tableinfo.tab6total_cell = (index) poParent->TableTotalCellIndex( 6, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					}
					poParent->tableinfo.tab6dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_ageAtBirth = value;
}

logical MotherStateFunctions::Set_table_filter_DurationOfLife( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_DurationOfLife != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_DurationOfLife;
		poParent->table_filter_DurationOfLife = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[10] >= 0 ) {
				if ( poParent->tableinfo.tab10events == -1 || poParent->tableinfo.tab10events < poParent->__events ) {
					if ( poParent->tableinfo.tab10events != -1 ) {
						poParent->tableinfo.tab10open0 = (real) poParent->CloseUpdate( 10, 0, poParent->tableinfo.tab10open0, poParent->tableinfo.tab10close0, poParent->tableinfo.tab10dim_filter, poParent->tableinfo.tab10cell, poParent->tableinfo.tab10total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab10open1 = (real) poParent->CloseUpdate( 10, 1, poParent->tableinfo.tab10open1, poParent->tableinfo.tab10close1, poParent->tableinfo.tab10dim_filter, poParent->tableinfo.tab10cell, poParent->tableinfo.tab10total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab10open2 = (real) poParent->CloseUpdate( 10, 2, poParent->tableinfo.tab10open2, poParent->tableinfo.tab10close2, poParent->tableinfo.tab10dim_filter, poParent->tableinfo.tab10cell, poParent->tableinfo.tab10total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab10open3 = (real) poParent->CloseUpdate( 10, 3, poParent->tableinfo.tab10open3, poParent->tableinfo.tab10close3, poParent->tableinfo.tab10dim_filter, poParent->tableinfo.tab10cell, poParent->tableinfo.tab10total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab10events = poParent->__events;
					poParent->tableinfo.tab10close0 = (real) poParent->alive_value_in;
					poParent->tableinfo.tab10close1 = (real) poDerivedStates->Get_duration_();
					poParent->tableinfo.tab10close2 = (real) Get_duration__max_value_out();
					poParent->tableinfo.tab10close3 = (real) Get_duration__min_value_out();
					poParent->tableinfo.tab10cell = (index) poParent->TableCellIndex( 10, 0 );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab10total_cell = (index) poParent->tableinfo.tab10cell;
					}
					else {
						poParent->tableinfo.tab10total_cell = (index) poParent->TableTotalCellIndex( 10, 0 );
					}
					poParent->tableinfo.tab10dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_DurationOfLife = value;
}

logical MotherStateFunctions::Set_table_filter_FertDetails( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_FertDetails != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_FertDetails;
		poParent->table_filter_FertDetails = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[7] >= 0 ) {
				if ( poParent->tableinfo.tab7events == -1 || poParent->tableinfo.tab7events < poParent->__events ) {
					if ( poParent->tableinfo.tab7events != -1 ) {
						Set_table_unit_FertDetails(poParent->table_unit_FertDetails+1);
						poParent->tableinfo.tab7close0 = (real) poParent->table_unit_FertDetails;
						poParent->tableinfo.tab7open0 = (real) poParent->CloseUpdate( 7, 0, poParent->tableinfo.tab7open0, poParent->tableinfo.tab7close0, poParent->tableinfo.tab7dim_filter, poParent->tableinfo.tab7cell, poParent->tableinfo.tab7total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab7events = poParent->__events;
					poParent->tableinfo.tab7close0 = (real) poParent->table_unit_FertDetails;
					poParent->tableinfo.tab7cell = (index) poParent->TableCellIndex( 7, 1,
						poParent->number_children - r_min_LIFE );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab7total_cell = (index) poParent->tableinfo.tab7cell;
					}
					else {
						poParent->tableinfo.tab7total_cell = (index) poParent->TableTotalCellIndex( 7, 1,
						poParent->number_children - r_min_LIFE );
					}
					poParent->tableinfo.tab7dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_FertDetails = value;
}

logical MotherStateFunctions::Set_table_filter_FertDetailsLink( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_FertDetailsLink != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_FertDetailsLink;
		poParent->table_filter_FertDetailsLink = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[8] >= 0 ) {
				if ( poParent->tableinfo.tab8events == -1 || poParent->tableinfo.tab8events < poParent->__events ) {
					if ( poParent->tableinfo.tab8events != -1 ) {
						poParent->tableinfo.tab8open0 = (real) poParent->CloseUpdate( 8, 0, poParent->tableinfo.tab8open0, poParent->tableinfo.tab8close0, poParent->tableinfo.tab8dim_filter, poParent->tableinfo.tab8cell, poParent->tableinfo.tab8total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab8events = poParent->__events;
					poParent->tableinfo.tab8close0 = (real) poDerivedStates->value_at_entrances_alive_FALSE_number_children_link;
					poParent->tableinfo.tab8cell = (index) poParent->TableCellIndex( 8, 0 );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab8total_cell = (index) poParent->tableinfo.tab8cell;
					}
					else {
						poParent->tableinfo.tab8total_cell = (index) poParent->TableTotalCellIndex( 8, 0 );
					}
					poParent->tableinfo.tab8dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_FertDetailsLink = value;
}

logical MotherStateFunctions::Set_table_filter_FertDetailsSum( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_FertDetailsSum != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_FertDetailsSum;
		poParent->table_filter_FertDetailsSum = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[9] >= 0 ) {
				if ( poParent->tableinfo.tab9events == -1 || poParent->tableinfo.tab9events < poParent->__events ) {
					if ( poParent->tableinfo.tab9events != -1 ) {
						poParent->tableinfo.tab9open0 = (real) poParent->CloseUpdate( 9, 0, poParent->tableinfo.tab9open0, poParent->tableinfo.tab9close0, poParent->tableinfo.tab9dim_filter, poParent->tableinfo.tab9cell, poParent->tableinfo.tab9total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab9events = poParent->__events;
					poParent->tableinfo.tab9close0 = (real) poDerivedStates->value_at_entrances_alive_FALSE_number_children;
					poParent->tableinfo.tab9cell = (index) poParent->TableCellIndex( 9, 0 );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab9total_cell = (index) poParent->tableinfo.tab9cell;
					}
					else {
						poParent->tableinfo.tab9total_cell = (index) poParent->TableTotalCellIndex( 9, 0 );
					}
					poParent->tableinfo.tab9dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_FertDetailsSum = value;
}

logical MotherStateFunctions::Set_table_filter_InfectionAge( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_InfectionAge != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_InfectionAge;
		poParent->table_filter_InfectionAge = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[11] >= 0 ) {
				if ( poParent->tableinfo.tab11events == -1 || poParent->tableinfo.tab11events < poParent->__events ) {
					if ( poParent->tableinfo.tab11events != -1 ) {
						poParent->tableinfo.tab11open0 = (real) poParent->CloseUpdate( 11, 0, poParent->tableinfo.tab11open0, poParent->tableinfo.tab11close0, poParent->tableinfo.tab11dim_filter, poParent->tableinfo.tab11cell, poParent->tableinfo.tab11total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab11open1 = (real) poParent->CloseUpdate( 11, 1, poParent->tableinfo.tab11open1, poParent->tableinfo.tab11close1, poParent->tableinfo.tab11dim_filter, poParent->tableinfo.tab11cell, poParent->tableinfo.tab11total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab11events = poParent->__events;
					if ( poParent->tableinfo.tab11events0 == poParent->__events ) {
						poParent->tableinfo.tab11close0 = (real) poParent->tableinfo.tab11prev0;
					}
					else {
						poParent->tableinfo.tab11close0 = (real) poParent->entrances_disease_status_INFECTED_event;
					}
					if ( poParent->tableinfo.tab11events1 == poParent->__events ) {
						poParent->tableinfo.tab11close1 = (real) poParent->tableinfo.tab11prev1;
					}
					else {
						poParent->tableinfo.tab11close1 = (real) poParent->value_at_entrances_disease_status_INFECTED_age_event;
					}
					poParent->tableinfo.tab11cell = (index) poParent->TableCellIndex( 11, 1,
						poParent->disease_status );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab11total_cell = (index) poParent->tableinfo.tab11cell;
					}
					else {
						poParent->tableinfo.tab11total_cell = (index) poParent->TableTotalCellIndex( 11, 1,
						poParent->disease_status );
					}
					poParent->tableinfo.tab11dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_InfectionAge = value;
}

logical MotherStateFunctions::Set_table_filter_InfectionAgeHist( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_InfectionAgeHist != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_InfectionAgeHist;
		poParent->table_filter_InfectionAgeHist = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[12] >= 0 ) {
				if ( poParent->tableinfo.tab12events == -1 || poParent->tableinfo.tab12events < poParent->__events ) {
					if ( poParent->tableinfo.tab12events != -1 ) {
						poParent->tableinfo.tab12open0 = (real) poParent->CloseUpdate( 12, 0, poParent->tableinfo.tab12open0, poParent->tableinfo.tab12close0, poParent->tableinfo.tab12dim_filter, poParent->tableinfo.tab12cell, poParent->tableinfo.tab12total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab12open1 = (real) poParent->CloseUpdate( 12, 1, poParent->tableinfo.tab12open1, poParent->tableinfo.tab12close1, poParent->tableinfo.tab12dim_filter, poParent->tableinfo.tab12cell, poParent->tableinfo.tab12total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab12events = poParent->__events;
					poParent->tableinfo.tab12close0 = (real) poDerivedStates->entrances_alive_FALSE;
					poParent->tableinfo.tab12close1 = (real) poDerivedStates->entrances_disease_status_INFECTED;
					poParent->tableinfo.tab12cell = (index) poParent->TableCellIndex( 12, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab12total_cell = (index) poParent->tableinfo.tab12cell;
					}
					else {
						poParent->tableinfo.tab12total_cell = (index) poParent->TableTotalCellIndex( 12, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					}
					poParent->tableinfo.tab12dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_InfectionAgeHist = value;
}

logical MotherStateFunctions::Set_tracking( logical value )
{

	value = value != 0;
	if ( poParent->tracking != value )
	{

		logical	OldValue;
		OldValue = poParent->tracking;
		poParent->tracking = value;
		if ( poParent->bUpdates && poParent->tracking ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 65, poParent->tracking );
		}
		if ( poParent->bUpdates && poParent->tracking ) {
			// output initial values of tracked states and links
			TrackChange( poParent->actor_id, (double) poParent->__time, 35, poParent->case_id );
			TrackChange( poParent->actor_id, (double) poParent->__time, 65, 1.0 );
			TrackChange( poParent->actor_id, (double) poParent->__time, 30, poParent->actor_id );
			TrackChange( poParent->actor_id, (double) poParent->__time, 31, poParent->actor_subsample_weight );
			TrackChange( poParent->actor_id, (double) poParent->__time, 32, poParent->actor_weight );
			TrackChange( poParent->actor_id, (double) poParent->__time, 33, poParent->age );
			TrackChange( poParent->actor_id, (double) poParent->__time, 34, poParent->alive );
			TrackChange( poParent->actor_id, (double) poParent->__time, 36, poParent->case_seed );
			TrackChange( poParent->actor_id, (double) poParent->__time, 37, poParent->disease_status );
			TrackChange( poParent->actor_id, (double) poParent->__time, 38, poParent->inf_mortality_draw );
			TrackChange( poParent->actor_id, (double) poParent->__time, 39, poParent->inf_mortality_rng );
			TrackChange( poParent->actor_id, (double) poParent->__time, 40, poParent->number_children );
			TrackChange( poParent->actor_id, (double) poParent->__time, 41, poParent->report_time );
			TrackChange( poParent->actor_id, (double) poParent->__time, 43, poParent->time );
			TrackChange( poParent->actor_id, (double) poParent->__time, 53, poParent->final_tracking );
			TrackChange( poParent->actor_id, (double) poParent->__time, 54, poParent->integer_age );
			TrackChange( poParent->actor_id, (double) poParent->__time, 55, poParent->number_children_link );
			TrackChange( poParent->actor_id, (double) poParent->__time, 65, poParent->tracking );
		}
		else if ( poParent->bUpdates ) {
			TrackChange( poParent->actor_id, (double) poParent->__time, 65, 0.0 );
		}
	}
	return poParent->tracking = value;
}

real MotherStateFunctions::Set_value_at_entrances_disease_status_INFECTED_age_event( real value )
{

	if ( poParent->value_at_entrances_disease_status_INFECTED_age_event != value )
	{

		real	OldValue;
		OldValue = poParent->value_at_entrances_disease_status_INFECTED_age_event;
		poParent->value_at_entrances_disease_status_INFECTED_age_event = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[11] >= 0 ) {
				poParent->tableinfo.tab11prev1 = (real) OldValue;
				poParent->tableinfo.tab11events1 = poParent->__events;
			}
		}
	}
	return poParent->value_at_entrances_disease_status_INFECTED_age_event = value;
}

counter MotherStateFunctions::Set_count_mlChild( counter value )
{

	if ( poParent->count_mlChild != value )
	{


		counter	OldValue;
		OldValue = poParent->count_mlChild;
		poParent->count_mlChild = value;

#line 24 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/MFertility.mpp"
		Set_number_children_link( poParent->count_mlChild );
#line 2756 "ACTORS.CPP"
	}
	return poParent->count_mlChild = value;
}

counter MotherDerivedStates::Set_changes_number_children( counter value )
{

	if ( changes_number_children != value ) {

counter		OldValue;
		OldValue = changes_number_children;
		changes_number_children = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[6] >= 0 ) {
				if ( poParent->tableinfo.tab6events == poParent->__events ) {
					poParent->tableinfo.tab6close0 = (real) changes_number_children;
				}
			}
		}
	}
	return changes_number_children = value;
}

TIME MotherDerivedStates::Set_duration_( TIME value )
{

	if ( duration_ != value ) {

		TIME OldValue;
		OldValue = duration_;
		duration_ = value;
		poStateFunctions->Set_duration__max_value_out( duration_ );
		poStateFunctions->Set_duration__min_value_out( duration_ );
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[10] >= 0 ) {
				if ( poParent->tableinfo.tab10events == poParent->__events ) {
					poParent->tableinfo.tab10close1 = (real) duration_;
				}
			}
		}
	}
	return duration_ = value;
}

counter MotherDerivedStates::Set_entrances_alive_FALSE( counter value )
{

	if ( entrances_alive_FALSE != value ) {

counter		OldValue;
		OldValue = entrances_alive_FALSE;
		entrances_alive_FALSE = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[12] >= 0 ) {
				if ( poParent->tableinfo.tab12events == poParent->__events ) {
					poParent->tableinfo.tab12close0 = (real) entrances_alive_FALSE;
				}
			}
		}
	}
	return entrances_alive_FALSE = value;
}

counter MotherDerivedStates::Set_entrances_disease_status_INFECTED( counter value )
{

	if ( entrances_disease_status_INFECTED != value ) {

		counter OldValue;
		OldValue = entrances_disease_status_INFECTED;
		entrances_disease_status_INFECTED = value;
		poStateFunctions->Set_entrances_disease_status_INFECTED_event( entrances_disease_status_INFECTED );
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[12] >= 0 ) {
				if ( poParent->tableinfo.tab12events == poParent->__events ) {
					poParent->tableinfo.tab12close1 = (real) entrances_disease_status_INFECTED;
				}
			}
		}
	}
	return entrances_disease_status_INFECTED = value;
}

counter MotherDerivedStates::Set_ssint_age( counter value )
{

	if ( ssint_age != value ) {

		counter OldValue;
		OldValue = ssint_age;
		ssint_age = value;
		poStateFunctions->Set_integer_age( ssint_age );
	}
	return ssint_age = value;
}

INFECTION_AGES MotherDerivedStates::Set_sssplit_age_INFECTION_AGES( INFECTION_AGES value )
{

	if ( sssplit_age_INFECTION_AGES != value ) {

INFECTION_AGES		OldValue;
		OldValue = sssplit_age_INFECTION_AGES;
		sssplit_age_INFECTION_AGES = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[6] >= 0 && ( poParent->tableinfo.tab6dim_filter || poParent->table_filter_ageAtBirth ) ) {
				if ( poParent->tableinfo.tab6events == -1 || poParent->tableinfo.tab6events < poParent->__events ) {
					if ( poParent->tableinfo.tab6events != -1 ) {
						poParent->tableinfo.tab6open0 = (real) poParent->CloseUpdate( 6, 0, poParent->tableinfo.tab6open0, poParent->tableinfo.tab6close0, poParent->tableinfo.tab6dim_filter, poParent->tableinfo.tab6cell, poParent->tableinfo.tab6total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab6events = poParent->__events;
					poParent->tableinfo.tab6close0 = (real) changes_number_children;
					poParent->tableinfo.tab6cell = (index) poParent->TableCellIndex( 6, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab6total_cell = (index) poParent->tableinfo.tab6cell;
					}
					else {
						poParent->tableinfo.tab6total_cell = (index) poParent->TableTotalCellIndex( 6, 1,
						OldValue );
					}
					poParent->tableinfo.tab6dim_filter = poParent->table_filter_ageAtBirth;
				}
			}
			if ( gplTableIndexes[12] >= 0 && ( poParent->tableinfo.tab12dim_filter || poParent->table_filter_InfectionAgeHist ) ) {
				if ( poParent->tableinfo.tab12events == -1 || poParent->tableinfo.tab12events < poParent->__events ) {
					if ( poParent->tableinfo.tab12events != -1 ) {
						poParent->tableinfo.tab12open0 = (real) poParent->CloseUpdate( 12, 0, poParent->tableinfo.tab12open0, poParent->tableinfo.tab12close0, poParent->tableinfo.tab12dim_filter, poParent->tableinfo.tab12cell, poParent->tableinfo.tab12total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab12open1 = (real) poParent->CloseUpdate( 12, 1, poParent->tableinfo.tab12open1, poParent->tableinfo.tab12close1, poParent->tableinfo.tab12dim_filter, poParent->tableinfo.tab12cell, poParent->tableinfo.tab12total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab12events = poParent->__events;
					poParent->tableinfo.tab12close0 = (real) entrances_alive_FALSE;
					poParent->tableinfo.tab12close1 = (real) entrances_disease_status_INFECTED;
					poParent->tableinfo.tab12cell = (index) poParent->TableCellIndex( 12, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab12total_cell = (index) poParent->tableinfo.tab12cell;
					}
					else {
						poParent->tableinfo.tab12total_cell = (index) poParent->TableTotalCellIndex( 12, 1,
						OldValue );
					}
					poParent->tableinfo.tab12dim_filter = poParent->table_filter_InfectionAgeHist;
				}
			}
		}
	}
	return sssplit_age_INFECTION_AGES = value;
}

integer MotherDerivedStates::Set_value_at_entrances_alive_FALSE_number_children( integer value )
{

	if ( value_at_entrances_alive_FALSE_number_children != value ) {

integer		OldValue;
		OldValue = value_at_entrances_alive_FALSE_number_children;
		value_at_entrances_alive_FALSE_number_children = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[9] >= 0 ) {
				if ( poParent->tableinfo.tab9events == poParent->__events ) {
					poParent->tableinfo.tab9close0 = (real) value_at_entrances_alive_FALSE_number_children;
				}
			}
		}
	}
	return value_at_entrances_alive_FALSE_number_children = value;
}

integer MotherDerivedStates::Set_value_at_entrances_alive_FALSE_number_children_link( integer value )
{

	if ( value_at_entrances_alive_FALSE_number_children_link != value ) {

integer		OldValue;
		OldValue = value_at_entrances_alive_FALSE_number_children_link;
		value_at_entrances_alive_FALSE_number_children_link = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[8] >= 0 ) {
				if ( poParent->tableinfo.tab8events == poParent->__events ) {
					poParent->tableinfo.tab8close0 = (real) value_at_entrances_alive_FALSE_number_children_link;
				}
			}
		}
	}
	return value_at_entrances_alive_FALSE_number_children_link = value;
}

real MotherDerivedStates::Set_value_at_entrances_disease_status_INFECTED_age( real value )
{

	if ( value_at_entrances_disease_status_INFECTED_age != value ) {

		real OldValue;
		OldValue = value_at_entrances_disease_status_INFECTED_age;
		value_at_entrances_disease_status_INFECTED_age = value;
		poStateFunctions->Set_value_at_entrances_disease_status_INFECTED_age_event( value_at_entrances_disease_status_INFECTED_age );
	}
	return value_at_entrances_disease_status_INFECTED_age = value;
}

void *Ticker::operator new( size_t count )
{
	Ticker *poActor;
	if ( gpoEmptyTicker != NULL ) {
		poActor = gpoEmptyTicker;
		gpoEmptyTicker = (Ticker *) gpoEmptyTicker->poNext;
	}
	else {
		poActor = ::new Ticker( TRUE ) ;
	}
	poActor->__finished = false;
	return poActor;
}

void Ticker::InitActor()
{
	poDerivedStates = new TickerDerivedStates();
	poDerivedStates->poParent = this;
	poStateFunctions = new TickerStateFunctions();
	poStateFunctions->poParent = this;
	poStateFunctions->poDerivedStates = poDerivedStates;
	poDerivedStates->poStateFunctions = poStateFunctions;
	bUpdates = FALSE;
	nActorNumber = 2;
	InitializeStates();
}

void Ticker::DeleteActor()
{
	delete poDerivedStates;
	delete poStateFunctions;
}

#pragma optimize( "", off )

void Ticker::InitializeStates()
{
	// initialization of states
	actor_id = (long) 0;
	actor_subsample_weight = (double) 0;
	actor_weight = (double) 0;
	age = (TIME) 0;
	case_id = (long) 0;
	case_seed = (double) 0;
	next_tick = (TIME) 0;
#line 18 "C:/Users/Administrator/Documents/Visual Studio 2010/Projects/MochModel3/TickerCore.mpp"
	CHKLMT(0, report_time, r_min_REPORT_TIME, r_max_REPORT_TIME);
	report_time = 0 < r_min_REPORT_TIME ? r_min_REPORT_TIME : 0 > r_max_REPORT_TIME ? r_max_REPORT_TIME : (REPORT_TIME) 0;
#line 3006 "ACTORS.CPP"
	time = (TIME) gdThreadCurrentTime;
	__finished = FALSE;
	__time = (TIME) gdThreadCurrentTime;
	__events = 0;
}

#pragma optimize( "", on )

void Ticker::UpdateLinkedStates()
{
}

void Ticker::StartSpecial1()
{
	nSubSample = GetSubSample();
	InitializeStates();
	case_seed = GetCaseSeed();
	case_id = GetCaseID();
	lActorId = actor_id = GetObjectID();
	GetCaseWeight( &actor_weight, &actor_subsample_weight );

	// initialization of events
	poTickEvent = gpoEventQueue->NewEvent( this, 0, 0 );
	//Initalize derived states
	CountActor(2);
}

#pragma optimize( "", off )

void Ticker::StartSpecial2()
{
	// initialization of expression and history states
	poStateFunctions->Set_time(TIME( CoarsenMantissa( time ) ), true);
	poStateFunctions->Set_age(TIME( CoarsenMantissa( age ) ), true);
	gdThreadCurrentTime = time;
	__time = time;
	poPrev = NULL;
	poNext = gpoEventQueue->poHeadActor;
	if ( gpoEventQueue->poHeadActor != NULL ) {
		gpoEventQueue->poHeadActor->poPrev = this;
	}
	gpoEventQueue->poHeadActor = this;
	bUpdates = TRUE;
}

void Ticker::FinishSpecial()
{
	CountFinishedActor(2);
	bUpdates = FALSE;
	gpoEventQueue->DestroyEvent( poTickEvent );

	// Remove actor from master list of active actors
	if ( poPrev != NULL )
	{
		poPrev->poNext = poNext;
	}
	else
	{
		if (gpoEventQueue->poHeadActor != NULL && gpoEventQueue->poHeadActor == this)
		{
			gpoEventQueue->poHeadActor = poNext;
		}
	}
	if ( poNext != NULL )
	{
		poNext->poPrev = poPrev;
	}
	poPrev = NULL;

	// Add actor to head of empty list of actors of this type
	poNext = gpoEmptyTicker;
	gpoEmptyTicker = this;
}

#pragma optimize( "", on )

void Ticker::WaitUntil( double target_time )
{
	//don't increase gdEventsForTabulation if special events because they are all treated as one event for tabulation purposes
	if (!gpoEventQueue->m_bSpecialEvents)
	{
		gdEventsForTabulation++;
	}
	__events = gdEventsForTabulation;
	if ( time < (TIME) target_time )
	{
		__time = (TIME) target_time;
		WaitSpecial( (TIME) target_time - time );
		poStateFunctions->Set_time( (TIME) target_time );
	}
}

Ticker *Ticker::UpdateTime(  )
{
	__events = gdEventsForTabulation;
	if ( time < (TIME) gdThreadCurrentTime ) {
		__time = (TIME) gdThreadCurrentTime;
		WaitSpecial( (TIME) gdThreadCurrentTime - time );
		poStateFunctions->Set_time( (TIME) gdThreadCurrentTime );
	}
	return this;
}

void Ticker::WaitUntilThisActor( double target_time )
{
	WaitUntil(target_time);
}

void Ticker::WaitSpecial( TIME wait_time )
{

	// increment the age
		poStateFunctions->Set_age( age + wait_time );
}

double Ticker::EventTime( int nEventNum, int *pnEventInfo )
{
	double	event_time = double( TIME_INFINITE );

	gdRuntimeTime = (double) time;
	gnRuntimeActorId = actor_id;
	switch( nEventNum ) {
		case 0:
			gszRuntimeEventName = "Ticker.TickEvent(time)";
			event_time = (double) timeTickEvent();
			TRCEVNT(case_seed, "Ticker", actor_id, "timeTickEvent", event_time);
			break;
		case -1:
		default:;
	}
	return TIME( CoarsenMantissa( (TIME) event_time ) );
}

void Ticker::Implement( int nEventNum, int nEventInfo )
{
	gdThreadEvents += 1;
	gdRuntimeTime = (double) time;
	gnRuntimeActorId = actor_id;
	switch( nEventNum )
	{
		case 0:
			CHKSUM(time, 0);
			TRCEVNT(case_seed, "Ticker", actor_id, "Ticker.TickEvent", time);
			gszRuntimeEventName = "Ticker.TickEvent(implement)";
			TickEvent();
			break;
		case -1:
		default:;
	}
}

double Ticker::Set_actor_subsample_weight( double value )
{
	return poStateFunctions->Set_actor_subsample_weight( value);
}

double Ticker::Set_actor_weight( double value )
{
	return poStateFunctions->Set_actor_weight( value);
}

TIME TickerStateFunctions::Set_next_tick( TIME value )
{

	if ( poParent->next_tick != value )
	{

		TIME	OldValue;
		OldValue = poParent->next_tick;
		poParent->next_tick = value;
		if ( poParent->poTickEvent->cRecalc == 0 ) {
			poParent->poTickEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poTickEvent );
		};
	}
	return poParent->next_tick = value;
}

REPORT_TIME TickerStateFunctions::Set_report_time( REPORT_TIME value )
{

	CHKLMT(value, report_time, r_min_REPORT_TIME, r_max_REPORT_TIME);
	if ( value < r_min_REPORT_TIME ) {
		value = r_min_REPORT_TIME;
	}
	else if ( value > r_max_REPORT_TIME ) {
		value = r_max_REPORT_TIME;
	}
	if ( poParent->report_time != value )
	{

		REPORT_TIME	OldValue;
		OldValue = poParent->report_time;
		poParent->report_time = value;
	}
	return poParent->report_time = value;
}
Thread Child	*gpoEmptyChild = NULL;

void DeleteAllChildActors()
{
}
Thread Mother	*gpoEmptyMother = NULL;

void DeleteAllMotherActors()
{
}
Thread Ticker	*gpoEmptyTicker = NULL;

void DeleteAllTickerActors()
{
}

void Mother_mlChild::Add( Child *prChild )
{
	BOOL	bFound = FALSE;
	int	nIndex;
	int	nNewPosition = -1;
	int	nSize;

	nSize = arData.GetSize();
	for ( nIndex = 0; nIndex < nSize; nIndex++ ) {
		if ( arData[nIndex] == prChild ) {
			bFound = TRUE;
			break;
		}
		if ( arData[nIndex] == NULL && nNewPosition == -1 ) {
			nNewPosition = nIndex;
		}
	}
	if ( !bFound ) {
		prChild->UpdateTime();
		if ( nNewPosition != -1 ) {
			arData[nNewPosition] = prChild;
		}
		else {
			arData.Add( prChild );
		}
		UpdateReferences();
		prChild->Set_lMother( prParentMother );
	}
}

void Mother_mlChild::Remove( Child *prChild )
{
	int	nIndex;
	int	nSize;

	nSize = arData.GetSize();
	for ( nIndex = 0; nIndex < nSize; nIndex++ ) {
		if ( arData[nIndex] == prChild ) {
			prChild->UpdateTime();
			arData[nIndex] = NULL;
			UpdateReferences();
			if ( prChild->lMother == prParentMother ) {
				prChild->Set_lMother( NULL );
			}
			break;
		}
	}
}

void Mother_mlChild::RemoveAll()
{
	int	nIndex;
	int	nSize;
	Child	*prChild;

	nSize = arData.GetSize();
	for ( nIndex = 0; nIndex < nSize; nIndex++ ) {
		prChild = arData[nIndex];
		if ( prChild != NULL ) {
			prChild->UpdateTime();
			arData[nIndex] = NULL;
			if ( prChild->lMother == prParentMother ) {
				prChild->Set_lMother( NULL );
			}
		}
	}
	UpdateReferences();
}

void Mother_mlChild::Finish()
{
	int	nSize = 0;
	nSize = arData.GetSize();
	RemoveAll();
	SetMaxActorLinked(1, 0, nSize);
}

void Mother_mlChild::FinishAll()
{
	int	nIndex;
	int	nSize;
	Child	*prChild;

	nSize = arData.GetSize();
	for ( nIndex = 0; nIndex < nSize; nIndex++ ) {
		prChild = arData[nIndex];
		if ( prChild != NULL ) {
			prChild->UpdateTime();
			prChild->Finish();
			arData[nIndex] = NULL;
		}
	}
	UpdateReferences();
}

Child *Mother_mlChild::GetNext( int nInitPos, int *pnPos )
{
	int	nIndex;
	int	nSize;
	Child	*prChild;
	Child	*prFoundChild = NULL;

	*pnPos = -1;
	nSize = arData.GetSize();
	for ( nIndex = nInitPos; nIndex < nSize; nIndex++ ) {
		prChild = arData[nIndex];
		if ( prChild != NULL ) {
			prFoundChild = prChild;
			*pnPos = nIndex;
			break;
		}
	}
	return prFoundChild;
}

counter Mother_mlChild::Count()
{
	int	nIndex;
	int	nResult = 0;
	int	nSize;

	nSize = arData.GetSize();
	for ( nIndex = 0; nIndex < nSize; nIndex++ ) {
		nResult += arData[nIndex] != NULL;
	}
	return nResult;
}

void Mother_mlChild::UpdateReferences()
{
	prParentMother->poStateFunctions->Set_count_mlChild( Count() );
}

int	asAllChildActorSetTree::ActorCompare(ActorClass *poActor1, ActorClass *poActor2)
	{
	Child	*poChild1 = NULL;
	Child	*poChild2 = NULL;

	poChild1 = (Child *) poActor1;
	poChild2 = (Child *) poActor2;
	return (poChild1->actor_id == poChild2->actor_id ? 0 :
			poChild1->actor_id < poChild2->actor_id ? -1 : 1);
}
Thread asAllChildActorSetTree	*asAllChild;

int	asAllMotherActorSetTree::ActorCompare(ActorClass *poActor1, ActorClass *poActor2)
	{
	Mother	*poMother1 = NULL;
	Mother	*poMother2 = NULL;

	poMother1 = (Mother *) poActor1;
	poMother2 = (Mother *) poActor2;
	return (poMother1->actor_id == poMother2->actor_id ? 0 :
			poMother1->actor_id < poMother2->actor_id ? -1 : 1);
}
Thread asAllMotherActorSetTree	*asAllMother;

void PreSimulation() {
}

void PostSimulation() {
}

void UserTables() {
}

BOOL ValidateParameters( SCENARIO_EVENT eEvent ) {
	BOOL bProceed = TRUE;

	 return bProceed;
}

void InitActorSets()
{

	asAllChild = new asAllChildActorSetTree();
	asAllMother = new asAllMotherActorSetTree();
}

void	DeleteActorSets()
{

	delete asAllChild;
	asAllChild = NULL;
	delete asAllMother;
	asAllMother = NULL;
}

} // namespace 
