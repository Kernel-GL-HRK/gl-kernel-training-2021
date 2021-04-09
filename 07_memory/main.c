// SPDX-License-Identifier: GPL-3.0
/**/
#include "memory.h"

int main(void)
{
	char *fname = "res.txt";

	check_malloc(fname);
	check_calloc(fname);
	check_alloca(fname);
	return 0;
}
