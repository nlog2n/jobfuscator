#ifndef __CACHE_H__
#define __CACHE_H__


#include "vkernel.h"

VOID Cc_ClearFileCache( 	
    __in PFILE_OBJECT FileObject, 
    __in BOOLEAN bIsFlushCache,
	__in PLARGE_INTEGER FileOffset, 
	__in ULONG Length ) ;


#endif   // __CACHE_H__