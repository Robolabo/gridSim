AC_PREREQ([2.63])

AC_INIT([gridSim],[0.0.0])
AM_INIT_AUTOMAKE(gridSim, 0.0.0)
AC_CONFIG_HEADERS([config.h])

dnl libreadline is not used AFAIK
dnl AC_CHECK_HEADERS(readline/readline.h, [], AC_MSG_ERROR([You need to install readline.h (probably in a readline-dev package)]))

dnl check for programs
AC_PROG_CC
AC_PROG_CXX([g++-4.4 g++])
AC_PROG_INSTALL
AC_PROG_LIBTOOL

dnl c flags tuning 

CFLAGS="-O3"
CXXFLAGS="-O3"

AC_ARG_ENABLE( [i686], 
	        AS_HELP_STRING([--enable-i686],[enable i686 optimizations]), 
		[CFLAGS=" -03 -march=i686 -pipe -fomit-frame-pointer -fforce-addr -frerun-cse-after-loop -frerun-loop-opt -falign-functions=4 -maccumulate-outgoing-args -ffast-math ", CXXFLAGS=" -O3 -march=i686 -pipe -fomit-frame-pointer -fforce-addr -frerun-cse-after-loop -frerun-loop-opt -falign-functions=4 -maccumulate-outgoing-args -ffast-math"])

AC_ARG_ENABLE( [athlonxp], 
	        AS_HELP_STRING([--enable-athlonxp],[enable Athlon XP optimizations]), 
		[CFLAGS=" -O3 -march=athlon-xp -mfpmath=sse -mmmx -m3dnow -msse -pipe -fomit-frame-pointer -fforce-addr -frerun-cse-after-loop -frerun-loop-opt -falign-functions=4 -maccumulate-outgoing-args -ffast-math ", CXXFLAGS=" -O3 -march=athlon-xp -mfpmath=sse -mmmx -m3dnow -msse -pipe -fomit-frame-pointer -fforce-addr -frerun-cse-after-loop -frerun-loop-opt -falign-functions=4 -maccumulate-outgoing-args -ffast-math"])

 AC_ARG_ENABLE( [debug], 
		AS_HELP_STRING([--enable-debug],[enable debugging]), 
		[CFLAGS=" -g", CXXFLAGS=" -g"])

 AC_ARG_ENABLE( [profile], 
		AS_HELP_STRING([--enable-profile],[enable profiling]), 
		[CFLAGS=" -g -pg", CXXFLAGS=" -g -pg"])

AC_ARG_ENABLE( [cluster], 
	        AS_HELP_STRING([--enable-cluster],[enable Opteron optimizations]), 
		[CFLAGS=" -O3 -march=opteron -pipe -fomit-frame-pointer -fforce-addr -frerun-cse-after-loop -frerun-loop-opt -falign-functions=4 -maccumulate-outgoing-args -ffast-math -fprefetch-loop-arrays", CXXFLAGS=" -O3 -march=opteron -pipe -fomit-frame-pointer -fforce-addr -frerun-cse-after-loop -frerun-loop-opt -falign-functions=4 -maccumulate-outgoing-args -ffast-math -fprefetch-loop-arrays"])

AC_CONFIG_FILES([
Makefile
common/Makefile
graphics_engine/Makefile
simulator/Makefile
simulator/grid/Makefile	 
simulator/elements/Makefile
simulator/controllers/Makefile	
simulator/user/Makefile 
simulator/main_control/Makefile
])

AC_OUTPUT
