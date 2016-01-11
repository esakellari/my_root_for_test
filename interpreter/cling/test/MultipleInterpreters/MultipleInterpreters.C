//------------------------------------------------------------------------------
// CLING - the C++ LLVM-based InterpreterG :)
//
// This file is dual-licensed: you can choose to license it under the University
// of Illinois Open Source License or the GNU Lesser General Public License. See
// LICENSE.TXT for details.
//------------------------------------------------------------------------------

// RUN: cat %s | %cling 2>&1 | FileCheck %s

// Test to check the functionality of the multiple interpreters.
// Create a "child" interpreter and use gCling as its "parent".

#include "cling/Interpreter/Interpreter.h"
#include <iostream>

const char * const argV = "cling";

cling::Interpreter ChildInterp(*gCling, 1, &argV);

//Declare something in the parent interpreter
gCling->declare("void foo(){ std::cout << \"foo(void)\" << std::endl; }");

//Check if function overloading works
gCling->declare("void foo(int i){ std::cout << \"foo(int)\" << std::endl; }");

//Then execute it from the child interpreter
ChildInterp.execute("foo()");
//CHECK:foo(void)

ChildInterp.execute("foo(1)");
//CHECK:foo(int)

.q
