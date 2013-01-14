#include <vector>
#include <iterator>
#include "quBit.hpp"
#include "quSample.hpp"

int main(int argc, char* argv[])
{
CQuBit<float> ans;
int i;

	for(i=1;i<100;i++) {
		if (QIsPrime(i)) {
			cout<< i << " is prime!\n";
		}
	}
	//
	ans = QFactors(255);
	cout << "The " << ans.GetCount() <<" factors of 255 are, " <<ans << endl;
	cout << "You can also reference them with [], i.e. ans[0]=" << ans[0] << endl;
	
	// If you're Looking for factors less than 10, you could write...
	
	// Code Sample #1
	for(i=2;i<=10;i++) {
		if ((ans.Any()==i).GetBoolResult()) {
			cout << i << " is a factor!" << endl;
		} else {
			cout << i << " is not a factor" << endl;
		}
	}

	// Code Sample #2
	CQuBit<float> ltt(2,10);
	cout << "The factors are:" << ltt.All(ans) << endl;

	// Code Sample #3
	CQuBit<float> minvalue(10,25,5);
	CQuBit<float> answer;
	answer = minvalue.Any() <= minvalue.All();
	cout << "The lowest is:" << answer.Eigenstates() << endl;

	return 0;
}


