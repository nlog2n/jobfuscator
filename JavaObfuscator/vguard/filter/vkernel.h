#ifndef __VKERNEL_H__
#define __VKERNEL_H__

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")


/************************************************************************* Pool Tags**********/
#define BUFFER_SWAP_TAG     'bdBS'
#define CONTEXT_TAG         'xcBS'
#define NAME_TAG            'mnBS'
#define PRE_2_POST_TAG      'ppBS'


/************************************************************************* Local structures****/
//  Volume context, one of these are attached to each volume we monitor, to get a "DOS" name for debug display.
typedef struct _VOLUME_CONTEXT {
    UNICODE_STRING Name; //  Holds the name to display
    ULONG SectorSize; //  Holds the sector size for this volume.
} VOLUME_CONTEXT, *PVOLUME_CONTEXT;

#define MIN_SECTOR_SIZE 0x200

//  Context structure used to pass state from pre-operation to post-operation callback.
typedef struct _PRE_2_POST_CONTEXT {
    //  Pointer to our volume context structure.  We always get the context
    //  in the preOperation path because you can not safely get it at DPC
    //  level.  We then release it in the postOperation path.  It is safe
    //  to release contexts at DPC level.
    PVOLUME_CONTEXT VolCtx;

    //  Since the post-operation parameters always receive the "original"
    //  parameters passed to the operation, we need to pass our new destination
    //  buffer to our post operation routine so we can free it.
    PVOID SwappedBuffer;
} PRE_2_POST_CONTEXT, *PPRE_2_POST_CONTEXT;



/********************************************************* Registration structure *****************/
extern const FLT_REGISTRATION FilterRegistration;  // defined in vinit.c


/************************************************************************* Prototypes**********/
NTSTATUS InstanceSetup (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType );

VOID CleanupVolumeContext( 
	__in PFLT_CONTEXT Context, 
	__in FLT_CONTEXT_TYPE ContextType );

NTSTATUS InstanceQueryTeardown ( 
	__in PCFLT_RELATED_OBJECTS FltObjects, 
	__in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags );

DRIVER_INITIALIZE DriverEntry;
NTSTATUS DriverEntry ( 
	__in PDRIVER_OBJECT DriverObject, 
	__in PUNICODE_STRING RegistryPath );

NTSTATUS FilterUnload ( __in FLT_FILTER_UNLOAD_FLAGS Flags );

FLT_PREOP_CALLBACK_STATUS VguardPreRead(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext );

FLT_POSTOP_CALLBACK_STATUS VguardPostRead(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags  );

FLT_POSTOP_CALLBACK_STATUS VguardPostReadWhenSafe(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags );

FLT_PREOP_CALLBACK_STATUS VguardPreDirCtrl(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext  );

FLT_POSTOP_CALLBACK_STATUS VguardPostDirCtrl(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags  );

FLT_POSTOP_CALLBACK_STATUS VguardPostDirCtrlWhenSafe (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags );

FLT_PREOP_CALLBACK_STATUS VguardPreWrite(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext );

FLT_POSTOP_CALLBACK_STATUS VguardPostWrite(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags );


// fanghui 20101229
FLT_PREOP_CALLBACK_STATUS VguardPreCleanup (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext );


// fanghui
FLT_PREOP_CALLBACK_STATUS VguardPreClose (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext );



VOID ReadDriverParameters ( 
	__in PUNICODE_STRING RegistryPath );


// fanghui 20101222
//ULONG GetProcessNameOffset();    


//  Assign text sections for each routine.
#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, InstanceSetup)
#pragma alloc_text(PAGE, CleanupVolumeContext)
#pragma alloc_text(PAGE, InstanceQueryTeardown)
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(INIT, ReadDriverParameters)
#pragma alloc_text(PAGE, FilterUnload)
#endif



#endif __VKERNEL_H__







