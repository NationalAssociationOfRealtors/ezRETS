#!/bin/sh

cat <<JIFFYJIFFYPOP
--- tools/build/v2/tools/darwin.jam	2009-04-14 02:59:30.000000000 -0500
+++ ../boost_1_40_0.fixed/tools/build/v2/tools/darwin.jam	2009-10-08 13:07:48.000000000 -0500
@@ -89,7 +89,7 @@
     local bin ;
     
     # - The configured compile driver command.
-    local command = [ common.get-invocation-command darwin : g++ : $(command) ] ;
+    local command = [ common.get-invocation-command darwin : /usr/bin/g++-4.0 : $(command) ] ;
     
     # The version as reported by the compiler
     local real-version ;
@@ -306,7 +306,7 @@
 
 arch-addr-flags darwin OPTIONS : combined : 32 : -arch i386 -arch ppc : default ;
 arch-addr-flags darwin OPTIONS : combined : 64 : -arch x86_64 -arch ppc64 ;
-arch-addr-flags darwin OPTIONS : combined : 32_64 : -arch i386 -arch ppc -arch x86_64 -arch ppc64 ;
+arch-addr-flags darwin OPTIONS : combined : 32_64 : -arch i386 -arch ppc -arch x86_64 ;
 
 arch-addr-flags darwin OPTIONS : x86 : 32 : -arch i386 : default ;
 arch-addr-flags darwin OPTIONS : x86 : 64 : -arch x86_64 ;
diff -ur tools/build/v2/tools/gcc.jam ../boost_1_40_0.fixed/tools/build/v2/tools/gcc.jam
--- tools/build/v2/tools/gcc.jam	2009-07-11 06:04:31.000000000 -0500
+++ ../boost_1_40_0.fixed/tools/build/v2/tools/gcc.jam	2009-10-08 12:11:49.000000000 -0500
@@ -373,7 +373,7 @@
             {
                 option = -m32 ;
             }
-            else
+            else if $(model) != 32_64
             {
                 option = -m64 ;
             }
JIFFYJIFFYPOP

bjam --layout=system --toolset=darwin architecture=combined address-model=32_64 macosx-version=10.4 macosx-version-min=10.4
