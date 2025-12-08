/*
	vcm_test

	start testing vcm loader. We do this by rippling calls to modules
	test1 to test3. We then call malloc to remove all free space and then
	cause module unloading
*/

void	vcm_test1(int i);

void	vcm_test()
{
	vcm_test1(0);
}

