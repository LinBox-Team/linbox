/* -*- mode: c; style: linux -*- */

/* tests/test-ntl-zz_p.cpp
 * Copyright (C) 2002 William J. Turner
 *
 * Written by William J. Turner <wjturner@math.ncsu.edu>
 *
 */

#include "linbox-config.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "linbox/field/ntl.h"

#include "test-common.h"
#include "test-generic.h"

using namespace LinBox;

int main (int argc, char **argv)
{
	static size_t n = 10000;
	static int iterations = 10;

        static Argument args[] = {
		{ 'n', "-n N", "Set dimension of test vectors to NxN (default 10000)",      TYPE_INT,     &n },
		{ 'i', "-i I", "Perform each test for I iterations (default 10)",           TYPE_INT,     &iterations },
                { '\0' }
        };

        parseArguments (argc, argv, args);

	cout << "UnparametricField<NTL::RR> field test suite" << endl << endl;
	cout.flush ();
	bool pass = true;

	UnparametricField<NTL::RR> F;

	// Make sure some more detailed messages get printed
	commentator.getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (2);

	if (!runFieldTests (F, "UnparametricField<NTL::RR>", iterations, n, false)) pass = false;

#if 0
	FieldArchetype K(new UnparametricField<NTL::RR>);

	if (!testField<FieldArchetype> (K, "Testing archetype with envelope of UnField<NTL::RR> field"))
		pass = false;
#endif

	return pass ? 0 : -1;
}
