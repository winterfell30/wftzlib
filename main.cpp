<<<<<<< HEAD
//ÓÃÓÚ²âÊÔ¿Õ¼äÅäÖÃÆ÷
#include <time.h>
#include "vector.h"
#include <vector>
#include <thread>

void g() { std::cout << "Hello world" << std::endl; }

#define print(x) std::cout << #x << "=" << x << std::endl;

int main()
{
	clock_t start, finish;
	start = clock();
	mystl::vector<int> v(1);
	v.push_back(2);
	v.erase(v.begin());
	print(*v.begin());
	print(*(v.end() - 1));
	print(v.size());
	print(v.capacity());
	for (mystl::vector<int>::iterator it = v.begin(); it != v.end(); it++) print(*it);
	finish = clock();
	std::cout << "time: " << (double)(finish - start) << std::endl;
=======
#include "vector.h"

int main()
{
	mystl::vector<int> a;
	a.insert(a.begin(), 3, 3);
>>>>>>> 1edf3499f81576aee1ac1d52adf065b9397bcbf7
	return 0;
}