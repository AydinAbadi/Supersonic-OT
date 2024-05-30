/*

- Random class.

*/
//**********************************************************************

#include <iostream>
#include "gmp.h"
#include <unordered_map>
#include <gmpxx.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <assert.h>
#include <time.h>
#include <bitset>
#include <vector>
#include <algorithm>

using namespace std;
typedef mpz_t bigint;
typedef unsigned char byte;

//**********************************************************************

class Random{

public:
	Random(){};
	~Random(){};
	void init_rand3(gmp_randstate_t& rand, bigint ran, int bytes);
	bigint* gen_randSet(int size, int max_bitsize);
	

private:
	void get_rand_devurandom(char* buf, int len);
	void get_rand_file(char* buf, int len, char* file);
};
//**********************************************************************
