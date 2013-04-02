#
#   mod-xslt -- Copyright (C) 2002-2008 
# 		 Carlo Contavalli 
# 		 <ccontavalli at inscatolati.net>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

# Process this file with helpers/release to produce a configure script.
AC_PREREQ([2.68])
AC_COPYRIGHT(
  [Copyright (C) 2003-2011 Carlo Contavalli <ccontavali at masobit.net>]
  [mod-xslt and this configure script are free software; you can ]
  [redistribute them and/or modify them under the terms of the GNU ]
  [General Public License. Read the provided documentation to know ]
  [more about this license and usage terms.])
AC_INIT([mod-xslt],[1.4.2],[ccontavalli at inscatolati.net],[mod-xslt])
AC_LANG(C)

# Libtool versioning
LIB_CURRENT=1
LIB_REVISION=0
LIB_AGE=0
LIB_VERSION=$LIB_CURRENT

# Check we really are using the correct directory
AC_CONFIG_SRCDIR(lib/modxslt-doc.c)
AC_CONFIG_HEADER(lib/modxslt1/modxslt-system.h)
AC_CONFIG_AUX_DIR([helpers])

# Check for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_CACHE_SAVE

LT_INIT
LT_OUTPUT
AC_SUBST(LIBTOOL_DEPS)
AC_CACHE_SAVE

# Check for basic functions
AC_CHECK_FUNCS([malloc memcmp memset realloc stat vprintf memset strdup],, [ 
		AC_MSG_ERROR([Necessary function is missing. Can't continue])])
AC_FUNC_MMAP
AC_FUNC_MEMCMP
AX_CREATE_STDINT_H(lib/modxslt1/modxslt-integers.h)

# Check for C compiler/library features
AC_SEARCH_LIBS([strerror],[cposix])
AC_C_CONST
AC_C_INLINE
AC_C_PROTOTYPES
AC_TYPE_SIZE_T
AC_CACHE_SAVE

# Check for standard headers
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h fcntl.h string.h unistd.h],, AC_MSG_ERROR([Missing needed header file ]))
AC_CHECK_HEADERS([syslog.h])

# Check if debugging is required
AC_ARG_ENABLE(xslt-debug, AS_HELP_STRING([[--enable-xslt-debug]],[Enable libxslt debugging]),
	   [AC_DEFINE(MXSLT_LIBXSLT_DEBUG,, [Enable libxslt debugging]) AC_SUBST(EN_XSLT_DEBUG, "enabled")],
	   [AC_SUBST(EN_XSLT_DEBUG, "disabled")])
AC_ARG_ENABLE(debug, AS_HELP_STRING([[--enable-debug]],[Compile code with gdb support without optimizations]), 
	      [ 
	        DEFAULT_TARGET="debug"; 
              ], 
	      [ 
                AC_SUBST(LIBTOOLQUIET, "--quiet")
	        DEFAULT_TARGET="all"; 
              ])
AC_ARG_ENABLE(verbose, AS_HELP_STRING([[--enable-verbose]],[Tell libtool to be verbose when running commands]), 
	      [ 
                AC_SUBST(LIBTOOLQUIET, "")
              ], 
	      [ 
                AC_SUBST(LIBTOOLQUIET, "--quiet")
              ])
AC_ARG_ENABLE(memory, AS_HELP_STRING([[--enable-memory]],[Place sentinels and perform strict checks upon memory allocations/free]), 
	      [ 
		AC_DEFINE(MXSLT_DEBUG_MEMORY,, [Enable strict memory checks])
              ])


AC_ARG_ENABLE(thread, AS_HELP_STRING([[--disable-thread]],[Disable threading support]), 
	      [PROBE_THREADS="no"; AC_SUBST(EN_THREAD, "disabled")],
	      [PROBE_THREADS="yes"; AC_SUBST(EN_THREAD, "enabled") AC_DEFINE(MXSLT_THREAD,, [Enable multithreading support])])
AC_ARG_ENABLE(extensions, AS_HELP_STRING([[--disable-extensions]],[Disable compilation of xslt extensions]),
	   [AC_DEFINE(MXSLT_DISABLE_EXTENSIONS,, [Disable compilation of xslt extensions]) 
	    AC_SUBST(EN_EXTENSIONS, "disabled")],
	   [AC_SUBST(EN_EXTENSIONS, "enabled")])
AC_ARG_ENABLE(xinclude, AS_HELP_STRING([[--disable-xinclude]],[Disable modxslt usage of xinclude]),
	   [AC_DEFINE(MXSLT_DISABLE_XINCLUDE,, [Disable modxslt usage of xinclude])
	    AC_SUBST(EN_XINCLUDE, "disabled")],
	   [AC_SUBST(EN_XINCLUDE, "enabled")])
AC_ARG_ENABLE(libxslt-hack, AS_HELP_STRING([[--enable-libxslt-hack]],[Enable the xsltDebugError wrapper to avoid SEGFAULTS due to a libxslt bug (read the FAQ)]),
	   [AC_DEFINE(MXSLT_LIBXSLT_HACK,, 
	     [Enable compilation of a wrapper for xsltDebugError to work around a libxslt bug])
	    AC_SUBST(EN_LIBXSLT_HACK, "enabled")
	   ], [ AC_SUBST(EN_LIBXSLT_HACK, "disabled") ])
AC_ARG_ENABLE(fallback-wraparound, AS_HELP_STRING([[--enable-fallback-wraparound]],[Stop the ``fallback'' error to be shown in the web servers log]),
	   [AC_DEFINE(MXSLT_FALLBACK_WRAPAROUND,, 
	     [Avoid the error ``fallback was not compiled'' to be shown in the web servers log])
	     AC_SUBST(EN_FALLBACK_WRAPAROUND, "enabled")
	   ], [AC_SUBST(EN_FALLBACK_WRAPAROUND, "disabled")])

# Check for needed wrappers 
#   - if command line is not specified, then the path value is taken
AC_ARG_WITH(sapi, AS_HELP_STRING([[--with-sapi=(apache1|apache2|none)]],[Web server interface to be used]), 
	    [ SAPI="$withval" ], [ SAPI="detect" ])
AC_ARG_WITH(xml2-config, AS_HELP_STRING([[--with-xml2-config=FILE]],[Path to xml2-config program]), 
	    [ XML2="$withval" ], [ AC_PATH_PROG(XML2, xml2-config, [], [$PATH:/usr/sbin/]) ])
AC_ARG_WITH(xslt-config, AS_HELP_STRING([[--with-xslt-config=FILE]],[Path to xslt-config program]), 
	    [ XSLT="$withval" ], [ AC_PATH_PROG(XSLT, xslt-config, [], [$PATH:/usr/sbin/]) ])
AC_ARG_WITH(pcre-config, AS_HELP_STRING([[--with-pcre-config=FILE]],[Path to pcre-config program]), 
	    [ PCRE="$withval" ], [ AC_PATH_PROG(PCRE, pcre-config, [], [$PATH:/usr/sbin/]) ])

# Check if we found the wrapper 
AC_DEFINED(XML2,,[AC_MSG_ERROR([Couldn't find 'xml2-config' in PATH. Please specify '--with-xml2-config' to configure. ])])
AC_DEFINED(XSLT,,[AC_MSG_ERROR([Couldn't find 'xslt-config' in PATH. Please specify '--with-xslt-config' to configure. ])])
AC_DEFINED(PCRE,,[AC_MSG_ERROR([Couldn't find 'pcre-config' in PATH. Please specify '--with-pcre-config' to configure. ])])

# Check if the wrapper we found is usable 
AC_ISCMD(XML2, [ --version ],, [ 
	AC_MSG_ERROR(['$XML2' is not executable or is not libxml2 xml2-config. Please use '--with-xml2-config'. ]) ])
AC_ISCMD(XSLT, [ --version ],, [
	AC_MSG_ERROR(['$XSLT' is not executable or is not libxslt xslt-config. Please use '--with-xslt-config'. ]) ])
AC_ISCMD(PCRE, [ --version ],, [
	AC_MSG_ERROR(['$PCRE' is not executable or is not libpcre pcre-config. Please use '--with-pcre-config'. ]) ])

# What a shame !@!! gcc-2.95 does not 
# like spaces between -l and the name of the library!
AC_CHECK_LIB([xml2], [xmlParseMemory], [ XML2F='yes' ], AC_MSG_ERROR([Couldn't find libxml2 ]), [ `$XML2 --libs` ])
AC_CHECK_LIB([xslt], [xsltApplyStylesheet], [ XSLTF='yes' ], AC_MSG_ERROR([Couldn't find libxslt ]), [ `$XSLT --libs` ])
AC_CHECK_LIB([pcre], [pcre_compile], [ PCREF='yes' ], AC_MSG_WARN([Going on without libpcre ]), [ `$PCRE --libs` ])

# Check for needed functions in libxslt
AC_CHECK_LIB([xslt], [xsltCopyTextString], [ XSLTF='yes' ], [AC_MSG_ERROR([]
  [*******************************]
  [* libxslt does not provide the function]
  [* 'xsltCopyTextString' needed by modxslt.]
  [* You should make sure to have a version of]
  [* libxslt >= 1.0.32 or to apply the provided]
  [* patch:]
  [*    patches/libxslt.before-1.0.32.diff]
  [* to any earlyer version.]
  [* Anyway, you can obtain the version of the]
  [* libxslt detected on your system by running: ]
  [*    $XSLT --version ])], [ `$XSLT --libs` ])

if test "$XML2F" = "yes"; then
  CPPFLAGS="`$XML2 --cflags` $CPPFLAGS";

    # Check for libxml2 headers
  AC_CHECK_HEADERS([libxml/uri.h libxml/tree.h libxml/parserInternals.h libxml/nanohttp.h],,
    		  AC_MSG_ERROR([Some or all required header files for libxml2 are missing. ]))

    # Check if we have threading enabled 
  AC_CHECK_LIB([xml2], [xmlGetGlobalState], [ 
          LIBS="$LIBS"  
	  AC_DEFINE(HAVE_LIBXML_THREADS,, [A version of libxml2 with multithreading support is being used])
	], [ 
  	  AC_MSG_WARN([ ] 
[******************************************************]
[* Your libxml2 was not compiled with multithreading support. ]
[* Please make sure to read the `README` _carefully_.]
[* Compilation will go on anyway - but don't expect me to]
[* believe your bug reports.]
[******************************************************]);
	], [ `$XML2 --libs` ])

    
    # Check to see if we exported private variables to tweak
    # on the http client code
  AC_CHECK_TYPE([xmlNanoHTTPCtxt], [
  	  AC_DEFINE(MXSLT_LIBXML_HTTP_STATUS,, [Define this if xmlNanoHTTPCtxt is available from libxml2 headers])
	], [
	  AC_MSG_WARN([]
[******************************************************]
[* Your libxml2 does not let applications access xmlNanoHTTPCtxt. ]
[* This is not bad by itself, but any 404 or 403 or any error page ]
[* returned by a remote host as a response to the request of a ]
[* stylesheet or a dtd will be parsed as xml, leading to weird ]
[* results. If you can live with it, ignore this warning. ]
[* Ignore this warning also if you patched your library with]
[*     patches/libxml.httpstatus.diff]
[* which cannot be easily detected. In any other case, you can]
[* apply the patch:]
[*     patches/libxml.httpcontextexport.diff]
[* If you need further help, please read the doc/patches/patches.txt]
[* Compilation will go on anyway without problems. ]
[******************************************************])
	 ], [#include <libxml/nanohttp.h>])

    # Check if libxml2 patch has been applyed
  AC_CHECK_LIB([xml2], [xmlSetGlobalState], [ 
          LIBS="$LIBS" 
          AC_DEFINE(HAVE_LIBXML_HACK,, [A version of libxml2 that does support setGlobalState is being used])
	], [
  	  AC_MSG_WARN([ ] 
[******************************************************]
[* Your libxml2 does not provide a setGlobalState. ]
[* You probably haven't patched it. Please read the manual, doc/manual/manual.txt.]
[* Compilation will go on anyway - I will compile a small compatibility layer.]
[******************************************************])
	 ], [ `$XML2 --libs` ])
fi

  # Add libxslt parameters
if test "$XSLTF" = "yes"; then
  CPPFLAGS="$CPPFLAGS `$XSLT --cflags`"

    # Check for libxslt headers
  AC_CHECK_HEADERS([libxslt/xslt.h libxslt/xsltInternals.h libxslt/transform.h libxslt/xsltutils.h],,
  		  [ AC_MSG_ERROR([Some or all header files for libxslt are missing ]) ])

  LIBS="$LIBS `$XSLT --libs`"

    # Check if exslt was asked
  AC_ARG_ENABLE([exslt], AS_HELP_STRING([[--disable-exslt]],[Disable exslt extensions for libxslt. Default enabled ]),
	    [ EXSLT="no" ], [ EXSLT="yes" ])


  if test "$EXSLT" = "yes"; then
    AC_CHECK_HEADERS([libexslt/exslt.h],, [ AC_MSG_ERROR([Could not find headers for exslt library ]) ])
    AC_CHECK_LIB([exslt], [exsltLibraryVersion], [ LIBS="$LIBS" ], [ AC_MSG_ERROR([Could not link against libexslt ]) ])

    AC_DEFINE(USE_EXSLT,,[libexslt is installed on the system and you want mod-xslt to use it])

    #CPPFLAGS="$CPPFLAGS"
    LIBS="$LIBS -lexslt"
  fi
fi

  # Add libpcre parameters
if test "$PCREF" = "yes"; then
  TMPFLAGS="$CPPFLAGS"
  CPPFLAGS="`$PCRE --cflags`"

    # Check for libpcre headers
  AC_CHECK_HEADER([pcre.h], [ CPPFLAGS="$TMPFLAGS $CPPFLAGS"; LIBS="$LIBS `$PCRE --libs`"; 
  			      AC_DEFINE(HAS_LIBPCRE,,[libpcre is installed on the system and is usable]) ], 
  			    [ CPPFLAGS="$TMPFLAGS"; AC_MSG_WARN([Going on without libpcre ]) ])
fi

if test "x$SAPI" = "xdetect"; then
  AC_MSG_NOTICE([* Detecting SAPI (Server API) to use])
fi
AC_CACHE_SAVE

# Before testing the api, make sure a pointer is 
# at least 32 bits long (we rely on it in apache1/2 sapi 
# to store an ip address in place of a void * pointer in
# an hashing table
AS_MESSAGE([checking if pointers (void *) are at least 32 bits long...]);
AC_RUN_IFELSE([AC_LANG_PROGRAM(, [[ if(sizeof(void *) < 4) exit(1); exit(0); ]])],,
	      [AC_MSG_ERROR([For mod-xslt to be compiled, pointers (void *) must be at least 32 bits long]
	      		    [If you really need to compile mod-xslt on this architecture, please contact]
			    [one of the authors. Support was "dropped" for lazyness.])])

## Setup SAPI specific variables - a simple finite state automa  
APXS1="apxs /usr/sbin/apxs /usr/bin/apxs /usr/local/bin/apxs /usr/local/apache/bin/apxs"
APXS2="apxs2 /usr/sbin/apxs2 /usr/bin/apxs2 /usr/local/bin/apxs2 /usr/local/apache/bin/apxs2"

true_var=:
sapi_previous="none"
while $true_var; do
  AC_MSG_NOTICE([* Trying with: '$SAPI'])

  case "$SAPI" in
    none)
       break;
      ;;

    detect)
      sapi_detected="true"

      case "$sapi_previous" in
        none)
          SAPI="apache";
	 ;;

        apache*)	
	  SAPI="error";
	 ;;
      esac

      sapi_previous="detect"
     ;;

    apache)
       sapi_previous="apache"
       sapi_apache_detected="true"

         # Get apxs path
       AC_APACHE_APXS([$APXS1 $APXS2], 
       			[AC_MSG_ERROR([apache: couldn't find 'apxs' in PATH. Please use '--with-apxs' to configure.])],
     		        [AC_MSG_ERROR([apache: couldn't find apxs '$APXS', or apxs is not executable or is not Apache apxs. Please use '--with-apxs'.])] )

	 # If ap_release.h exists in the path specified by apxs,
	 # this is apache2... otherwise, this is apache1
       APXS_DIR=`$APXS -q INCLUDEDIR`	
       AC_CHECK_HEADER([$APXS_DIR/ap_release.h], 
           [SAPI='apache2'; AC_MSG_NOTICE([apache: Apache 2.0.x was detected thanks to apxs])],
       	   [SAPI='apache1'; AC_MSG_NOTICE([apache: Apache 1.3.x was detected thanks to apxs])])

     ;;

    apache1)
         # Get apxs path
       AC_APACHE_APXS([$APXS1], [AC_MSG_ERROR([apache1: couldn't find 'apxs' in PATH. Please use '--with-apxs' to configure.])],
     		        [AC_MSG_ERROR([apache1: '$APXS', or apxs is not executable or is not Apache apxs. Please use '--with-apxs'.])] )

# Verify we have ioctl available
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
  #include <stdio.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/ioctl.h>
  #include <net/if.h>
  #include <netinet/in.h>
]], [[
  struct ifconf ifc;
  int fd, status, i;
  struct ifreq * ifrp, ifr;
  char * str; 

    /* Zero up structure */
  memset(&ifc, 0, sizeof(struct ifconf));
  memset(&ifr, 0, sizeof(struct ifreq));

    /* open socket */
  fd=socket(AF_INET, SOCK_DGRAM, 0);
  if(fd < 0)
    exit(1);

    /* Perform ioctl */
  status=ioctl(fd, SIOCGIFCONF, (char *)&ifc);
  if(status < 0)
    exit(1);

    /* Allocate buffer */
  ifc.ifc_buf=(char *)malloc(ifc.ifc_len+1);

    /* Perform ioctl */
  status=ioctl(fd, SIOCGIFCONF, (char *)&ifc);
  if(status < 0)
    exit(1);

    /* For each interface we found */
  for(ifrp=(struct ifreq *)(ifc.ifc_buf); (char *)ifrp < (((char *)ifc.ifc_buf)+ifc.ifc_len); ifrp++) {
    strcpy(ifr.ifr_name, ifrp->ifr_name);
    ifr.ifr_addr.sa_family=AF_INET;
    status=ioctl(fd, SIOCGIFADDR, &ifr);
    if(status < 0)
      exit(1);

    str=inet_ntoa((struct in_addr)(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));
  }

  exit(0);
]])],[AC_DEFINE(HAVE_NETWORK_IOCTLS,,[enable this if SIOCGIFADDR is available])],[AC_MSG_WARN([]
[*************************]
[* Cannot use SIOCGIFADDR ioctl]
[* If you know how to get the ip addresses]
[* your system is listening on, please contact the]
[* author - some sapi won't be able to detect]
[* loops or protect you against some attacks]
[* please read the manual, doc/manual/manual.txt,]
[* to know about this issue]
[************************])])

	 # Check for apache 1 headers
       APXS_DIR=`$APXS -q INCLUDEDIR`	
       AC_CHECK_HEADER([$APXS_DIR/httpd.h],, [
          AC_SAPI_STEP(apache1, [apache2 error], [couldn't find a valid 'httpd.h'.], [sapi_apache_detected]) ])

	 # Next step is apache-force
       sapi_previous='apache1'

         # check for the compiler being used
       SAPI_CC=`$APXS -q CC`
       
         # If compiler is different than the one we're using
       if test "x$CC" !=  "x$SAPI_CC"; then
         AC_MSG_WARN([]
[* Apache has been compiled with a different compiler than the specified (or detected) one!]
[* I will use the same compiler as used by apache: '$SAPI_CC']
[* I'm also assuming '$SAPI_CC' is ISO C and POSIX compliant.]);
       fi

         # Setup the linker 
         # apxs for apache1 returns a valid linker, while apxs
         # for apache2 returns '', so be careful
       SAPI_LD="`$APXS -q LD_SHLIB`"
       SAPI_LDFLAGS=`$APXS -q LDFLAGS_SHLIB`
     
       SAPI_CPPFLAGS="$CPPFLAGS -I`$APXS -q INCLUDEDIR`"
       SAPI_CFLAGS="`$APXS -q CFLAGS` `$APXS -q CFLAGS_SHLIB`"
       SAPI_LIBS="`$APXS -q LIBS_SHLIB`"

       APXS_INSTALLDIR="`$APXS -q LIBEXECDIR`"

       AC_SUBST(APXS_INSTALLDIR)
       AC_SUBST(APXS)
       break;
      ;;

    apache2)
	 # We _need_ multithreading
       PROBE_THREADS="yes"

         # Get apxs path
       AC_APACHE_APXS([$APXS2 $APXS1], 
       		      [AC_MSG_ERROR([apache2: couldn't find 'apxs' in PATH. Please use '--with-apxs' to configure.])],
     		      [AC_MSG_ERROR([apache2: '$APXS' is not executable or is not Apache apxs. Please use '--with-apxs'.])] )


         # Compiler is checked before libtool is configured
       SAPI_CC=`$APXS -q CC`
       
         # If compiler is different than the one we're using
       if test "x$CC" !=  "x$SAPI_CC"; then
         AC_MSG_WARN([]
[* Apache has been compiled with a different compiler than the specified (or detected) one!]
[* I will use the same compiler as used by apache: '$SAPI_CC']
[* I'm also assuming '$SAPI_CC' is ISO C and POSIX compliant.]);
       fi
      
         # Look for apr_config
       AC_ARG_WITH(apr-config, AS_HELP_STRING([[--with-apr-config=FILE]],[Path to apr-config program]), 
	    [ APRC="$withval" ], [ 
	      AC_PATH_PROG(APRC, apr-config, [], [$PATH:/usr/sbin/:/usr/local/apache2/bin]) 
	      AC_DEFINED(APRC,,[
	        AC_PATH_PROG(APRC, [apr-1-config], [], [$PATH:/usr/sbin/:/usr/local/apache2/bin]) 
	      ])
	    ])

         # Check if we found it
       AC_DEFINED(APRC,,[
         AC_SAPI_STEP(apache2, [apache1 error], [couldn't find a valid 'apr-config'. Please use --with-apr-config.], 
	 	      [sapi_apache_detected])
         ])

         # Verify we can run apr-config
       AC_ISCMD(APRC, [--version],,[ 
         AC_SAPI_STEP(apache2, [apache1 detect], [found 'apr-config' is not executable.], [sapi_apache_detected])
	 ])

          # Check if we can link with libapr 
       APR_LIB_NAME=`$APRC --apr-la-file | sed -e s/.*lib// -e s/\.la//`
       AC_CHECK_LIB([$APR_LIB_NAME], [apr_palloc], [APRCF='yes'],[
         AC_SAPI_STEP(apache2, [apache1 detect], ["$APR_LIB_NAME" library found is not valid.], [sapi_apache_detected])
         ], [`$APRC --link-ld` `$APRC --libs`])

         # Remember to provide aprc to compilation process
       AC_SUBST(APRC)

         # Look for apu_config
       AC_ARG_WITH(apu-config, AS_HELP_STRING([[--with-apu-config=FILE]],[Path to apu-config program]), 
	    [ APUC="$withval" ], [ 
	      AC_PATH_PROG(APUC, apu-config, [], [$PATH:/usr/sbin/:/usr/local/apache2/bin]) 
	      AC_DEFINED(APUC,,[
	        AC_PATH_PROG(APUC, [apu-1-config], [], [$PATH:/usr/sbin/:/usr/local/apache2/bin]) 
	      ])
	    ])

         # Check if we found it
       AC_DEFINED(APUC,,[
         AC_SAPI_STEP(apache2, [apache1 error], [couldn't find a valid 'apu-config'. Please use --with-apu-config.], 
	 	      [sapi_apache_detected])
         ])

         # Verify we can run apr-config
       AC_ISCMD(APUC, [--version],,[ 
         AC_SAPI_STEP(apache2, [apache1 detect], ['apu-config' found is not executable.], [sapi_apache_detected])
	 ])

          # Check if we can link with libapr 
       APU_LIB_NAME=`$APUC --apu-la-file | sed -e s/.*lib// -e s/\.la//`
       AC_CHECK_LIB([$APU_LIB_NAME], [apr_brigade_create], [APRUF='yes'],[
         AC_SAPI_STEP(apache2, [apache1 detect], ["$APU_LIB_NAME" library found is not valid.], [sapi_apache_detected])
         ], [`$APUC --link-ld` `$APUC --libs` `$APRC --link-ld`])

         # Remember to provide aprc to compilation process
       AC_SUBST(APUC)

         # Ok, setup for apache 2 compilation variables
       SAPI_CPPFLAGS="$CPPFLAGS `$APRC --cppflags` `$APRC --includes` `$APUC --includes`"
       SAPI_CFLAGS="$SAPI_CFLAGS `$APRC --cflags`"

         # Ok, try to get the linker
       SAPI_LDFLAGS="$SAPI_LDFLAGS `$APRC --ldflags` `$APRC --libs` `$APRC --link-ld` `$APXS -q LDFLAGS_SHLIB`"
       SAPI_LDFLAGS="$SAPI_LDFLAGS `$APUC --ldflags` `$APUC --libs` `$APUC --link-ld`"

       SAPI_CPPFLAGS="$SAPI_CPPFLAGS -I`$APXS -q INCLUDEDIR`"
       SAPI_CFLAGS="$SAPI_CFLAGS `$APXS -q CFLAGS` `$APXS -q CFLAGS_SHLIB`"
       SAPI_LIBS="`$APXS -q LIBS_SHLIB` `$APRC --link-libtool` `$APUC --link-libtool`"

       APXS_INSTALLDIR="`$APXS -q LIBEXECDIR`"

       AC_SUBST(APXS_INSTALLDIR)
       AC_SUBST(APXS)

       sapi_previous="apache2"
       break;
      ;;

  error)
     AC_MSG_ERROR([Couldn't find an usable SAPI...])
    ;;

  *) # By Default, we don't know what to use
     AC_MSG_ERROR(['$SAPI': is not known. I don't know what to compile to work with the specified web server. ])
    ;;
  esac
done

AC_MSG_WARN([]
[******************************************** ]
[* Generating Makefiles and headers for      ]
[* mod-xslt to be used with:                ]
[*             * $sapi_previous *              ]
[******************************************** ]);

echo "#define MXSLT_VERSION \"$PACKAGE_VERSION\"" \
		> $srcdir/lib/modxslt1/modxslt-version.h
echo "#define MXSLT_MAJOR \"`echo $PACKAGE_VERSION|sed -e 's/\..*//'`\"" \
		>> $srcdir/lib/modxslt1/modxslt-version.h
echo "#define MXSLT_MINOR \"`echo $PACKAGE_VERSION|sed -e 's/[^\.]*\.\([^\.]*\).*/\1/'`\""  \
		>> $srcdir/lib/modxslt1/modxslt-version.h
echo "#define MXSLT_PATCHLEVEL \"`echo $PACKAGE_VERSION|sed -e 's/[^\.]*\.[^\.]*\.//'`\"" \
		>> $srcdir/lib/modxslt1/modxslt-version.h

  # Probe for multithreading support 
if test "$PROBE_THREADS" = "yes"; then
  AC_CHECK_HEADER(pthread.h,,[AC_MSG_ERROR([could not find pthread.h -- needed for threading support])])
  AC_CHECK_LIB(pthread, pthread_setspecific, [
    LIBS="$LIBS -lpthread"
    CFLAGS="$CFLAGS -D_REENTRANT"
    AC_DEFINE(HAVE_PTHREADS,,[posix threading interface (pthread) is available])
  ])
fi

AC_SUBST(XML2)
AC_SUBST(XSLT)
AC_SUBST(PCRE)

AC_SUBST(SAPI_CC)
AC_SUBST(SAPI_CFLAGS)
AC_SUBST(SAPI_CPPFLAGS)
AC_SUBST(SAPI_LD)
AC_SUBST(SAPI_LDFLAGS)
AC_SUBST(SAPI_LIBS)

AC_SUBST(CC)
AC_SUBST(LD)
AC_SUBST(LIBS)
AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(DEFAULT_TARGET)
AC_SUBST(SAPI)

AC_SUBST(LIB_VERSION)
AC_SUBST(LIB_CURRENT)
AC_SUBST(LIB_REVISION)
AC_SUBST(LIB_AGE)


AC_CONFIG_FILES([Makefile utils/Makefile lib/Makefile lib/parser/Makefile
                 sapi/apache1/Makefile sapi/apache2/Makefile sapi/none/Makefile
                 utils/modxslt-config.pre doc/man/Makefile])

AC_OUTPUT
