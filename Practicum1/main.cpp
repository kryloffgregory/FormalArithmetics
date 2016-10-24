#include <iostream>
#include <CAutomat.h>
using namespace std;

int main()
{
  CAutomat G = CAutomat("bba.ab.+*b..*");
  G.print();
  return 0;
}

