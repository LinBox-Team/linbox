# Check for Maple, for maple examples
# Rich Seagraves, 2002-10-31
# Modified by PAscal Giorgi, 2003-12-03
# a gutted and modified version of ntl-check.m4 by Bradford Hovinen, 2001-06-13
# which was in turn inspired by gnome-bonobo-check.m4 by Miguel de Icaza, 99-04-12
# Stolen from Chris Lahey
# stolen from Manish Singh again
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor


dnl LB_CHECK_MAPLE()

AC_DEFUN([LB_CHECK_MAPLE],
[

AC_ARG_WITH(maple,
	    [ --with-maple=<path>|yes|no 
					   Use Maple library. 
					   If argument is no, you do not have the library installed on your machine (set as default).
					   If argument is yes or <empty> that means the library is  well installed and so reachable.				   	 				   Otherwise you give the <path> to the directory which contains the Software. 
	    ],
            [if test "$withval" = yes ; then
		MAPLE_HOME_PATH="${DEFAULT_CHECKING_PATH} unknown"	
	      elif test "$withval" != no ; then			
		MAPLE_HOME_PATH="$withval ${DEFAULT_CHECKING_PATH} unknown"
	     fi	],
	    [])

AC_ARG_ENABLE(shared,
	      [ --enable-shared 	  
				    	   Check for shared compilation (needed by --with-maple)
	      ],
	      [have_shared="$enableval"],
              [have_shared="no"])

if test -n "$MAPLE_HOME_PATH" ; then
min_maple_version=ifelse([$1], ,7.0,$1)
AC_MSG_CHECKING(for MAPLE >= $min_maple_version)
fi

eval LIB_DIR=${libdir}
if test ${LIB_DIR} = "NONE/lib" ; then
	eval LIB_DIR="${prefix}/lib"
fi


for MAPLE_HOME in ${MAPLE_HOME_PATH} 
do
		if test "x$MAPLE_HOME" != "xunknown"; then
			if test -r "${MAPLE_HOME}/bin/maple.system.type" && test -r "${MAPLE_HOME}/extern/include/maplec.h" ; then
				MAPLE_BIN=${MAPLE_HOME}/`${MAPLE_HOME}/bin/maple.system.type`
			else			
				MAPLE_BIN=""								
			fi
		else
			if test -r "/usr/local/bin/xmaple" && test -r "/usr/local/bin/maple.system.type"; then
					MAPLE_HOME=`sed -ne "s/MAPLE='\(.*\)'/\\1/p" /usr/local/bin/xmaple`
					MAPLE_BIN="${MAPLE_HOME}/"`${MAPLE_HOME}/bin/maple.system.type`
			elif test -r "/usr/bin/xmaple" ; then
					MAPLE_HOME=`sed -ne "s/MAPLE='\(.*\)'/\\1/p" /usr/bin/xmaple`
					MAPLE_BIN="${MAPLE_HOME}/"`${MAPLE_HOME}/bin/maple.system.type`
			else
					MAPLE_BIN=""				
			fi
		fi					
	
		if test -z "${MAPLE_BIN}" ; then
			maple_found="no"			
		else
			maple_found="yes"
			if test $have_shared = "yes"; then
				AC_MSG_RESULT(found)	
				LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${MAPLE_BIN}:${LIB_DIR}"
				LD_RUN_PATH="${LD_RUN_PATH}:${MAPLE_BIN}:${LIB_DIR}"
				export LD_LIBRARY_PATH
				export LD_RUN_PATH
				MAPLE_LIBS="-L${MAPLE_BIN} -lmaplec -lstdc++"
				MAPLE_CFLAGS="-I${MAPLE_HOME}/extern/include"				
				cp -f interfaces/maple/lbmaple.mpl.head interfaces/maple/lbmaple.mpl
				echo "\"${LIB_DIR}/liblbmaple.so\"; " >> interfaces/maple/lbmaple.mpl
				cat interfaces/maple/lbmaple.mpl.tail >> interfaces/maple/lbmaple.mpl
       				AC_SUBST(MAPLE_LIBS)
				AC_SUBST(MAPLE_CFLAGS)
				AC_DEFINE(HAVE_MAPLE,1,[Define if MAPLE is installed])
				HAVE_MAPLE=yes
				break
			else			
				AC_MSG_RESULT(problem)
				echo " you need to give option --enable-shared to allow Maple interfacing. Disabling."
				break
			fi
		fi
done

if test "x$maple_found" = "xno" ; then
	AC_MSG_RESULT(not found)
fi

AM_CONDITIONAL(LINBOX_HAVE_MAPLE, test "x$HAVE_MAPLE" = "xyes")


])