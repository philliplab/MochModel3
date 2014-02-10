#include "model.h"

namespace mm {


double	Tab0Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab0Expr1( XTable *prTable, int nIndex )
{
	return prTable->GetVal(3, nIndex);
}

double	Tab0Expr2( XTable *prTable, int nIndex )
{
	return prTable->GetVal(2, nIndex);
}

double	Tab0Expr3( XTable *prTable, int nIndex )
{
	return Division( prTable->GetVal(1, nIndex), prTable->GetVal(0, nIndex) );
}

double	Tab1Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab1Expr1( XTable *prTable, int nIndex )
{
	return prTable->GetVal(3, nIndex);
}

double	Tab1Expr2( XTable *prTable, int nIndex )
{
	return prTable->GetVal(2, nIndex);
}

double	Tab1Expr3( XTable *prTable, int nIndex )
{
	return Division( prTable->GetVal(1, nIndex), prTable->GetVal(0, nIndex) );
}

double	Tab2Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab2Expr1( XTable *prTable, int nIndex )
{
	return prTable->GetVal(1, nIndex);
}

double	Tab3Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab4Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab4Expr1( XTable *prTable, int nIndex )
{
	return prTable->GetVal(1, nIndex);
}

double	Tab5Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab6Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab7Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab8Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab9Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab10Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab10Expr1( XTable *prTable, int nIndex )
{
	return prTable->GetVal(3, nIndex);
}

double	Tab10Expr2( XTable *prTable, int nIndex )
{
	return prTable->GetVal(2, nIndex);
}

double	Tab10Expr3( XTable *prTable, int nIndex )
{
	return Division( prTable->GetVal(1, nIndex), prTable->GetVal(0, nIndex) );
}

double	Tab11Expr0( XTable *prTable, int nIndex )
{
	return Division( prTable->GetVal(1, nIndex), prTable->GetVal(0, nIndex) );
}

double	Tab12Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab12Expr1( XTable *prTable, int nIndex )
{
	return prTable->GetVal(1, nIndex);
}

#pragma optimize( "", off )
void InitTables()
{
	AddTable( 0,
		Tab0Expr0,
		Tab0Expr1,
		Tab0Expr2,
		Tab0Expr3 );
	AddTable( 1,
		Tab1Expr0,
		Tab1Expr1,
		Tab1Expr2,
		Tab1Expr3 );
	AddTable( 2,
		Tab2Expr0,
		Tab2Expr1 );
	AddTable( 3,
		Tab3Expr0 );
	AddTable( 4,
		Tab4Expr0,
		Tab4Expr1 );
	AddTable( 5,
		Tab5Expr0 );
	AddTable( 6,
		Tab6Expr0 );
	AddTable( 7,
		Tab7Expr0 );
	AddTable( 8,
		Tab8Expr0 );
	AddTable( 9,
		Tab9Expr0 );
	AddTable( 10,
		Tab10Expr0,
		Tab10Expr1,
		Tab10Expr2,
		Tab10Expr3 );
	AddTable( 11,
		Tab11Expr0 );
	AddTable( 12,
		Tab12Expr0,
		Tab12Expr1 );
}

#pragma optimize( "", on )


int nTesTPaths = CheckPathsForLibraries_Modgen11();

} // namespace 
