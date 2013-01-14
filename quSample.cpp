/*
** QuBit - Sample Code
*/
#include <math.h>
#include "quBit.hpp"
#include "quSample.hpp"


CQuBit<int> Qmin(CQuBit<int> &ql)
{
	// PERL: eigenstates(any(@_) <= any(@_))
	return (ql.Any() <= ql.All()).Eigenstates();
}


CQuBit<int> Qmax(CQuBit<int> &ql)
{
	// PERL: eigenstates(any(@_) >= any(@_))
	return (ql.Any() >= ql.All()).Eigenstates();
}


bool QIsPrime(int i)
{
// PERL: $_[0]==2 || $_[0] % all(2..sqrt($_[0])+1) != 0

CQuBit<int> prime, ans;

	if (i==2)	return true;
	prime.AddRange(2, (int)(sqrt((float)i)+1.0f));
	ans = (i % prime.All()) != 0;
	return ans.GetBoolResult();
}


CQuBit<float> QFactors(int v)
{
	float i = (float)v;
// PERL: $q = $n / any(2..$n-1); eigenstates(floor($q)==$q);
// or    eigenstates( int($_[0] / any(2..$n-1)) == ($_[0] / any(2..$n-1)
CQuBit<float> n, q;
CQuBit<float> results;

	n.AddRange(2.0f, (float)(i-1));
	q = i/n.Any();
	q = q.Floor()==q;
	return q.Eigenstates();
}


CQuBit<float> QGCD(int a, int b)
{
// PERL: $common = all(any(factors($x)), any(factors($y)));
//		any(eigenstates $common) > all(eigenstates $common);
CQuBit<float> af, bf, ans;

	af = QFactors(a);
	bf = QFactors(b);
	ans = af.All(af, bf);
	return (ans.Any() >= ans.All()).Eigenstates();
}

int GetQGCD(int a, int b)
{
	return (int)QGCD(a,b).GetItem(0);
}
