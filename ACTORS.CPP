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
	if ( !VerifyParameter( pszError, "FertilityRates" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "IncidenceRates" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "IncidenceRatesSource" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MaxBirthWait" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MaxChildren" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MaxInfWait" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MaxLife" ) ) bResult = FALSE;
	if ( !VerifyParameter( pszError, "MinBirthWait" ) ) bResult = FALSE;
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

Parameters::Parameters()
{
	Initializer( &FertilityRates, "FertilityRates" );
	Initializer( &IncidenceRates, "IncidenceRates" );
	Initializer( &IncidenceRatesSource, "IncidenceRatesSource" );
	Initializer( &MaxBirthWait, "MaxBirthWait" );
	Initializer( &MaxChildren, "MaxChildren" );
	Initializer( &MaxInfWait, "MaxInfWait" );
	Initializer( &MaxLife, "MaxLife" );
	Initializer( &MinBirthWait, "MinBirthWait" );
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
#line 6 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	alive = TRUE != 0;
#line 214 "ACTORS.CPP"
	case_id = (long) 0;
	case_seed = (double) 0;
#line 8 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	mother_infected_fm = FALSE != 0;
#line 219 "ACTORS.CPP"
#line 5 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	CHKLMT(0, report_time, r_min_REPORT_TIME, r_max_REPORT_TIME);
	report_time = 0 < r_min_REPORT_TIME ? r_min_REPORT_TIME : 0 > r_max_REPORT_TIME ? r_max_REPORT_TIME : (REPORT_TIME) 0;
#line 223 "ACTORS.CPP"
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
	poDerivedStates->entrances_mother_infected_dl_TRUE = (counter) 0;
	poDerivedStates->entrances_mother_infected_fm_TRUE = (counter) 0;
	mother_infected_dl = FALSE;
	poDerivedStates->sssplit_age_INFECTION_AGES = (INFECTION_AGES) 0;
	poDerivedStates->sssplit_age_INFECTION_AGES_time = (TIME) TIME_INFINITE;
	table_filter_DurationOfLifea = FALSE;
	table_filter_Infection_count_dl = FALSE;
	table_filter_Infection_count_fm = FALSE;
	table_filter_InfectionAgeHist_dl = FALSE;
	table_filter_InfectionAgeHist_fm = FALSE;
	lMother_infected = FALSE;
	__finished = FALSE;
	__time = (TIME) gdThreadCurrentTime;
	__events = 0;
}

#pragma optimize( "", on )

void Child::UpdateLinkedStates()
{
	if ( lMother != NULL ) {
		poStateFunctions->Set_lMother_infected( lMother->infected );
	}
	else {
		poStateFunctions->Set_lMother_infected( logical( 0 ) );
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
	poScheduleEvent = gpoEventQueue->NewEvent( this, 0, 255 );
	schedule_event_time = TIME_INFINITE;
	//Initalize derived states
#line 35 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_alive_value_in( logical( alive ) );
#line 281 "ACTORS.CPP"
#line 35 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_duration__max_value_out( TIME( poDerivedStates->duration_ ) );
#line 284 "ACTORS.CPP"
#line 35 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_duration__min_value_out( TIME( poDerivedStates->duration_ ) );
#line 287 "ACTORS.CPP"
#line 7 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_mother_infected_dl( logical( lMother_infected ) );
#line 290 "ACTORS.CPP"
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
#line 1 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_actorset_filter_asAllChild( logical( 1 ) );
#line 305 "ACTORS.CPP"
	poDerivedStates->Set_duration_( (TIME) 0 );
	poDerivedStates->duration__time = time;
	poDerivedStates->Set_entrances_alive_FALSE( (counter) 0 );
	poDerivedStates->Set_entrances_mother_infected_dl_TRUE( (counter) 0 );
	poDerivedStates->Set_entrances_mother_infected_fm_TRUE( (counter) 0 );
#line 29 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_DurationOfLifea( logical( 1 ) );
#line 313 "ACTORS.CPP"
#line 49 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_Infection_count_dl( logical( 1 ) );
#line 316 "ACTORS.CPP"
#line 68 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_Infection_count_fm( logical( 1 ) );
#line 319 "ACTORS.CPP"
#line 39 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_InfectionAgeHist_dl( logical( 1 ) );
#line 322 "ACTORS.CPP"
#line 58 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
	poStateFunctions->Set_table_filter_InfectionAgeHist_fm( logical( 1 ) );
#line 325 "ACTORS.CPP"

	// initialization of cross-tabulation updates
	tableinfo.tab0events = -1;
	tableinfo.tab0open0 = (real) ( alive_value_in );
	tableinfo.tab0open1 = (real) ( poDerivedStates->duration_ );
	tableinfo.tab0open2 = (real) ( duration__max_value_out );
	tableinfo.tab0open3 = (real) ( duration__min_value_out );
	tableinfo.tab1events = -1;
	tableinfo.tab1open0 = (real) ( poDerivedStates->entrances_alive_FALSE );
	tableinfo.tab1open1 = (real) ( poDerivedStates->entrances_mother_infected_dl_TRUE );
	tableinfo.tab2events = -1;
	tableinfo.tab2open0 = (real) ( table_unit_Infection_count_dl );
	tableinfo.tab3events = -1;
	tableinfo.tab3open0 = (real) ( poDerivedStates->entrances_alive_FALSE );
	tableinfo.tab3open1 = (real) ( poDerivedStates->entrances_mother_infected_fm_TRUE );
	tableinfo.tab4events = -1;
	tableinfo.tab4open0 = (real) ( table_unit_Infection_count_fm );
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
	gpoEventQueue->DestroyEvent(poScheduleEvent);

	// close all updates
	if ( gplTableIndexes[0] >= 0 && ( tableinfo.tab0dim_filter || table_filter_DurationOfLifea ) ) {
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
		CloseUpdate( 0, 0, tableinfo.tab0open0, alive_value_in, table_filter_DurationOfLifea, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 0, 1, tableinfo.tab0open1, poDerivedStates->Get_duration_(), table_filter_DurationOfLifea, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 0, 2, tableinfo.tab0open2, poStateFunctions->Get_duration__max_value_out(), table_filter_DurationOfLifea, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 0, 3, tableinfo.tab0open3, poStateFunctions->Get_duration__min_value_out(), table_filter_DurationOfLifea, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[1] >= 0 && ( tableinfo.tab1dim_filter || table_filter_InfectionAgeHist_dl ) ) {
		if ( tableinfo.tab1events != -1 ) {
			tableinfo.tab1open0 = (real) CloseUpdate( 1, 0, tableinfo.tab1open0, tableinfo.tab1close0, tableinfo.tab1dim_filter, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab1open1 = (real) CloseUpdate( 1, 1, tableinfo.tab1open1, tableinfo.tab1close1, tableinfo.tab1dim_filter, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab1cell = (index) TableCellIndex( 1, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab1total_cell = (index) tableinfo.tab1cell;
		}
		else {
			tableinfo.tab1total_cell = (index) TableTotalCellIndex( 1, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		}
		CloseUpdate( 1, 0, tableinfo.tab1open0, poDerivedStates->entrances_alive_FALSE, table_filter_InfectionAgeHist_dl, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 1, 1, tableinfo.tab1open1, poDerivedStates->entrances_mother_infected_dl_TRUE, table_filter_InfectionAgeHist_dl, tableinfo.tab1cell, tableinfo.tab1total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[2] >= 0 && ( tableinfo.tab2dim_filter || table_filter_Infection_count_dl ) ) {
		if ( tableinfo.tab2events != -1 ) {
			poStateFunctions->Set_table_unit_Infection_count_dl(table_unit_Infection_count_dl+1);
			tableinfo.tab2close0 = (real) table_unit_Infection_count_dl;
			tableinfo.tab2open0 = (real) CloseUpdate( 2, 0, tableinfo.tab2open0, tableinfo.tab2close0, tableinfo.tab2dim_filter, tableinfo.tab2cell, tableinfo.tab2total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab2cell = (index) TableCellIndex( 2, 1,
			mother_infected_dl );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab2total_cell = (index) tableinfo.tab2cell;
		}
		else {
			tableinfo.tab2total_cell = (index) TableTotalCellIndex( 2, 1,
			mother_infected_dl );
		}
		poStateFunctions->Set_table_unit_Infection_count_dl(table_unit_Infection_count_dl+1);
		CloseUpdate( 2, 0, tableinfo.tab2open0, table_unit_Infection_count_dl, table_filter_Infection_count_dl, tableinfo.tab2cell, tableinfo.tab2total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[3] >= 0 && ( tableinfo.tab3dim_filter || table_filter_InfectionAgeHist_fm ) ) {
		if ( tableinfo.tab3events != -1 ) {
			tableinfo.tab3open0 = (real) CloseUpdate( 3, 0, tableinfo.tab3open0, tableinfo.tab3close0, tableinfo.tab3dim_filter, tableinfo.tab3cell, tableinfo.tab3total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab3open1 = (real) CloseUpdate( 3, 1, tableinfo.tab3open1, tableinfo.tab3close1, tableinfo.tab3dim_filter, tableinfo.tab3cell, tableinfo.tab3total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab3cell = (index) TableCellIndex( 3, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab3total_cell = (index) tableinfo.tab3cell;
		}
		else {
			tableinfo.tab3total_cell = (index) TableTotalCellIndex( 3, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		}
		CloseUpdate( 3, 0, tableinfo.tab3open0, poDerivedStates->entrances_alive_FALSE, table_filter_InfectionAgeHist_fm, tableinfo.tab3cell, tableinfo.tab3total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 3, 1, tableinfo.tab3open1, poDerivedStates->entrances_mother_infected_fm_TRUE, table_filter_InfectionAgeHist_fm, tableinfo.tab3cell, tableinfo.tab3total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[4] >= 0 && ( tableinfo.tab4dim_filter || table_filter_Infection_count_fm ) ) {
		if ( tableinfo.tab4events != -1 ) {
			poStateFunctions->Set_table_unit_Infection_count_fm(table_unit_Infection_count_fm+1);
			tableinfo.tab4close0 = (real) table_unit_Infection_count_fm;
			tableinfo.tab4open0 = (real) CloseUpdate( 4, 0, tableinfo.tab4open0, tableinfo.tab4close0, tableinfo.tab4dim_filter, tableinfo.tab4cell, tableinfo.tab4total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab4cell = (index) TableCellIndex( 4, 1,
			mother_infected_fm );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab4total_cell = (index) tableinfo.tab4cell;
		}
		else {
			tableinfo.tab4total_cell = (index) TableTotalCellIndex( 4, 1,
			mother_infected_fm );
		}
		poStateFunctions->Set_table_unit_Infection_count_fm(table_unit_Infection_count_fm+1);
		CloseUpdate( 4, 0, tableinfo.tab4open0, table_unit_Infection_count_fm, table_filter_Infection_count_fm, tableinfo.tab4cell, tableinfo.tab4total_cell, actor_weight, actor_subsample_weight );
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
		CHKSUM(time, 9);
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

#line 35 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
		Set_alive_value_in( poParent->alive );
#line 630 "ACTORS.CPP"
	}
	return poParent->alive = value;
}

logical ChildStateFunctions::Set_mother_infected_fm( logical value )
{

	value = value != 0;
	if ( poParent->mother_infected_fm != value )
	{
		if ( value == TRUE ) {
			poDerivedStates->Set_entrances_mother_infected_fm_TRUE( poDerivedStates->entrances_mother_infected_fm_TRUE + 1 );
		}


		logical	OldValue;
		OldValue = poParent->mother_infected_fm;
		poParent->mother_infected_fm = value;

		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[4] >= 0 && ( poParent->tableinfo.tab4dim_filter || poParent->table_filter_Infection_count_fm ) ) {
				if ( poParent->tableinfo.tab4events == -1 || poParent->tableinfo.tab4events < poParent->__events ) {
					if ( poParent->tableinfo.tab4events != -1 ) {
						Set_table_unit_Infection_count_fm(poParent->table_unit_Infection_count_fm+1);
						poParent->tableinfo.tab4close0 = (real) poParent->table_unit_Infection_count_fm;
						poParent->tableinfo.tab4open0 = (real) poParent->CloseUpdate( 4, 0, poParent->tableinfo.tab4open0, poParent->tableinfo.tab4close0, poParent->tableinfo.tab4dim_filter, poParent->tableinfo.tab4cell, poParent->tableinfo.tab4total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab4events = poParent->__events;
					poParent->tableinfo.tab4close0 = (real) poParent->table_unit_Infection_count_fm;
					poParent->tableinfo.tab4cell = (index) poParent->TableCellIndex( 4, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab4total_cell = (index) poParent->tableinfo.tab4cell;
					}
					else {
						poParent->tableinfo.tab4total_cell = (index) poParent->TableTotalCellIndex( 4, 1,
						OldValue );
					}
					poParent->tableinfo.tab4dim_filter = poParent->table_filter_Infection_count_fm;
				}
			}
		}
	}
	return poParent->mother_infected_fm = value;
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
			if ( gplTableIndexes[2] >= 0 ) {
				if ( poParent->tableinfo.tab2events == poParent->__events ) {
					poParent->tableinfo.tab2close0 = (real) poParent->table_unit_Infection_count_dl;
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
			if ( gplTableIndexes[4] >= 0 ) {
				if ( poParent->tableinfo.tab4events == poParent->__events ) {
					poParent->tableinfo.tab4close0 = (real) poParent->table_unit_Infection_count_fm;
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
		}
	}
	return poParent->duration__min_value_out = value;
}

TIME	ChildStateFunctions::Get_duration__min_value_out(){
return poDerivedStates->Get_duration_();
};

logical ChildStateFunctions::Set_mother_infected_dl( logical value )
{

	value = value != 0;
	if ( poParent->mother_infected_dl != value )
	{
		if ( value == TRUE ) {
			poDerivedStates->Set_entrances_mother_infected_dl_TRUE( poDerivedStates->entrances_mother_infected_dl_TRUE + 1 );
		}


		logical	OldValue;
		OldValue = poParent->mother_infected_dl;
		poParent->mother_infected_dl = value;

		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[2] >= 0 && ( poParent->tableinfo.tab2dim_filter || poParent->table_filter_Infection_count_dl ) ) {
				if ( poParent->tableinfo.tab2events == -1 || poParent->tableinfo.tab2events < poParent->__events ) {
					if ( poParent->tableinfo.tab2events != -1 ) {
						Set_table_unit_Infection_count_dl(poParent->table_unit_Infection_count_dl+1);
						poParent->tableinfo.tab2close0 = (real) poParent->table_unit_Infection_count_dl;
						poParent->tableinfo.tab2open0 = (real) poParent->CloseUpdate( 2, 0, poParent->tableinfo.tab2open0, poParent->tableinfo.tab2close0, poParent->tableinfo.tab2dim_filter, poParent->tableinfo.tab2cell, poParent->tableinfo.tab2total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab2events = poParent->__events;
					poParent->tableinfo.tab2close0 = (real) poParent->table_unit_Infection_count_dl;
					poParent->tableinfo.tab2cell = (index) poParent->TableCellIndex( 2, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab2total_cell = (index) poParent->tableinfo.tab2cell;
					}
					else {
						poParent->tableinfo.tab2total_cell = (index) poParent->TableTotalCellIndex( 2, 1,
						OldValue );
					}
					poParent->tableinfo.tab2dim_filter = poParent->table_filter_Infection_count_dl;
				}
			}
		}
	}
	return poParent->mother_infected_dl = value;
}

logical ChildStateFunctions::Set_table_filter_DurationOfLifea( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_DurationOfLifea != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_DurationOfLifea;
		poParent->table_filter_DurationOfLifea = value;
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
	return poParent->table_filter_DurationOfLifea = value;
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
			if ( gplTableIndexes[2] >= 0 ) {
				if ( poParent->tableinfo.tab2events == -1 || poParent->tableinfo.tab2events < poParent->__events ) {
					if ( poParent->tableinfo.tab2events != -1 ) {
						Set_table_unit_Infection_count_dl(poParent->table_unit_Infection_count_dl+1);
						poParent->tableinfo.tab2close0 = (real) poParent->table_unit_Infection_count_dl;
						poParent->tableinfo.tab2open0 = (real) poParent->CloseUpdate( 2, 0, poParent->tableinfo.tab2open0, poParent->tableinfo.tab2close0, poParent->tableinfo.tab2dim_filter, poParent->tableinfo.tab2cell, poParent->tableinfo.tab2total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab2events = poParent->__events;
					poParent->tableinfo.tab2close0 = (real) poParent->table_unit_Infection_count_dl;
					poParent->tableinfo.tab2cell = (index) poParent->TableCellIndex( 2, 1,
						poParent->mother_infected_dl );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab2total_cell = (index) poParent->tableinfo.tab2cell;
					}
					else {
						poParent->tableinfo.tab2total_cell = (index) poParent->TableTotalCellIndex( 2, 1,
						poParent->mother_infected_dl );
					}
					poParent->tableinfo.tab2dim_filter = OldValue;
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
			if ( gplTableIndexes[4] >= 0 ) {
				if ( poParent->tableinfo.tab4events == -1 || poParent->tableinfo.tab4events < poParent->__events ) {
					if ( poParent->tableinfo.tab4events != -1 ) {
						Set_table_unit_Infection_count_fm(poParent->table_unit_Infection_count_fm+1);
						poParent->tableinfo.tab4close0 = (real) poParent->table_unit_Infection_count_fm;
						poParent->tableinfo.tab4open0 = (real) poParent->CloseUpdate( 4, 0, poParent->tableinfo.tab4open0, poParent->tableinfo.tab4close0, poParent->tableinfo.tab4dim_filter, poParent->tableinfo.tab4cell, poParent->tableinfo.tab4total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab4events = poParent->__events;
					poParent->tableinfo.tab4close0 = (real) poParent->table_unit_Infection_count_fm;
					poParent->tableinfo.tab4cell = (index) poParent->TableCellIndex( 4, 1,
						poParent->mother_infected_fm );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab4total_cell = (index) poParent->tableinfo.tab4cell;
					}
					else {
						poParent->tableinfo.tab4total_cell = (index) poParent->TableTotalCellIndex( 4, 1,
						poParent->mother_infected_fm );
					}
					poParent->tableinfo.tab4dim_filter = OldValue;
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
			if ( gplTableIndexes[1] >= 0 ) {
				if ( poParent->tableinfo.tab1events == -1 || poParent->tableinfo.tab1events < poParent->__events ) {
					if ( poParent->tableinfo.tab1events != -1 ) {
						poParent->tableinfo.tab1open0 = (real) poParent->CloseUpdate( 1, 0, poParent->tableinfo.tab1open0, poParent->tableinfo.tab1close0, poParent->tableinfo.tab1dim_filter, poParent->tableinfo.tab1cell, poParent->tableinfo.tab1total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab1open1 = (real) poParent->CloseUpdate( 1, 1, poParent->tableinfo.tab1open1, poParent->tableinfo.tab1close1, poParent->tableinfo.tab1dim_filter, poParent->tableinfo.tab1cell, poParent->tableinfo.tab1total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab1events = poParent->__events;
					poParent->tableinfo.tab1close0 = (real) poDerivedStates->entrances_alive_FALSE;
					poParent->tableinfo.tab1close1 = (real) poDerivedStates->entrances_mother_infected_dl_TRUE;
					poParent->tableinfo.tab1cell = (index) poParent->TableCellIndex( 1, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab1total_cell = (index) poParent->tableinfo.tab1cell;
					}
					else {
						poParent->tableinfo.tab1total_cell = (index) poParent->TableTotalCellIndex( 1, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					}
					poParent->tableinfo.tab1dim_filter = OldValue;
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
			if ( gplTableIndexes[3] >= 0 ) {
				if ( poParent->tableinfo.tab3events == -1 || poParent->tableinfo.tab3events < poParent->__events ) {
					if ( poParent->tableinfo.tab3events != -1 ) {
						poParent->tableinfo.tab3open0 = (real) poParent->CloseUpdate( 3, 0, poParent->tableinfo.tab3open0, poParent->tableinfo.tab3close0, poParent->tableinfo.tab3dim_filter, poParent->tableinfo.tab3cell, poParent->tableinfo.tab3total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab3open1 = (real) poParent->CloseUpdate( 3, 1, poParent->tableinfo.tab3open1, poParent->tableinfo.tab3close1, poParent->tableinfo.tab3dim_filter, poParent->tableinfo.tab3cell, poParent->tableinfo.tab3total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab3events = poParent->__events;
					poParent->tableinfo.tab3close0 = (real) poDerivedStates->entrances_alive_FALSE;
					poParent->tableinfo.tab3close1 = (real) poDerivedStates->entrances_mother_infected_fm_TRUE;
					poParent->tableinfo.tab3cell = (index) poParent->TableCellIndex( 3, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab3total_cell = (index) poParent->tableinfo.tab3cell;
					}
					else {
						poParent->tableinfo.tab3total_cell = (index) poParent->TableTotalCellIndex( 3, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					}
					poParent->tableinfo.tab3dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_InfectionAgeHist_fm = value;
}

logical ChildStateFunctions::Set_lMother_infected( logical value )
{

	value = value != 0;
	if ( poParent->lMother_infected != value )
	{


		logical	OldValue;
		OldValue = poParent->lMother_infected;
		poParent->lMother_infected = value;

#line 7 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/ChildCore.mpp"
		Set_mother_infected_dl( poParent->lMother_infected );
#line 1073 "ACTORS.CPP"
	}
	return poParent->lMother_infected = value;
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
			if ( gplTableIndexes[1] >= 0 ) {
				if ( poParent->tableinfo.tab1events == poParent->__events ) {
					poParent->tableinfo.tab1close0 = (real) entrances_alive_FALSE;
				}
			}
			if ( gplTableIndexes[3] >= 0 ) {
				if ( poParent->tableinfo.tab3events == poParent->__events ) {
					poParent->tableinfo.tab3close0 = (real) entrances_alive_FALSE;
				}
			}
		}
	}
	return entrances_alive_FALSE = value;
}

counter ChildDerivedStates::Set_entrances_mother_infected_dl_TRUE( counter value )
{

	if ( entrances_mother_infected_dl_TRUE != value ) {

counter		OldValue;
		OldValue = entrances_mother_infected_dl_TRUE;
		entrances_mother_infected_dl_TRUE = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[1] >= 0 ) {
				if ( poParent->tableinfo.tab1events == poParent->__events ) {
					poParent->tableinfo.tab1close1 = (real) entrances_mother_infected_dl_TRUE;
				}
			}
		}
	}
	return entrances_mother_infected_dl_TRUE = value;
}

counter ChildDerivedStates::Set_entrances_mother_infected_fm_TRUE( counter value )
{

	if ( entrances_mother_infected_fm_TRUE != value ) {

counter		OldValue;
		OldValue = entrances_mother_infected_fm_TRUE;
		entrances_mother_infected_fm_TRUE = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[3] >= 0 ) {
				if ( poParent->tableinfo.tab3events == poParent->__events ) {
					poParent->tableinfo.tab3close1 = (real) entrances_mother_infected_fm_TRUE;
				}
			}
		}
	}
	return entrances_mother_infected_fm_TRUE = value;
}

INFECTION_AGES ChildDerivedStates::Set_sssplit_age_INFECTION_AGES( INFECTION_AGES value )
{

	if ( sssplit_age_INFECTION_AGES != value ) {

INFECTION_AGES		OldValue;
		OldValue = sssplit_age_INFECTION_AGES;
		sssplit_age_INFECTION_AGES = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[1] >= 0 && ( poParent->tableinfo.tab1dim_filter || poParent->table_filter_InfectionAgeHist_dl ) ) {
				if ( poParent->tableinfo.tab1events == -1 || poParent->tableinfo.tab1events < poParent->__events ) {
					if ( poParent->tableinfo.tab1events != -1 ) {
						poParent->tableinfo.tab1open0 = (real) poParent->CloseUpdate( 1, 0, poParent->tableinfo.tab1open0, poParent->tableinfo.tab1close0, poParent->tableinfo.tab1dim_filter, poParent->tableinfo.tab1cell, poParent->tableinfo.tab1total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab1open1 = (real) poParent->CloseUpdate( 1, 1, poParent->tableinfo.tab1open1, poParent->tableinfo.tab1close1, poParent->tableinfo.tab1dim_filter, poParent->tableinfo.tab1cell, poParent->tableinfo.tab1total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab1events = poParent->__events;
					poParent->tableinfo.tab1close0 = (real) entrances_alive_FALSE;
					poParent->tableinfo.tab1close1 = (real) entrances_mother_infected_dl_TRUE;
					poParent->tableinfo.tab1cell = (index) poParent->TableCellIndex( 1, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab1total_cell = (index) poParent->tableinfo.tab1cell;
					}
					else {
						poParent->tableinfo.tab1total_cell = (index) poParent->TableTotalCellIndex( 1, 1,
						OldValue );
					}
					poParent->tableinfo.tab1dim_filter = poParent->table_filter_InfectionAgeHist_dl;
				}
			}
			if ( gplTableIndexes[3] >= 0 && ( poParent->tableinfo.tab3dim_filter || poParent->table_filter_InfectionAgeHist_fm ) ) {
				if ( poParent->tableinfo.tab3events == -1 || poParent->tableinfo.tab3events < poParent->__events ) {
					if ( poParent->tableinfo.tab3events != -1 ) {
						poParent->tableinfo.tab3open0 = (real) poParent->CloseUpdate( 3, 0, poParent->tableinfo.tab3open0, poParent->tableinfo.tab3close0, poParent->tableinfo.tab3dim_filter, poParent->tableinfo.tab3cell, poParent->tableinfo.tab3total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab3open1 = (real) poParent->CloseUpdate( 3, 1, poParent->tableinfo.tab3open1, poParent->tableinfo.tab3close1, poParent->tableinfo.tab3dim_filter, poParent->tableinfo.tab3cell, poParent->tableinfo.tab3total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab3events = poParent->__events;
					poParent->tableinfo.tab3close0 = (real) entrances_alive_FALSE;
					poParent->tableinfo.tab3close1 = (real) entrances_mother_infected_fm_TRUE;
					poParent->tableinfo.tab3cell = (index) poParent->TableCellIndex( 3, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab3total_cell = (index) poParent->tableinfo.tab3cell;
					}
					else {
						poParent->tableinfo.tab3total_cell = (index) poParent->TableTotalCellIndex( 3, 1,
						OldValue );
					}
					poParent->tableinfo.tab3dim_filter = poParent->table_filter_InfectionAgeHist_fm;
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
#line 22 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	alive = TRUE != 0;
#line 1263 "ACTORS.CPP"
	case_id = (long) 0;
	case_seed = (double) 0;
#line 21 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MInfection.mpp"
	infected = FALSE != 0;
#line 1268 "ACTORS.CPP"
#line 13 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MFertility.mpp"
	number_children = (int) 0;
#line 1271 "ACTORS.CPP"
#line 21 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	CHKLMT(0, report_time, r_min_REPORT_TIME, r_max_REPORT_TIME);
	report_time = 0 < r_min_REPORT_TIME ? r_min_REPORT_TIME : 0 > r_max_REPORT_TIME ? r_max_REPORT_TIME : (REPORT_TIME) 0;
#line 1275 "ACTORS.CPP"
	time = (TIME) gdThreadCurrentTime;
	actorset_filter_asAllMother = FALSE;
	alive_value_in = FALSE;
	poDerivedStates->changes_infected = (counter) 0;
	changes_infected_event = (counter) 0;
	poDerivedStates->changes_number_children = (counter) 0;
	poDerivedStates->duration_ = (TIME) 0;
	poDerivedStates->duration__time = (TIME) gdThreadCurrentTime;
	duration__max_value_out = (TIME) 0;
	duration__min_value_out = (TIME) 0;
	poDerivedStates->entrances_alive_FALSE = (counter) 0;
	poDerivedStates->entrances_infected_TRUE = (counter) 0;
	integer_age = r_min_LIFE;
	poDerivedStates->ssint_age = (counter) 0;
	poDerivedStates->ssint_age_time = (TIME) TIME_INFINITE;
	poDerivedStates->sssplit_age_INFECTION_AGES = (INFECTION_AGES) 0;
	poDerivedStates->sssplit_age_INFECTION_AGES_time = (TIME) TIME_INFINITE;
	table_filter_ageAtBirth = FALSE;
	table_filter_DurationOfLife = FALSE;
	table_filter_InfectionAge = FALSE;
	table_filter_InfectionAgeHist = FALSE;
	poDerivedStates->value_at_changes_infected_age = (real) 0;
	value_at_changes_infected_age_event = (real) 0;
	__finished = FALSE;
	__time = (TIME) gdThreadCurrentTime;
	__events = 0;
}

#pragma optimize( "", on )

void Mother::UpdateLinkedStates()
{
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
	poScheduleEvent = gpoEventQueue->NewEvent( this, 2, 255 );
	schedule_event_time = TIME_INFINITE;
	//Initalize derived states
#line 45 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_alive_value_in( logical( alive ) );
#line 1331 "ACTORS.CPP"
#line 55 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_changes_infected_event( counter( poDerivedStates->changes_infected ) );
#line 1334 "ACTORS.CPP"
#line 45 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_duration__max_value_out( TIME( poDerivedStates->duration_ ) );
#line 1337 "ACTORS.CPP"
#line 45 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_duration__min_value_out( TIME( poDerivedStates->duration_ ) );
#line 1340 "ACTORS.CPP"
#line 23 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_integer_age( LIFE( poDerivedStates->ssint_age ) );
#line 1343 "ACTORS.CPP"
	integer_age = integer_age < r_min_LIFE ? r_min_LIFE : integer_age > r_max_LIFE ? r_max_LIFE : integer_age;
#line 55 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_value_at_changes_infected_age_event( real( poDerivedStates->value_at_changes_infected_age ) );
#line 1347 "ACTORS.CPP"
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
#line 17 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_actorset_filter_asAllMother( logical( 1 ) );
#line 1362 "ACTORS.CPP"
	poDerivedStates->Set_changes_infected( (counter) 0 );
	poDerivedStates->Set_changes_number_children( (counter) 0 );
	poDerivedStates->Set_duration_( (TIME) 0 );
	poDerivedStates->duration__time = time;
	poDerivedStates->Set_entrances_alive_FALSE( (counter) 0 );
	poDerivedStates->Set_entrances_infected_TRUE( (counter) 0 );
#line 40 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MFertility.mpp"
	poStateFunctions->Set_table_filter_ageAtBirth( logical( 1 ) );
#line 1371 "ACTORS.CPP"
#line 39 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_table_filter_DurationOfLife( logical( 1 ) );
#line 1374 "ACTORS.CPP"
#line 49 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_table_filter_InfectionAge( logical( 1 ) );
#line 1377 "ACTORS.CPP"
#line 58 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_table_filter_InfectionAgeHist( logical( 1 ) );
#line 1380 "ACTORS.CPP"
	poDerivedStates->Set_value_at_changes_infected_age( (real) 0 );

	// initialization of cross-tabulation updates
	tableinfo.tab5events = -1;
	tableinfo.tab5open0 = (real) ( poDerivedStates->changes_number_children );
	tableinfo.tab6events = -1;
	tableinfo.tab6open0 = (real) ( alive_value_in );
	tableinfo.tab6open1 = (real) ( poDerivedStates->duration_ );
	tableinfo.tab6open2 = (real) ( duration__max_value_out );
	tableinfo.tab6open3 = (real) ( duration__min_value_out );
	tableinfo.tab7events = -1;
	tableinfo.tab7open0 = (real) ( changes_infected_event );
	tableinfo.tab7prev0 = (real) ( changes_infected_event );
	tableinfo.tab7events0 = -1;
	tableinfo.tab7open1 = (real) ( value_at_changes_infected_age_event );
	tableinfo.tab7prev1 = (real) ( value_at_changes_infected_age_event );
	tableinfo.tab7events1 = -1;
	tableinfo.tab8events = -1;
	tableinfo.tab8open0 = (real) ( poDerivedStates->entrances_alive_FALSE );
	tableinfo.tab8open1 = (real) ( poDerivedStates->entrances_infected_TRUE );
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
	bUpdates = FALSE;
	gpoEventQueue->DestroyEvent( poBirthEvent );
	gpoEventQueue->DestroyEvent( poInfectionEvent );
	gpoEventQueue->DestroyEvent(poScheduleEvent);

	// close all updates
	if ( gplTableIndexes[5] >= 0 && ( tableinfo.tab5dim_filter || table_filter_ageAtBirth ) ) {
		if ( tableinfo.tab5events != -1 ) {
			tableinfo.tab5open0 = (real) CloseUpdate( 5, 0, tableinfo.tab5open0, tableinfo.tab5close0, tableinfo.tab5dim_filter, tableinfo.tab5cell, tableinfo.tab5total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab5cell = (index) TableCellIndex( 5, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab5total_cell = (index) tableinfo.tab5cell;
		}
		else {
			tableinfo.tab5total_cell = (index) TableTotalCellIndex( 5, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		}
		CloseUpdate( 5, 0, tableinfo.tab5open0, poDerivedStates->changes_number_children, table_filter_ageAtBirth, tableinfo.tab5cell, tableinfo.tab5total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[6] >= 0 && ( tableinfo.tab6dim_filter || table_filter_DurationOfLife ) ) {
		if ( tableinfo.tab6events != -1 ) {
			tableinfo.tab6open0 = (real) CloseUpdate( 6, 0, tableinfo.tab6open0, tableinfo.tab6close0, tableinfo.tab6dim_filter, tableinfo.tab6cell, tableinfo.tab6total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab6open1 = (real) CloseUpdate( 6, 1, tableinfo.tab6open1, tableinfo.tab6close1, tableinfo.tab6dim_filter, tableinfo.tab6cell, tableinfo.tab6total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab6open2 = (real) CloseUpdate( 6, 2, tableinfo.tab6open2, tableinfo.tab6close2, tableinfo.tab6dim_filter, tableinfo.tab6cell, tableinfo.tab6total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab6open3 = (real) CloseUpdate( 6, 3, tableinfo.tab6open3, tableinfo.tab6close3, tableinfo.tab6dim_filter, tableinfo.tab6cell, tableinfo.tab6total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab6cell = (index) TableCellIndex( 6, 0 );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab6total_cell = (index) tableinfo.tab6cell;
		}
		else {
			tableinfo.tab6total_cell = (index) TableTotalCellIndex( 6, 0 );
		}
		CloseUpdate( 6, 0, tableinfo.tab6open0, alive_value_in, table_filter_DurationOfLife, tableinfo.tab6cell, tableinfo.tab6total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 6, 1, tableinfo.tab6open1, poDerivedStates->Get_duration_(), table_filter_DurationOfLife, tableinfo.tab6cell, tableinfo.tab6total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 6, 2, tableinfo.tab6open2, poStateFunctions->Get_duration__max_value_out(), table_filter_DurationOfLife, tableinfo.tab6cell, tableinfo.tab6total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 6, 3, tableinfo.tab6open3, poStateFunctions->Get_duration__min_value_out(), table_filter_DurationOfLife, tableinfo.tab6cell, tableinfo.tab6total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[7] >= 0 && ( tableinfo.tab7dim_filter || table_filter_InfectionAge ) ) {
		if ( tableinfo.tab7events != -1 ) {
			tableinfo.tab7open0 = (real) CloseUpdate( 7, 0, tableinfo.tab7open0, tableinfo.tab7close0, tableinfo.tab7dim_filter, tableinfo.tab7cell, tableinfo.tab7total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab7open1 = (real) CloseUpdate( 7, 1, tableinfo.tab7open1, tableinfo.tab7close1, tableinfo.tab7dim_filter, tableinfo.tab7cell, tableinfo.tab7total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab7cell = (index) TableCellIndex( 7, 1,
			infected );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab7total_cell = (index) tableinfo.tab7cell;
		}
		else {
			tableinfo.tab7total_cell = (index) TableTotalCellIndex( 7, 1,
			infected );
		}
		CloseUpdate( 7, 0, tableinfo.tab7open0, changes_infected_event, table_filter_InfectionAge, tableinfo.tab7cell, tableinfo.tab7total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 7, 1, tableinfo.tab7open1, value_at_changes_infected_age_event, table_filter_InfectionAge, tableinfo.tab7cell, tableinfo.tab7total_cell, actor_weight, actor_subsample_weight );
	}
	if ( gplTableIndexes[8] >= 0 && ( tableinfo.tab8dim_filter || table_filter_InfectionAgeHist ) ) {
		if ( tableinfo.tab8events != -1 ) {
			tableinfo.tab8open0 = (real) CloseUpdate( 8, 0, tableinfo.tab8open0, tableinfo.tab8close0, tableinfo.tab8dim_filter, tableinfo.tab8cell, tableinfo.tab8total_cell, actor_weight, actor_subsample_weight );
			tableinfo.tab8open1 = (real) CloseUpdate( 8, 1, tableinfo.tab8open1, tableinfo.tab8close1, tableinfo.tab8dim_filter, tableinfo.tab8cell, tableinfo.tab8total_cell, actor_weight, actor_subsample_weight );
		}
		tableinfo.tab8cell = (index) TableCellIndex( 8, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		if ( gpModelApp->m_nSubSamples == 1 ) {
			tableinfo.tab8total_cell = (index) tableinfo.tab8cell;
		}
		else {
			tableinfo.tab8total_cell = (index) TableTotalCellIndex( 8, 1,
			poDerivedStates->sssplit_age_INFECTION_AGES );
		}
		CloseUpdate( 8, 0, tableinfo.tab8open0, poDerivedStates->entrances_alive_FALSE, table_filter_InfectionAgeHist, tableinfo.tab8cell, tableinfo.tab8total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 8, 1, tableinfo.tab8open1, poDerivedStates->entrances_infected_TRUE, table_filter_InfectionAgeHist, tableinfo.tab8cell, tableinfo.tab8total_cell, actor_weight, actor_subsample_weight );
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
		CHKSUM(time, 13);
		TRCEVNT(case_seed, "Mother", actor_id, "scheduled - 0", time);
		gszRuntimeEventName = "Mother.scheduled - 0";
		poDerivedStates->Set_ssint_age( poDerivedStates->ssint_age + 1 );
		poDerivedStates->ssint_age_time = WAIT( 1 );
	}

	if (poDerivedStates->sssplit_age_INFECTION_AGES_time == schedule_event_time)
	{
		CHKSUM(time, 14);
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


		logical	OldValue;
		OldValue = poParent->alive;
		poParent->alive = value;

#line 45 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
		Set_alive_value_in( poParent->alive );
#line 1718 "ACTORS.CPP"
	}
	return poParent->alive = value;
}

logical MotherStateFunctions::Set_infected( logical value )
{

	value = value != 0;
	if ( poParent->infected != value )
	{
		poDerivedStates->Set_changes_infected( poDerivedStates->changes_infected + 1 );
		if ( value == TRUE ) {
			poDerivedStates->Set_entrances_infected_TRUE( poDerivedStates->entrances_infected_TRUE + 1 );
		}
		poDerivedStates->Set_value_at_changes_infected_age( poDerivedStates->value_at_changes_infected_age + poParent->age );


		logical	OldValue;
		OldValue = poParent->infected;
		poParent->infected = value;

		{
			int	nIndex;
			int	nSize;
			Child	*prChild;

			nSize = poParent->mlChild->arData.GetSize();
			for ( nIndex = 0; nIndex < nSize; nIndex++ ) {
				prChild = poParent->mlChild->arData[nIndex];
				if ( prChild != NULL ) {
					prChild->UpdateTime();
					prChild->poStateFunctions->Set_lMother_infected( poParent->infected );
				}
			}
		}
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[7] >= 0 && ( poParent->tableinfo.tab7dim_filter || poParent->table_filter_InfectionAge ) ) {
				if ( poParent->tableinfo.tab7events == -1 || poParent->tableinfo.tab7events < poParent->__events ) {
					if ( poParent->tableinfo.tab7events != -1 ) {
						poParent->tableinfo.tab7open0 = (real) poParent->CloseUpdate( 7, 0, poParent->tableinfo.tab7open0, poParent->tableinfo.tab7close0, poParent->tableinfo.tab7dim_filter, poParent->tableinfo.tab7cell, poParent->tableinfo.tab7total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab7open1 = (real) poParent->CloseUpdate( 7, 1, poParent->tableinfo.tab7open1, poParent->tableinfo.tab7close1, poParent->tableinfo.tab7dim_filter, poParent->tableinfo.tab7cell, poParent->tableinfo.tab7total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab7events = poParent->__events;
					if ( poParent->tableinfo.tab7events0 == poParent->__events ) {
						poParent->tableinfo.tab7close0 = (real) poParent->tableinfo.tab7prev0;
					}
					else {
						poParent->tableinfo.tab7close0 = (real) poParent->changes_infected_event;
					}
					if ( poParent->tableinfo.tab7events1 == poParent->__events ) {
						poParent->tableinfo.tab7close1 = (real) poParent->tableinfo.tab7prev1;
					}
					else {
						poParent->tableinfo.tab7close1 = (real) poParent->value_at_changes_infected_age_event;
					}
					poParent->tableinfo.tab7cell = (index) poParent->TableCellIndex( 7, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab7total_cell = (index) poParent->tableinfo.tab7cell;
					}
					else {
						poParent->tableinfo.tab7total_cell = (index) poParent->TableTotalCellIndex( 7, 1,
						OldValue );
					}
					poParent->tableinfo.tab7dim_filter = poParent->table_filter_InfectionAge;
				}
			}
		}
		if ( poParent->poInfectionEvent->cRecalc == 0 ) {
			poParent->poInfectionEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poInfectionEvent );
		};
	}
	return poParent->infected = value;
}

int MotherStateFunctions::Set_number_children( int value )
{

	if ( poParent->number_children != value )
	{
		poDerivedStates->Set_changes_number_children( poDerivedStates->changes_number_children + 1 );


		int	OldValue;
		OldValue = poParent->number_children;
		poParent->number_children = value;

		if ( poParent->poBirthEvent->cRecalc == 0 ) {
			poParent->poBirthEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poBirthEvent );
		};
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
	}
	return poParent->report_time = value;
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
			if ( gplTableIndexes[6] >= 0 ) {
				if ( poParent->tableinfo.tab6events == poParent->__events ) {
					poParent->tableinfo.tab6close0 = (real) poParent->alive_value_in;
				}
			}
		}
	}
	return poParent->alive_value_in = value;
}

counter MotherStateFunctions::Set_changes_infected_event( counter value )
{

	if ( poParent->changes_infected_event != value )
	{

		counter	OldValue;
		OldValue = poParent->changes_infected_event;
		poParent->changes_infected_event = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[7] >= 0 ) {
				poParent->tableinfo.tab7prev0 = (real) OldValue;
				poParent->tableinfo.tab7events0 = poParent->__events;
			}
		}
	}
	return poParent->changes_infected_event = value;
}

TIME MotherStateFunctions::Set_duration__max_value_out( TIME value )
{

	if ( poParent->duration__max_value_out != value )
	{

		TIME	OldValue;
		OldValue = poParent->duration__max_value_out;
		poParent->duration__max_value_out = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[6] >= 0 ) {
				if ( poParent->tableinfo.tab6events == poParent->__events ) {
					poParent->tableinfo.tab6close2 = (real) poParent->duration__max_value_out;
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
			if ( gplTableIndexes[6] >= 0 ) {
				if ( poParent->tableinfo.tab6events == poParent->__events ) {
					poParent->tableinfo.tab6close3 = (real) poParent->duration__min_value_out;
				}
			}
		}
	}
	return poParent->duration__min_value_out = value;
}

TIME	MotherStateFunctions::Get_duration__min_value_out(){
return poDerivedStates->Get_duration_();
};

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
		if ( poParent->poInfectionEvent->cRecalc == 0 ) {
			poParent->poInfectionEvent->cRecalc = 1;
			gpoEventQueue->RecalculateEvent( poParent->poInfectionEvent );
		};
	}
	return poParent->integer_age = value;
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
			if ( gplTableIndexes[5] >= 0 ) {
				if ( poParent->tableinfo.tab5events == -1 || poParent->tableinfo.tab5events < poParent->__events ) {
					if ( poParent->tableinfo.tab5events != -1 ) {
						poParent->tableinfo.tab5open0 = (real) poParent->CloseUpdate( 5, 0, poParent->tableinfo.tab5open0, poParent->tableinfo.tab5close0, poParent->tableinfo.tab5dim_filter, poParent->tableinfo.tab5cell, poParent->tableinfo.tab5total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab5events = poParent->__events;
					poParent->tableinfo.tab5close0 = (real) poDerivedStates->changes_number_children;
					poParent->tableinfo.tab5cell = (index) poParent->TableCellIndex( 5, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab5total_cell = (index) poParent->tableinfo.tab5cell;
					}
					else {
						poParent->tableinfo.tab5total_cell = (index) poParent->TableTotalCellIndex( 5, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					}
					poParent->tableinfo.tab5dim_filter = OldValue;
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
			if ( gplTableIndexes[6] >= 0 ) {
				if ( poParent->tableinfo.tab6events == -1 || poParent->tableinfo.tab6events < poParent->__events ) {
					if ( poParent->tableinfo.tab6events != -1 ) {
						poParent->tableinfo.tab6open0 = (real) poParent->CloseUpdate( 6, 0, poParent->tableinfo.tab6open0, poParent->tableinfo.tab6close0, poParent->tableinfo.tab6dim_filter, poParent->tableinfo.tab6cell, poParent->tableinfo.tab6total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab6open1 = (real) poParent->CloseUpdate( 6, 1, poParent->tableinfo.tab6open1, poParent->tableinfo.tab6close1, poParent->tableinfo.tab6dim_filter, poParent->tableinfo.tab6cell, poParent->tableinfo.tab6total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab6open2 = (real) poParent->CloseUpdate( 6, 2, poParent->tableinfo.tab6open2, poParent->tableinfo.tab6close2, poParent->tableinfo.tab6dim_filter, poParent->tableinfo.tab6cell, poParent->tableinfo.tab6total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab6open3 = (real) poParent->CloseUpdate( 6, 3, poParent->tableinfo.tab6open3, poParent->tableinfo.tab6close3, poParent->tableinfo.tab6dim_filter, poParent->tableinfo.tab6cell, poParent->tableinfo.tab6total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab6events = poParent->__events;
					poParent->tableinfo.tab6close0 = (real) poParent->alive_value_in;
					poParent->tableinfo.tab6close1 = (real) poDerivedStates->Get_duration_();
					poParent->tableinfo.tab6close2 = (real) Get_duration__max_value_out();
					poParent->tableinfo.tab6close3 = (real) Get_duration__min_value_out();
					poParent->tableinfo.tab6cell = (index) poParent->TableCellIndex( 6, 0 );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab6total_cell = (index) poParent->tableinfo.tab6cell;
					}
					else {
						poParent->tableinfo.tab6total_cell = (index) poParent->TableTotalCellIndex( 6, 0 );
					}
					poParent->tableinfo.tab6dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_DurationOfLife = value;
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
			if ( gplTableIndexes[7] >= 0 ) {
				if ( poParent->tableinfo.tab7events == -1 || poParent->tableinfo.tab7events < poParent->__events ) {
					if ( poParent->tableinfo.tab7events != -1 ) {
						poParent->tableinfo.tab7open0 = (real) poParent->CloseUpdate( 7, 0, poParent->tableinfo.tab7open0, poParent->tableinfo.tab7close0, poParent->tableinfo.tab7dim_filter, poParent->tableinfo.tab7cell, poParent->tableinfo.tab7total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab7open1 = (real) poParent->CloseUpdate( 7, 1, poParent->tableinfo.tab7open1, poParent->tableinfo.tab7close1, poParent->tableinfo.tab7dim_filter, poParent->tableinfo.tab7cell, poParent->tableinfo.tab7total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab7events = poParent->__events;
					if ( poParent->tableinfo.tab7events0 == poParent->__events ) {
						poParent->tableinfo.tab7close0 = (real) poParent->tableinfo.tab7prev0;
					}
					else {
						poParent->tableinfo.tab7close0 = (real) poParent->changes_infected_event;
					}
					if ( poParent->tableinfo.tab7events1 == poParent->__events ) {
						poParent->tableinfo.tab7close1 = (real) poParent->tableinfo.tab7prev1;
					}
					else {
						poParent->tableinfo.tab7close1 = (real) poParent->value_at_changes_infected_age_event;
					}
					poParent->tableinfo.tab7cell = (index) poParent->TableCellIndex( 7, 1,
						poParent->infected );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab7total_cell = (index) poParent->tableinfo.tab7cell;
					}
					else {
						poParent->tableinfo.tab7total_cell = (index) poParent->TableTotalCellIndex( 7, 1,
						poParent->infected );
					}
					poParent->tableinfo.tab7dim_filter = OldValue;
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
			if ( gplTableIndexes[8] >= 0 ) {
				if ( poParent->tableinfo.tab8events == -1 || poParent->tableinfo.tab8events < poParent->__events ) {
					if ( poParent->tableinfo.tab8events != -1 ) {
						poParent->tableinfo.tab8open0 = (real) poParent->CloseUpdate( 8, 0, poParent->tableinfo.tab8open0, poParent->tableinfo.tab8close0, poParent->tableinfo.tab8dim_filter, poParent->tableinfo.tab8cell, poParent->tableinfo.tab8total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab8open1 = (real) poParent->CloseUpdate( 8, 1, poParent->tableinfo.tab8open1, poParent->tableinfo.tab8close1, poParent->tableinfo.tab8dim_filter, poParent->tableinfo.tab8cell, poParent->tableinfo.tab8total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab8events = poParent->__events;
					poParent->tableinfo.tab8close0 = (real) poDerivedStates->entrances_alive_FALSE;
					poParent->tableinfo.tab8close1 = (real) poDerivedStates->entrances_infected_TRUE;
					poParent->tableinfo.tab8cell = (index) poParent->TableCellIndex( 8, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab8total_cell = (index) poParent->tableinfo.tab8cell;
					}
					else {
						poParent->tableinfo.tab8total_cell = (index) poParent->TableTotalCellIndex( 8, 1,
						poDerivedStates->sssplit_age_INFECTION_AGES );
					}
					poParent->tableinfo.tab8dim_filter = OldValue;
				}
			}
		}
	}
	return poParent->table_filter_InfectionAgeHist = value;
}

real MotherStateFunctions::Set_value_at_changes_infected_age_event( real value )
{

	if ( poParent->value_at_changes_infected_age_event != value )
	{

		real	OldValue;
		OldValue = poParent->value_at_changes_infected_age_event;
		poParent->value_at_changes_infected_age_event = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[7] >= 0 ) {
				poParent->tableinfo.tab7prev1 = (real) OldValue;
				poParent->tableinfo.tab7events1 = poParent->__events;
			}
		}
	}
	return poParent->value_at_changes_infected_age_event = value;
}

counter MotherDerivedStates::Set_changes_infected( counter value )
{

	if ( changes_infected != value ) {

		counter OldValue;
		OldValue = changes_infected;
		changes_infected = value;
		poStateFunctions->Set_changes_infected_event( changes_infected );
	}
	return changes_infected = value;
}

counter MotherDerivedStates::Set_changes_number_children( counter value )
{

	if ( changes_number_children != value ) {

counter		OldValue;
		OldValue = changes_number_children;
		changes_number_children = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[5] >= 0 ) {
				if ( poParent->tableinfo.tab5events == poParent->__events ) {
					poParent->tableinfo.tab5close0 = (real) changes_number_children;
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
			if ( gplTableIndexes[6] >= 0 ) {
				if ( poParent->tableinfo.tab6events == poParent->__events ) {
					poParent->tableinfo.tab6close1 = (real) duration_;
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
			if ( gplTableIndexes[8] >= 0 ) {
				if ( poParent->tableinfo.tab8events == poParent->__events ) {
					poParent->tableinfo.tab8close0 = (real) entrances_alive_FALSE;
				}
			}
		}
	}
	return entrances_alive_FALSE = value;
}

counter MotherDerivedStates::Set_entrances_infected_TRUE( counter value )
{

	if ( entrances_infected_TRUE != value ) {

counter		OldValue;
		OldValue = entrances_infected_TRUE;
		entrances_infected_TRUE = value;
		if ( poParent->bUpdates ) {
			if ( gplTableIndexes[8] >= 0 ) {
				if ( poParent->tableinfo.tab8events == poParent->__events ) {
					poParent->tableinfo.tab8close1 = (real) entrances_infected_TRUE;
				}
			}
		}
	}
	return entrances_infected_TRUE = value;
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
			if ( gplTableIndexes[5] >= 0 && ( poParent->tableinfo.tab5dim_filter || poParent->table_filter_ageAtBirth ) ) {
				if ( poParent->tableinfo.tab5events == -1 || poParent->tableinfo.tab5events < poParent->__events ) {
					if ( poParent->tableinfo.tab5events != -1 ) {
						poParent->tableinfo.tab5open0 = (real) poParent->CloseUpdate( 5, 0, poParent->tableinfo.tab5open0, poParent->tableinfo.tab5close0, poParent->tableinfo.tab5dim_filter, poParent->tableinfo.tab5cell, poParent->tableinfo.tab5total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab5events = poParent->__events;
					poParent->tableinfo.tab5close0 = (real) changes_number_children;
					poParent->tableinfo.tab5cell = (index) poParent->TableCellIndex( 5, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab5total_cell = (index) poParent->tableinfo.tab5cell;
					}
					else {
						poParent->tableinfo.tab5total_cell = (index) poParent->TableTotalCellIndex( 5, 1,
						OldValue );
					}
					poParent->tableinfo.tab5dim_filter = poParent->table_filter_ageAtBirth;
				}
			}
			if ( gplTableIndexes[8] >= 0 && ( poParent->tableinfo.tab8dim_filter || poParent->table_filter_InfectionAgeHist ) ) {
				if ( poParent->tableinfo.tab8events == -1 || poParent->tableinfo.tab8events < poParent->__events ) {
					if ( poParent->tableinfo.tab8events != -1 ) {
						poParent->tableinfo.tab8open0 = (real) poParent->CloseUpdate( 8, 0, poParent->tableinfo.tab8open0, poParent->tableinfo.tab8close0, poParent->tableinfo.tab8dim_filter, poParent->tableinfo.tab8cell, poParent->tableinfo.tab8total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
						poParent->tableinfo.tab8open1 = (real) poParent->CloseUpdate( 8, 1, poParent->tableinfo.tab8open1, poParent->tableinfo.tab8close1, poParent->tableinfo.tab8dim_filter, poParent->tableinfo.tab8cell, poParent->tableinfo.tab8total_cell, poParent->actor_weight, poParent->actor_subsample_weight );
					}
					poParent->tableinfo.tab8events = poParent->__events;
					poParent->tableinfo.tab8close0 = (real) entrances_alive_FALSE;
					poParent->tableinfo.tab8close1 = (real) entrances_infected_TRUE;
					poParent->tableinfo.tab8cell = (index) poParent->TableCellIndex( 8, 1,
						OldValue );
					if ( gpModelApp->m_nSubSamples == 1 ) {
						poParent->tableinfo.tab8total_cell = (index) poParent->tableinfo.tab8cell;
					}
					else {
						poParent->tableinfo.tab8total_cell = (index) poParent->TableTotalCellIndex( 8, 1,
						OldValue );
					}
					poParent->tableinfo.tab8dim_filter = poParent->table_filter_InfectionAgeHist;
				}
			}
		}
	}
	return sssplit_age_INFECTION_AGES = value;
}

real MotherDerivedStates::Set_value_at_changes_infected_age( real value )
{

	if ( value_at_changes_infected_age != value ) {

		real OldValue;
		OldValue = value_at_changes_infected_age;
		value_at_changes_infected_age = value;
		poStateFunctions->Set_value_at_changes_infected_age_event( value_at_changes_infected_age );
	}
	return value_at_changes_infected_age = value;
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
#line 18 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/TickerCore.mpp"
	CHKLMT(0, report_time, r_min_REPORT_TIME, r_max_REPORT_TIME);
	report_time = 0 < r_min_REPORT_TIME ? r_min_REPORT_TIME : 0 > r_max_REPORT_TIME ? r_max_REPORT_TIME : (REPORT_TIME) 0;
#line 2367 "ACTORS.CPP"
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

void Mother_mlChild::UpdateReferences()
{
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
