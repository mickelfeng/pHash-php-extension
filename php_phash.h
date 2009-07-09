/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifndef PHP_PHASH_H
#define PHP_PHASH_H

extern zend_module_entry phash_module_entry;
#define phpext_phash_ptr &phash_module_entry

#include "CImg.h"
#include "audiophash.h"
#include "pHash.h"

#ifdef PHP_WIN32
#define PHP_PHASH_API __declspec(dllexport)
#else
#define PHP_PHASH_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(phash);
PHP_MSHUTDOWN_FUNCTION(phash);
PHP_RINIT_FUNCTION(phash);
PHP_RSHUTDOWN_FUNCTION(phash);
PHP_MINFO_FUNCTION(phash);

PHP_FUNCTION(phash_dct_video_hash);	/* DCT hash for videos */
PHP_FUNCTION(phash_image_digest);		/* Radial hash for images */
PHP_FUNCTION(phash_crosscorr);		/* Cross correlation of images */
PHP_FUNCTION(phash_compare_images);	/* Compare images using radial hash */
PHP_FUNCTION(phash_dct_image_hash);	/* DCT hash for images */
PHP_FUNCTION(phash_hamming_distance);	/* Hamming distance (videos & images) */
PHP_FUNCTION(phash_audio_hash);		/* Hash for audio files */
PHP_FUNCTION(phash_audio_distance);	/* Hash distance for audio files */
PHP_FUNCTION(phash_text_hash);		/* Text hash */
PHP_FUNCTION(confirm_phash_compiled);	/* For testing, remove later. */

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(phash)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(phash)
*/

/* In every utility function you add that needs to use variables 
   in php_phash_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as PHASH_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define PHASH_G(v) TSRMG(phash_globals_id, zend_phash_globals *, v)
#else
#define PHASH_G(v) (phash_globals.v)
#endif

#endif	/* PHP_PHASH_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
