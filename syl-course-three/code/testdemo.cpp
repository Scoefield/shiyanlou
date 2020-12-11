#include <iostream>
#include <string>
#include <cstdio>

using namespace std;
 
int main()
{
	int as = 0x12345678;
	char *p =(char *) &as;
	if(*p == 0x12)
		cout << "Big-endian" <<endl;
	else
		cout << "Little-endian" <<endl;
		
		
	union {
		short  s;
		char   c[sizeof(short)];
	} un;
    
	un.s = 0x0102;
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2)
			printf("Big-endian\n");
		else if (un.c[0] == 2 && un.c[1] == 1)
			printf("Little-endian\n");
		else
			printf("unknown\n");
	} else
		printf("sizeof(short) = %d\n", sizeof(short));
 
	return 0;
}