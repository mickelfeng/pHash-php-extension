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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_phash.h"
#include <stdlib.h>

/* If you declare any globals in php_phash.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(phash)
*/

/* True global resources - no need for thread safety here */
static int le_phash;

/* {{{ phash_functions[]
 *
 * Every user visible function must have an entry in phash_functions[].
 */
zend_function_entry phash_functions[] = {
	PHP_FE(phash_dct_video_hash, NULL)
	PHP_FE(phash_image_digest, NULL)
	PHP_FE(phash_crosscorr, NULL)
	PHP_FE(phash_compare_images, NULL)
	PHP_FE(phash_dct_image_hash, NULL)
	PHP_FE(phash_hamming_distance, NULL)
	PHP_FE(phash_audio_hash, NULL)
	PHP_FE(phash_audio_distance, NULL)
	PHP_FE(phash_text_hash, NULL)
	{NULL, NULL, NULL}	/* Must be the last line in phash_functions[] */
};
/* }}} */

/* {{{ phash_module_entry
 */
zend_module_entry phash_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"phash",
	phash_functions,
	PHP_MINIT(phash),
	PHP_MSHUTDOWN(phash),
	NULL,
	NULL,
	PHP_MINFO(phash),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PHASH
ZEND_GET_MODULE(phash)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("phash.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_phash_globals, phash_globals)
    STD_PHP_INI_ENTRY("phash.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_phash_globals, phash_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_phash_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_phash_init_globals(zend_phash_globals *phash_globals)
{
	phash_globals->global_value = 0;
	phash_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(phash)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(phash)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(phash)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "phash support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

PHP_FUNCTION(phash_dct_video_hash)
{
	
	zval *filename;
	char return_val[256];
	ulong64 hash;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &filename) == FAILURE)
	{
		RETURN_NULL();
	}
	
	convert_to_string(filename);
	
	if(ph_dct_videohash(Z_STRVAL_P(filename), hash) < 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to compute hash!");
		RETURN_FALSE;	
	}

	sprintf(return_val, "%llu", hash);
	RETURN_STRING(return_val, 1);
	
}

PHP_FUNCTION(phash_image_digest)
{
	zval *filename;
	double sigma = 1.0;
	double gamma = 1.0;
	long angles = 180;
	Digest digest;
	char rval[256];

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ddl", &filename, &sigma, &gamma, &angles) == FAILURE)
	{
		RETURN_NULL();
	}
	
	convert_to_string(filename);
	
	if (ph_image_digest(Z_STRVAL_P(filename), sigma, gamma, digest, angles) < 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to compute image digest");
		RETURN_FALSE;
	}
	
	array_init(return_value);
	for(int i=0; i < digest.size; i++)
	{
		sprintf(rval, "%u", digest.coeffs[i]);
		add_index_string(return_value, i, rval, 1);
	}
}

PHP_FUNCTION(phash_crosscorr)
{
	zval *arr1, *arr2, **data1, **data2;
	HashTable *arr_hash1, *arr_hash2;
	HashPosition pointer1, pointer2;
	int arrayCount1, arrayCount2;
	Digest digest1, digest2;
	double threshold = 0.9;
	double pcc;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa", &arr1, &arr2) == FAILURE)
	{
		RETURN_NULL();
	}
	
	arr_hash1 = Z_ARRVAL_P(arr1);
	arr_hash2 = Z_ARRVAL_P(arr2);
	
	arrayCount1 = zend_hash_num_elements(arr_hash1);
	arrayCount2 = zend_hash_num_elements(arr_hash2);
	
	uint8_t *tmpCoeffs1 = (uint8_t*)malloc(arrayCount1*sizeof(uint8_t));
	uint8_t *tmpCoeffs2 = (uint8_t*)malloc(arrayCount2*sizeof(uint8_t));
	
	digest1.id = NULL;
	digest1.coeffs = tmpCoeffs1;
	digest1.size = arrayCount1;
	
	digest2.id = NULL;
	digest2.coeffs = tmpCoeffs2;
	digest2.size = arrayCount2;

	for(zend_hash_internal_pointer_reset_ex(arr_hash1, &pointer1);
			zend_hash_get_current_data_ex(arr_hash1, (void **)&data1, &pointer1) == SUCCESS;
				zend_hash_move_forward_ex(arr_hash1, &pointer1))
	{
		
		sscanf(Z_STRVAL_PP(data1), "%u", tmpCoeffs1);
		tmpCoeffs1++;
	}
	
	for(zend_hash_internal_pointer_reset_ex(arr_hash2, &pointer2);
			zend_hash_get_current_data_ex(arr_hash2, (void **)&data2, &pointer2) == SUCCESS;
				zend_hash_move_forward_ex(arr_hash2, &pointer2))
	{
		
		sscanf(Z_STRVAL_PP(data2), "%u", tmpCoeffs2);
		tmpCoeffs2++;
	}
	
	ph_crosscorr(digest1, digest2, pcc, threshold);
	RETURN_DOUBLE(pcc);
}

PHP_FUNCTION(phash_compare_images)
{
	zval *filename1, *filename2;
	double sigma = 1.0;
	double gamma = 1.0;
	long angles = 180;
	double threshold = 0.9;
	double pcc;
	Digest digestA, digestB;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|ddld", &filename1, &filename2, &sigma, &gamma, &angles, &threshold) == FAILURE)
	{
		RETURN_NULL();
	}
	
	convert_to_string(filename1);
	convert_to_string(filename2);
	
	if (ph_image_digest(Z_STRVAL_P(filename1), sigma, gamma, digestA, angles) < 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to compute image digest");
		RETURN_FALSE;
	}
	
	if (ph_image_digest(Z_STRVAL_P(filename2), sigma, gamma, digestB, angles) < 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to compute image digest");
		RETURN_FALSE;
	}
	
	if (ph_crosscorr(digestA,digestB,pcc,threshold) < 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to compute peak cross correlation");
		RETURN_FALSE;
	}
	
	if( pcc > threshold)
	{
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}

PHP_FUNCTION(phash_dct_image_hash)
{
	zval *filename;
	char return_val[256];
	ulong64 hash;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &filename) == FAILURE)
	{
		RETURN_NULL();
	}
	
	if( ph_dct_imagehash(Z_STRVAL_P(filename), hash) < 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to compute hash!");
		RETURN_FALSE;
	}
	
	sprintf(return_val, "%llu", hash);
	RETURN_STRING(return_val, 1);
}


PHP_FUNCTION(phash_hamming_distance)
{
	zval *arg1, *arg2;
	ulong64 hash1, hash2;
	int distance = -1;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &arg1, &arg2) == FAILURE)
	{
		RETURN_NULL();
	}
	
	convert_to_string(arg1);
	convert_to_string(arg2);
	sscanf(Z_STRVAL_P(arg1), "%llu", &hash1);
	sscanf(Z_STRVAL_P(arg2), "%llu", &hash2);
	distance = ph_hamming_distance(hash1, hash2);
	RETURN_LONG(distance);
}

PHP_FUNCTION(phash_audio_hash)
{
	zval *filename;
	float *buf;
	int sr = 8000;
	int channels = 1;
	int buf_len;
	uint32_t *hash = NULL;
	int frames;
	int N,i=0;
	char rval[128];	
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &filename, &sr, &channels) == FAILURE)
	{
		RETURN_NULL();
	}
	
	if(sr <= 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "sr value must be positive!");
		RETURN_FALSE;
	}
	
	if(sr <= 0)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "channels value must be positive!");
		RETURN_FALSE;
	}
	
	convert_to_string(filename);
	
	buf = ph_readaudio(Z_STRVAL_P(filename), sr, channels, N);
	if (!buf)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to compute hash!");
		RETURN_FALSE;
	}
	
	hash = ph_audiohash(buf, N, sr, frames);
     if (!hash)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to compute hash!");
		RETURN_FALSE;
	}
	
	delete buf;
	
	array_init(return_value);
	while(i<frames){
		sprintf(rval, "%u", *hash);
		add_index_string(return_value, i, rval, 1);
		hash++;
		i++;
	}
	
}

PHP_FUNCTION(phash_audio_distance)
{
	zval *arr1, *arr2, **data1, **data2;
	HashTable *arr_hash1, *arr_hash2;
	HashPosition pointer1, pointer2;
	int arrayCount1, arrayCount2;
	int i=0;
	float threshold = 0.30;        //ber threshold (0.25-0.35)
	int blockSize = 256; 
	int Nc;
	double maxC = 0.0;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa|dl", &arr1, &arr2, &threshold, &blockSize) == FAILURE)
	{
		RETURN_NULL();
	}
	
	arr_hash1 = Z_ARRVAL_P(arr1);
	arr_hash2 = Z_ARRVAL_P(arr2);
	
	arrayCount1 = zend_hash_num_elements(arr_hash1);
	arrayCount2 = zend_hash_num_elements(arr_hash2);
	
	uint32_t *tmpHash1 = (uint32_t*)malloc(arrayCount1*sizeof(uint32_t));
	uint32_t *tmpHash2 = (uint32_t*)malloc(arrayCount2*sizeof(uint32_t));
	
	uint32_t *pHash1 = tmpHash1;
	uint32_t *pHash2 = tmpHash2;

	for(zend_hash_internal_pointer_reset_ex(arr_hash1, &pointer1);
			zend_hash_get_current_data_ex(arr_hash1, (void **)&data1, &pointer1) == SUCCESS;
				zend_hash_move_forward_ex(arr_hash1, &pointer1))
	{
		
		sscanf(Z_STRVAL_PP(data1), "%u", tmpHash1);
		tmpHash1++;
	}
	
	for(zend_hash_internal_pointer_reset_ex(arr_hash2, &pointer2);
			zend_hash_get_current_data_ex(arr_hash2, (void **)&data2, &pointer2) == SUCCESS;
				zend_hash_move_forward_ex(arr_hash2, &pointer2))
	{
		
		sscanf(Z_STRVAL_PP(data2), "%u", tmpHash2);
		tmpHash2++;
	}
	
	double *pC = ph_audio_distance_ber(pHash1, arrayCount1, pHash2, arrayCount2, threshold, blockSize, Nc);
	
	
	maxC = 0.0;
	for (int j=0;j<Nc;j++){
	    if (pC[j] > maxC){
		maxC = pC[j];
	    }
	}
	
	delete pHash1;
	delete pHash2;
	
	RETURN_DOUBLE(maxC);
}

PHP_FUNCTION(phash_text_hash)
{
	zval *filename;
	int nbpoints = 0;
	zval *subarr;
	char hash[256], index[256];
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &filename) == FAILURE)
	{
		RETURN_NULL();
	}
	
	convert_to_string(filename);
	TxtHashPoint *txtHash = ph_texthash(Z_STRVAL_P(filename), &nbpoints);
	if(!txtHash)
	{
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to compute hash!");
		RETURN_FALSE;
	}
	
	
	
	int j=0;
		
	for(j=0; j < nbpoints; j++){
		php_printf("Hash %llu index %jd \n", txtHash[j].hash, (intmax_t)txtHash[j].index);
	}
	
	array_init(return_value);
	for(int i=0; i < nbpoints; i++){
		ALLOC_INIT_ZVAL(subarr);
		array_init(subarr);
		sprintf(hash, "%llu", txtHash[i].hash);
		sprintf(index, "%jd", (intmax_t)txtHash[i].index);
		add_index_string(subarr, 0, hash, 1);
		add_index_string(subarr, 1, index, 1);
		add_index_zval(return_value, i, subarr);
	}
}
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
