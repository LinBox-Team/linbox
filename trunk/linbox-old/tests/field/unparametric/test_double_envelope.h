/* File: src/examples/field/unparametric/test_double_envelope.h
 * Author: William J Turner for the LinBox group
 */

#ifndef _TEST_DOUBLE_ENVELOPE_
#define _TEST_DOUBLE_ENVELOPE_

#include "Examples/test_linbox.h"
#include "LinBox/unparam_field.h"
#include "LinBox/field_archetype.h"
#include "LinBox/field_envelope.h"
#include "LinBox/randiter_envelope.h"
#include "LinBox/randiter_archetype.h"

// Specialization of setup_field for double_envelope
template <> 
bool test_linbox::test<test_linbox::field_categories::double_envelope_tag>(void) const
{
  LinBox::unparam_field<double> F;
  LinBox::Field_envelope< LinBox::unparam_field<double> > E(F);
  LinBox::Field_envelope< LinBox::unparam_field<double> >::element e;
  LinBox::Field_envelope< LinBox::unparam_field<double> >::randIter r(E);
  LinBox::Field_archetype A(&E, &e, &r);
  return run_tests(A);
} // template <> bool test_linbox<double_envelope_tag>(void)

#endif // _TEST_DOUBLE_ENVELOPE_
