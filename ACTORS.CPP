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
	if ( !VerifyParameter( pszError, "MortalityHazard" ) ) bResult = FALSE;
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

Parameters::Parameters()
{
	Initializer( &MortalityHazard, "MortalityHazard" );
	Initializer( &StartingPopulationSize, "StartingPopulationSize" );
}

void ProcessModelGeneratedParameters()
{
}

void InitActors()
{
	gpoEventQueue = new EventQueue();
	gpoEmptyMother = NULL;
	gpoEmptyTicker = NULL;
}

void DeleteActors()
{
	delete gpoEventQueue;
	gctrlSection.Lock();
	gctrlSection.Unlock();



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
int ::ganSizeOfActors[2] = {
	sizeof(Mother) + sizeof(MotherDerivedStates) + sizeof(MotherTable),
	sizeof(Ticker) + sizeof(TickerDerivedStates),
};

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
	nActorNumber = 0;
	InitializeStates();
}

void Mother::DeleteActor()
{
	delete poDerivedStates;
	delete poStateFunctions;
}

#pragma optimize( "", off )

void Mother::InitializeStates()
{
	// initialization of states
	actor_id = (long) 0;
	actor_subsample_weight = (double) 0;
	actor_weight = (double) 0;
	age = (TIME) 0;
#line 12 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	alive = TRUE != 0;
#line 158 "ACTORS.CPP"
	case_id = (long) 0;
	case_seed = (double) 0;
#line 11 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	CHKLMT(0, report_time, r_min_REPORT_TIME, r_max_REPORT_TIME);
	report_time = 0 < r_min_REPORT_TIME ? r_min_REPORT_TIME : 0 > r_max_REPORT_TIME ? r_max_REPORT_TIME : (REPORT_TIME) 0;
#line 164 "ACTORS.CPP"
	time = (TIME) gdThreadCurrentTime;
	actorset_filter_asAllMother = FALSE;
	alive_value_in = FALSE;
	poDerivedStates->duration_ = (TIME) 0;
	poDerivedStates->duration__time = (TIME) gdThreadCurrentTime;
	duration__max_value_out = (TIME) 0;
	duration__min_value_out = (TIME) 0;
	table_filter_DurationOfLife = FALSE;
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

	// initialization of actor set nodes
	poasAllMother = NULL;

	// initialization of events
	poMortalityEvent = gpoEventQueue->NewEvent( this, 0, 0 );
	//Initalize derived states
#line 47 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_alive_value_in( logical( alive ) );
#line 201 "ACTORS.CPP"
#line 47 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_duration__max_value_out( TIME( poDerivedStates->duration_ ) );
#line 204 "ACTORS.CPP"
#line 47 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_duration__min_value_out( TIME( poDerivedStates->duration_ ) );
#line 207 "ACTORS.CPP"
	CountActor(0);
}

#pragma optimize( "", off )

void Mother::StartSpecial2()
{
	// initialization of expression and history states
	poStateFunctions->Set_time(TIME( CoarsenMantissa( time ) ), true);
	poStateFunctions->Set_age(TIME( CoarsenMantissa( age ) ), true);
	gdThreadCurrentTime = time;
	__time = time;
#line 7 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_actorset_filter_asAllMother( logical( 1 ) );
#line 222 "ACTORS.CPP"
	poDerivedStates->Set_duration_( (TIME) 0 );
	poDerivedStates->duration__time = time;
#line 41 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
	poStateFunctions->Set_table_filter_DurationOfLife( logical( 1 ) );
#line 227 "ACTORS.CPP"

	// initialization of cross-tabulation updates
	tableinfo.tab0events = -1;
	tableinfo.tab0open0 = (real) ( alive_value_in );
	tableinfo.tab0open1 = (real) ( poDerivedStates->duration_ );
	tableinfo.tab0open2 = (real) ( duration__max_value_out );
	tableinfo.tab0open3 = (real) ( duration__min_value_out );
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
	CountFinishedActor(0);
	bUpdates = FALSE;
	gpoEventQueue->DestroyEvent( poMortalityEvent );

	// close all updates
	if ( gplTableIndexes[0] >= 0 && ( tableinfo.tab0dim_filter || table_filter_DurationOfLife ) ) {
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
		CloseUpdate( 0, 0, tableinfo.tab0open0, alive_value_in, table_filter_DurationOfLife, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 0, 1, tableinfo.tab0open1, poDerivedStates->Get_duration_(), table_filter_DurationOfLife, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 0, 2, tableinfo.tab0open2, poStateFunctions->Get_duration__max_value_out(), table_filter_DurationOfLife, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
		CloseUpdate( 0, 3, tableinfo.tab0open3, poStateFunctions->Get_duration__min_value_out(), table_filter_DurationOfLife, tableinfo.tab0cell, tableinfo.tab0total_cell, actor_weight, actor_subsample_weight );
	}
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

double Mother::EventTime( int nEventNum, int *pnEventInfo )
{
	double	event_time = double( TIME_INFINITE );

	gdRuntimeTime = (double) time;
	gnRuntimeActorId = actor_id;
	switch( nEventNum ) {
		case 0:
			gszRuntimeEventName = "Mother.MortalityEvent(time)";
			event_time = (double) timeMortalityEvent();
			TRCEVNT(case_seed, "Mother", actor_id, "timeMortalityEvent", event_time);
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
			TRCEVNT(case_seed, "Mother", actor_id, "Mother.MortalityEvent", time);
			gszRuntimeEventName = "Mother.MortalityEvent(implement)";
			MortalityEvent();
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

logical MotherStateFunctions::Set_alive( logical value )
{

	value = value != 0;
	if ( poParent->alive != value )
	{


		logical	OldValue;
		OldValue = poParent->alive;
		poParent->alive = value;

#line 47 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/MotherCore.mpp"
		Set_alive_value_in( poParent->alive );
#line 399 "ACTORS.CPP"
	}
	return poParent->alive = value;
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
			if ( gplTableIndexes[0] >= 0 ) {
				if ( poParent->tableinfo.tab0events == poParent->__events ) {
					poParent->tableinfo.tab0close0 = (real) poParent->alive_value_in;
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
			if ( gplTableIndexes[0] >= 0 ) {
				if ( poParent->tableinfo.tab0events == poParent->__events ) {
					poParent->tableinfo.tab0close2 = (real) poParent->duration__max_value_out;
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
			if ( gplTableIndexes[0] >= 0 ) {
				if ( poParent->tableinfo.tab0events == poParent->__events ) {
					poParent->tableinfo.tab0close3 = (real) poParent->duration__min_value_out;
				}
			}
		}
	}
	return poParent->duration__min_value_out = value;
}

TIME	MotherStateFunctions::Get_duration__min_value_out(){
return poDerivedStates->Get_duration_();
};

logical MotherStateFunctions::Set_table_filter_DurationOfLife( logical value )
{

	value = value != 0;
	if ( poParent->table_filter_DurationOfLife != value )
	{

		logical	OldValue;
		OldValue = poParent->table_filter_DurationOfLife;
		poParent->table_filter_DurationOfLife = value;
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
	return poParent->table_filter_DurationOfLife = value;
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
			if ( gplTableIndexes[0] >= 0 ) {
				if ( poParent->tableinfo.tab0events == poParent->__events ) {
					poParent->tableinfo.tab0close1 = (real) duration_;
				}
			}
		}
	}
	return duration_ = value;
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
	nActorNumber = 1;
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
#line 625 "ACTORS.CPP"
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
	CountActor(1);
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
	CountFinishedActor(1);
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
Thread Mother	*gpoEmptyMother = NULL;

void DeleteAllMotherActors()
{
}
Thread Ticker	*gpoEmptyTicker = NULL;

void DeleteAllTickerActors()
{
}

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

	asAllMother = new asAllMotherActorSetTree();
}

void	DeleteActorSets()
{

	delete asAllMother;
	asAllMother = NULL;
}

} // namespace 
