#include "alltests.h"


AllTests::AllTests()
{
}

#include "../server/utils.h"
using namespace delta3;
void AllTests::testUtils1()
{
    #define mycmp(T,l,r) QCOMPARE( fromBytes<T>( toBytes<T>(l) ), r )
    mycmp(int,5,5);
    auto input2 = "asdf";
    mycmp(const char*, input2, input2);
    double input3 = 2.718281828;
    mycmp(double, input3, input3);
}
