#include <iostream>
#include <CAutomat.h>
using namespace std;

int main()
{
  CAutomat G = CAutomat("acb..bab.c. * .ab.ba.+.+*a.");
  G.print();
  cout << G.minWordLength('b', 3);
  return 0;
}
//ab+c.aba.∗.bac.+.+∗
//acb..bab.c. ∗ .ab.ba. + . + ∗a.
