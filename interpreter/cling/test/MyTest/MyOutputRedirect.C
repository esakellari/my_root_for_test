// RUN: cat %s | %cling | cat /tmp/aa.log | FileCheck --check-prefix=CHECK-REDIRECTOUT %s
#include <iostream>

//Test redirect
.> /tmp/aa.log
.x /Users/admin/root/hello.C
//CHECK-REDIRECTOUT: Hello!



