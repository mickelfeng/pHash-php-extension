dnl $Id$
dnl config.m4 for extension phash

PHP_ARG_WITH(phash, for phash support,
[  --with-phash[=DIR]		  Include phash support])

PHP_ARG_WITH(cimg,	CImg installation directory,
[  --with-cimg[=DIR]		  Define CImg location])

if test "$PHP_PHASH" != "no"; then

	AC_DEFINE(HAVE_PHASH, 1, [Whether you have phash support]) 
	PHP_REQUIRE_CXX()
	PHP_SUBST(PHASH_SHARED_LIBADD)
	PHP_ADD_LIBRARY_WITH_PATH(stdc++, "", PHASH_SHARED_LIBADD)
	
	if test -r $PHP_CIMG/include/CImg.h; then
		CIMG_DIR=$PHP_CIMG
	else
		AC_MSG_CHECKING([for CImg installation])
		for i in /usr/include /usr/local/include; do
			if test -r $i/CImg.h; then
				CIMG_DIR=$i
				AC_MSG_RESULT(found in $i)
				break
			fi
		done
	fi
	
	if test -z "$CIMG_DIR"; then
		AC_MSG_RESULT(not found)
		AC_MSG_ERROR(Please install the CImg library)
	fi
	
	AC_MSG_CHECKING([for audiophash.h existence])
	for i in /usr/include /usr/local/include; do
		if test -r $i/audiophash.h; then
			AUDIOPHASH_HEADER_DIR=$i
			AC_MSG_RESULT(found in $i)
			break
		fi
	done
	
	if test -z "$AUDIOPHASH_HEADER_DIR"; then
		AC_MSG_RESULT(not found)
		AC_MSG_ERROR(Please install audiophash.h to the system include directory)
	fi
	
	
	PHP_ADD_LIBRARY_WITH_PATH(pHash, "", PHASH_SHARED_LIBADD)
	dnl PHP_ADD_LIBRARY_WITH_PATH(avformat, "", PHASH_SHARED_LIBADD)
	dnl PHP_ADD_LIBRARY_WITH_PATH(swscale, "", PHASH_SHARED_LIBADD)..
 
	SEARCH_PATH="/usr/local /usr"    
	SEARCH_FOR="/include/pHash.h"  
	if test -r $PHP_PHASH/$SEARCH_FOR; then
		PHASH_DIR=$PHP_PHASH
	else
		AC_MSG_CHECKING([for phash files in default path])
		for i in $SEARCH_PATH ; do
			if test -r $i/$SEARCH_FOR; then
				PHASH_DIR=$i
				AC_MSG_RESULT(found in $i)
			fi
		done
	fi

	if test -z "$PHASH_DIR"; then
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([Please reinstall the pHash distribution])
	fi

	PHP_ADD_INCLUDE($PHASH_DIR/include)

	PHP_CHECK_LIBRARY(
		pHash,
		ph_about,
	 	[
	 		PHP_ADD_LIBRARY_WITH_PATH(pHash, $PHASH_DIR/lib, PHASH_SHARED_LIBADD)
	 		AC_DEFINE(HAVE_PHASHLIB,1,[ ])
	 	],[
	 		AC_MSG_ERROR([wrong pHash lib version or lib not found])
	 	],[
	 	]
	)
  
	PHP_SUBST(PHASH_SHARED_LIBADD)

	PHP_NEW_EXTENSION(phash, phash.cpp, $ext_shared)
fi
