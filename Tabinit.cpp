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
	return prTable->GetVal(1, nIndex);
}

double	Tab2Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab3Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab3Expr1( XTable *prTable, int nIndex )
{
	return prTable->GetVal(1, nIndex);
}

double	Tab4Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab5Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab5Expr1( XTable *prTable, int nIndex )
{
	return prTable->GetVal(3, nIndex);
}

double	Tab5Expr2( XTable *prTable, int nIndex )
{
	return prTable->GetVal(2, nIndex);
}

double	Tab5Expr3( XTable *prTable, int nIndex )
{
	return Division( prTable->GetVal(1, nIndex), prTable->GetVal(0, nIndex) );
}

double	Tab6Expr0( XTable *prTable, int nIndex )
{
	return Division( prTable->GetVal(1, nIndex), prTable->GetVal(0, nIndex) );
}

double	Tab7Expr0( XTable *prTable, int nIndex )
{
	return prTable->GetVal(0, nIndex);
}

double	Tab7Expr1( XTable *prTable, int nIndex )
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
		Tab1Expr1 );
	AddTable( 2,
		Tab2Expr0 );
	AddTable( 3,
		Tab3Expr0,
		Tab3Expr1 );
	AddTable( 4,
		Tab4Expr0 );
	AddTable( 5,
		Tab5Expr0,
		Tab5Expr1,
		Tab5Expr2,
		Tab5Expr3 );
	AddTable( 6,
		Tab6Expr0 );
	AddTable( 7,
		Tab7Expr0,
		Tab7Expr1 );
}

#pragma optimize( "", on )


int nTesTPaths = CheckPathsForLibraries_Modgen11();

} // namespace 
