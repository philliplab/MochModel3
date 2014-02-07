#include "model.h"
static int HaveAGoodDay;
namespace mm {
#line 1 "c:/users/administrator/documents/visual studio 2010/Projects/MochModel3/TickerCore.mpp"
//LABEL (TickerCore, EN) Definition and initialization of Ticker actor

/*NOTE(TickerCore, EN)
	This module contains the basic information associated with a Ticker actor.
	One single Ticker actor is instantiated in a simulation.  It is used to synchronise
	time among all of the actors in the population in order to ensure
	accurate reports.
*/

//range REPORT_TIME  //EN Reporting intervals
//{
//	0, 200
//};

//actor Ticker //EN Actor for synchronising population-level activities
//{
//	//EN Report time
//	REPORT_TIME report_time = { 0 };
//
//	//EN Time of next Tick event
//	TIME next_tick;
//
//	//LABEL(Ticker.Start, EN) Starts the ticker
//	void	Start();
//
//	//LABEL(Ticker.Finish, EN) Finishes the actor
//	void	Finish();	
//
//	event	timeTickEvent, TickEvent; //EN Tick event
//};

/* NOTE(Ticker.Start,EN) 
	Initializes the Ticker actor.
*/
void Ticker::Start( )
{ if (__finished) {StartAfterFinish(); return; }; StartSpecial1();
	 poStateFunctions->Set_time(  (TIME) 0, true );
	 poStateFunctions->Set_age(  0, true );
	 poStateFunctions->Set_next_tick(  0 );

StartSpecial2();}


/*NOTE(Ticker.Finish, EN)
	The Finish function terminates the simulation of the ticker actor.
*/
void Ticker::Finish()
{
if (__finished) {	DoubleFinish(case_seed);	return; } else __finished = TRUE;FinishSpecial();}


/* NOTE(Ticker.TickEvent,EN)
	Increments the clock and synchronizes actors for reporting purposes.
*/
TIME Ticker::timeTickEvent()
{
	return (TIME) next_tick ;
}

void Ticker::TickEvent()
{
	// Increment report time
	 poStateFunctions->Set_report_time(  COERCE(REPORT_TIME,  report_time + 1) );

	// Age all Mothers to the current time.
	int nMother = asAllMother->Count();
	for ( int nJ = 0; nJ < nMother; nJ++ ) 
	{
		Mother *paMother = asAllMother->Item( nJ );
		paMother-> UpdateTime() -> poStateFunctions->Set_report_time(  report_time  );
	}

	// Schedule the next tick.
	// The next line spreads reporting times evenly over the simulation
	//next_tick = next_tick + SIMULATION_END() / SIZE(REPORT_TIME);
	// The next line sets reporting times at equal intervals of size 1.0
	 poStateFunctions->Set_next_tick(   next_tick + (TIME) 1.0 );
}




} // namespace 
