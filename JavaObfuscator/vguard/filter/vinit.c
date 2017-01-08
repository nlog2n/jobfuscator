/*++ 

Module: vinit.c
Environment:    Kernel mode 

--*/

#include "vkernel.h"

//---------------------------------------------------------------------------
//  Registration information for FLTMGR.
//---------------------------------------------------------------------------


//  Operation we currently care about.
CONST FLT_OPERATION_REGISTRATION Callbacks[] = {

//	{ IRP_MJ_CLEANUP, 1 /*FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO*/, VguardPreCleanup,   NULL },  /*fanghui*/
//	{ IRP_MJ_CLOSE,             0, VguardPreClose,     NULL },   /*fanghui*/


    { IRP_MJ_READ,              0, VguardPreRead,    VguardPostRead    },
    { IRP_MJ_WRITE,             0, VguardPreWrite,   VguardPostWrite   },
    { IRP_MJ_DIRECTORY_CONTROL, 0, VguardPreDirCtrl, VguardPostDirCtrl },
    { IRP_MJ_OPERATION_END }
};

//  Context definitions we currently care about.  Note that the system will
//  create a lookAside list for the volume context because an explicit size
//  of the context is specified.
CONST FLT_CONTEXT_REGISTRATION ContextNotifications[] = {
     { FLT_VOLUME_CONTEXT, 0, CleanupVolumeContext, sizeof(VOLUME_CONTEXT), CONTEXT_TAG },
     { FLT_CONTEXT_END }
};

//  define what we want to filter with FltMgr
CONST FLT_REGISTRATION FilterRegistration = {
    sizeof( FLT_REGISTRATION ),         //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    ContextNotifications,               //  Context
    Callbacks,                          //  Operation callbacks

    FilterUnload,                       //  MiniFilterUnload

    InstanceSetup,                      //  InstanceSetup
    InstanceQueryTeardown,              //  InstanceQueryTeardown
    NULL,                               //  InstanceTeardownStart
    NULL,                               //  InstanceTeardownComplete

    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent
};




