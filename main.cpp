#include "gc_pointer.h"

int main()
{
	try 
	{
		/* Using single object pointer */
		pointer<int> p = new int(1);
		p = new int(2);
		p = new int(3);
		p = new int(4);
		pointer<int>::collect();
		std::cout << "*p: " << *p << std::endl;

		/* Using an array */
		pointer<int, 10> ap = new int[10];
		for (int i = 0; i < 10; i++)
		{
			ap[i] = i;
		}
		for (int i = 0; i < 10; i++)
		{
			std::cout << ap[i] << " ";
		}
	}

	catch (std::bad_alloc& exc)
	{
		std::cout << "Allocation failure!\n";
	}

	return 0;
}