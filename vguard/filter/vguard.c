/*++   
Module:      vguard.c
Abstract:    A file filter which accesses and swaps file data buffer for
                 IRP_MJ_READ
                 IRP_MJ_WRITE
                 IRP_MJ_DIRECTORY_CONTROL
    By default this filter attaches to all volumes it is notified about.  It
    does support having multiple instances on a given volume.
Environment:      Kernel mode
Author:   Fang Hui,  hfang@i2r.a-star.edu.sg 
--*/


#include "vguard.h"



//----------------------------------------------------------------------
//                       G L O B A L S
//---------------------------------------------------------------------- 

PFLT_FILTER gFilterHandle;

// This is the offset into a KPEB of the current process name. This is determined
// dynamically by scanning the process block belonging to the GUI for the name
// of the system process, in who's context we execute in DriverEntry
ULONG               ProcessNameOffset = 0 ;


//  This is a lookAside list used to allocate our pre-2-post structure.
NPAGED_LOOKASIDE_LIST Pre2PostContextList;



//ULONG LoggingFlags = 0;             // all disabled by default
ULONG LoggingFlags = 39;      // only enalbe vguard related, read, write and errors





//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//                      Routines
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
NTSTATUS InstanceSetup (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
    )
/*++
Func: This routine is called whenever a new instance is created on a volume.
    By default we want to attach to all volumes.  This routine will try and
    get a "DOS" name for the given volume.  If it can't, it will try and
    get the "NT" name for the volume (which is what happens on network
    volumes).  If a name is retrieved a volume context will be created with  that name.
Arguments:
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.
    Flags - Flags describing the reason for this attach request.
Return:
    STATUS_SUCCESS - attach
    STATUS_FLT_DO_NOT_ATTACH - do not attach
--*/
{
    PDEVICE_OBJECT devObj = NULL;
    PVOLUME_CONTEXT ctx = NULL;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG retLen;
    PUNICODE_STRING workingName;
    USHORT size;
    UCHAR volPropBuffer[sizeof(FLT_VOLUME_PROPERTIES)+512];
    PFLT_VOLUME_PROPERTIES volProp = (PFLT_VOLUME_PROPERTIES)volPropBuffer;

    PAGED_CODE();

    UNREFERENCED_PARAMETER( Flags );
    UNREFERENCED_PARAMETER( VolumeDeviceType );
    UNREFERENCED_PARAMETER( VolumeFilesystemType );

    try {
        //  Allocate a volume context structure.
        status = FltAllocateContext( FltObjects->Filter, FLT_VOLUME_CONTEXT, sizeof(VOLUME_CONTEXT), NonPagedPool, &ctx );
        if (!NT_SUCCESS(status)) {
            leave; //  We could not allocate a context, quit now
        }

        //  Always get the volume properties, so I can get a sector size
        status = FltGetVolumeProperties( FltObjects->Volume, volProp, sizeof(volPropBuffer), &retLen );
        if (!NT_SUCCESS(status)) {
            leave;
        }

        //  Save the sector size in the context for later use.  Note that
        //  we will pick a minimum sector size if a sector size is not specified.
        ASSERT((volProp->SectorSize == 0) || (volProp->SectorSize >= MIN_SECTOR_SIZE));
        ctx->SectorSize = max(volProp->SectorSize,MIN_SECTOR_SIZE);
        ctx->Name.Buffer = NULL; //  Init the buffer field (which may be allocated later).

        //  Get the storage device object we want a name for.
        status = FltGetDiskDeviceObject( FltObjects->Volume, &devObj );
        if (NT_SUCCESS(status)) {
            //  Try and get the DOS name.  If it succeeds we will have
            //  an allocated name buffer.  If not, it will be NULL
#pragma prefast(suppress:__WARNING_USE_OTHER_FUNCTION, "Used to maintain compatability with Win 2k")
            status = RtlVolumeDeviceToDosName( devObj, &ctx->Name );
        }

        //  If we could not get a DOS name, get the NT name.
        if (!NT_SUCCESS(status)) {
            ASSERT(ctx->Name.Buffer == NULL);
            //  Figure out which name to use from the properties
            if (volProp->RealDeviceName.Length > 0) {
                workingName = &volProp->RealDeviceName;
            } else if (volProp->FileSystemDeviceName.Length > 0) {
                workingName = &volProp->FileSystemDeviceName;
            } else {
                //  No name, don't save the context
                status = STATUS_FLT_DO_NOT_ATTACH;
                leave;
            }

            //  Get size of buffer to allocate.  This is the length of the string plus room for a trailing colon.
            size = workingName->Length + sizeof(WCHAR);

            //  Now allocate a buffer to hold this name
#pragma prefast(suppress:__WARNING_MEMORY_LEAK, "ctx->Name.Buffer will not be leaked because it is freed in CleanupVolumeContext")
            ctx->Name.Buffer = ExAllocatePoolWithTag( NonPagedPool, size, NAME_TAG );
            if (ctx->Name.Buffer == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                leave;
            }

            //  Init the rest of the fields
            ctx->Name.Length = 0;
            ctx->Name.MaximumLength = size;

            RtlCopyUnicodeString( &ctx->Name, workingName ); //  Copy the name in
            RtlAppendUnicodeToString( &ctx->Name, L":" ); //  Put a trailing colon to make the display look good
        }

        //  Set the context
        status = FltSetVolumeContext( FltObjects->Volume, FLT_SET_CONTEXT_KEEP_IF_EXISTS, ctx, NULL );

        //  Log debug info
        LOG_PRINT( LOGFL_VOLCTX,
                   ("Vguard!InstanceSetup:                  Real SectSize=0x%04x, Used SectSize=0x%04x, Name=\"%wZ\"\n",
                    volProp->SectorSize,
                    ctx->SectorSize,
                    &ctx->Name) );

        //  It is OK for the context to already be defined.
        if (status == STATUS_FLT_CONTEXT_ALREADY_DEFINED) {
            status = STATUS_SUCCESS;
        }
    } finally {
        //  Always release the context.  If the set failed, it will free the context.  If not, it will remove the reference added by the set.
        //  Note that the name buffer in the ctx will get freed by the context cleanup routine.
        if (ctx) {
            FltReleaseContext( ctx );
        }

        //  Remove the reference added to the device object by FltGetDiskDeviceObject.
        if (devObj) {
            ObDereferenceObject( devObj );
        }
    }
    return status;
}

VOID CleanupVolumeContext( __in PFLT_CONTEXT Context, __in FLT_CONTEXT_TYPE ContextType )
/*++
Func: free the given context.   Free the allocated name buffer if there one.
Arguments:
    Context - The context being freed
    ContextType - The type of context this is
Return:    None
--*/
{
    PVOLUME_CONTEXT ctx = Context;
    PAGED_CODE();
    UNREFERENCED_PARAMETER( ContextType );
    ASSERT(ContextType == FLT_VOLUME_CONTEXT);
    if (ctx->Name.Buffer != NULL) {
        ExFreePool(ctx->Name.Buffer);
        ctx->Name.Buffer = NULL;
    }
}

NTSTATUS InstanceQueryTeardown ( __in PCFLT_RELATED_OBJECTS FltObjects, __in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags )
/*++
Func:  Called when an instance is being manually deleted by a call to FltDetachVolume or FilterDetach.  
Arguments:
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.
    Flags - Indicating where this detach request came from.
Return:    Always succeed to detach.
--*/
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );
    return STATUS_SUCCESS;
}


/************************************************************************* Initialization and unload routines.******/
NTSTATUS DriverEntry ( __in PDRIVER_OBJECT DriverObject,  __in PUNICODE_STRING RegistryPath )
/*++
Func:  initialization routine.  This registers with FltMgr and initializes all global data structures.
Arguments:
    DriverObject - Pointer to driver object created by the system to represent this driver.
    RegistryPath - Unicode string identifying where the parameters for this driver are located in the registry.
Return:    Status of the operation
--*/
{
    NTSTATUS status;
     
    ReadDriverParameters( RegistryPath ); //  Get debug trace flags


    // Find the process name offset.   fanghui 20101224
    ProcessNameOffset = GetProcessNameOffset();


	

    //  Init lookaside list used to allocate our context structure used to
    //  pass information from out preOperation callback to our postOperation callback.
    ExInitializeNPagedLookasideList( &Pre2PostContextList,
                                     NULL,
                                     NULL,
                                     0,
                                     sizeof(PRE_2_POST_CONTEXT),
                                     PRE_2_POST_TAG,
                                     0 );

    //  Register with FltMgr
    status = FltRegisterFilter( DriverObject, &FilterRegistration, &gFilterHandle );
    if (! NT_SUCCESS( status )) {
        goto SwapDriverEntryExit;
    }

    //  Start filtering i/o
    status = FltStartFiltering( gFilterHandle );
    if (! NT_SUCCESS( status )) {
        FltUnregisterFilter( gFilterHandle );
        goto SwapDriverEntryExit;
    }

SwapDriverEntryExit:
    if(! NT_SUCCESS( status )) {
        ExDeleteNPagedLookasideList( &Pre2PostContextList );
    }
    return status;
}


NTSTATUS FilterUnload (  __in FLT_FILTER_UNLOAD_FLAGS Flags )
/*++
Func: Called when this mini-filter is about to be unloaded.  We unregister
    from the FltMgr and then return it is OK to unload
Arguments:
    Flags - Indicating if this is a mandatory unload.
Return:  Returns the final status of this operation.
--*/
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER( Flags );
    FltUnregisterFilter( gFilterHandle ); //  Unregister from FLT mgr
    ExDeleteNPagedLookasideList( &Pre2PostContextList ); //  Delete lookaside list
    return STATUS_SUCCESS;
}





/************************************************************************* MiniFilter callback routines.******/
FLT_PREOP_CALLBACK_STATUS VguardPreRead( 
    __inout PFLT_CALLBACK_DATA Data, 
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext )
/*++
Func: swap buffers for the READ operation.
    Note that it handles all errors by simply not doing the buffer swap.
Arguments:
    Data - Pointer to the filter callbackData that is passed to us.
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and file object.
    CompletionContext - Receives the context that will be passed to the post-operation callback.
Return:
    FLT_PREOP_SUCCESS_WITH_CALLBACK - we want a postOpeation callback
    FLT_PREOP_SUCCESS_NO_CALLBACK - we don't want a postOperation callback
--*/
{
    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
    FLT_PREOP_CALLBACK_STATUS retValue = FLT_PREOP_SUCCESS_NO_CALLBACK;
    PVOID newBuf = NULL;
    PMDL newMdl = NULL;
    PVOLUME_CONTEXT volCtx = NULL;
    PPRE_2_POST_CONTEXT p2pCtx;
    NTSTATUS status;
    ULONG readLen = iopb->Parameters.Read.Length;

    try {

        /////////////////////////  fanghui start

		if ( !MonitorCurrentFileAccess( Data, FltObjects, LOGFL_READ ) ) {
			// simply return without post-op callback
			leave;  // will jump to finally (including finally block)
		}

        LOG_PRINT( LOGFL_READ, ("Vguard!VguardPreRead: 		   Start\n"));


/* 
		//fast io path, disallow it, this will lead to an equivalent irp request coming in
		if (FLT_IS_FASTIO_OPERATION(Data)) { 
			// disallow fast io path
			retValue = FLT_PREOP_DISALLOW_FASTIO ;
			leave ;
		} 
		
		//cached io irp path
		if (!(Data->Iopb->IrpFlags & (IRP_NOCACHE | IRP_PAGING_IO | IRP_SYNCHRONOUS_PAGING_IO))) {	
			leave ;
		}
*/

        ///////////////////////// fanghui end


	
        //  If they are trying to read ZERO bytes, then don't do anything and don't need a post-operation callback.
        if (readLen == 0) {
            leave;
        }

        //  Get our volume context so we can display our volume name in the debug output.
        status = FltGetVolumeContext( FltObjects->Filter, FltObjects->Volume, &volCtx );
        if (!NT_SUCCESS(status)) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreRead:             Error getting volume context, status=%x\n",
                        status) );
            leave;
        }

        //  If this is a non-cached I/O we need to round the length up to the sector size for this device.
        //  We must do this because the file systems do this and we need to make sure our buffer is as big
        //  as they are expecting.
        if (FlagOn(IRP_NOCACHE,iopb->IrpFlags)) {
            readLen = (ULONG)ROUND_TO_SIZE(readLen,volCtx->SectorSize);
        }

        //  Allocate nonPaged memory for the buffer we are swapping to.
        //  If we fail to get the memory, just don't swap buffers on this operation.
        newBuf = ExAllocatePoolWithTag( NonPagedPool,  readLen, BUFFER_SWAP_TAG );
        if (newBuf == NULL) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreRead:             %wZ Failed to allocate %d bytes of memory\n",
                        &volCtx->Name,
                        readLen) );
            leave;
        }

        //  We only need to build a MDL for IRP operations.  We don't need to
        //  do this for a FASTIO operation since the FASTIO interface has no
        //  parameter for passing the MDL to the file system.
        if (FlagOn(Data->Flags,FLTFL_CALLBACK_DATA_IRP_OPERATION)) {
            //  Allocate a MDL for the new allocated memory.  If we fail
            //  the MDL allocation then we won't swap buffer for this operation
            newMdl = IoAllocateMdl( newBuf, readLen, FALSE, FALSE, NULL );
            if (newMdl == NULL) {
                LOG_PRINT( LOGFL_ERRORS,
                           ("Vguard!VguardPreRead:             %wZ Failed to allocate MDL\n",
                            &volCtx->Name) );
                leave;
            }
            MmBuildMdlForNonPagedPool( newMdl ); //  setup the MDL for the non-paged pool we just allocated
        }

        //  We are ready to swap buffers, get a pre2Post context structure. We need it to
        //  pass the volume context and the allocate memory buffer to the post operation callback.
        p2pCtx = ExAllocateFromNPagedLookasideList( &Pre2PostContextList );
        if (p2pCtx == NULL) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreRead:             %wZ Failed to allocate pre2Post context structure\n",
                        &volCtx->Name) );
            leave;
        }

        //  Log that we are swapping
        LOG_PRINT( LOGFL_READ,
                   ("Vguard!VguardPreRead:             Swapping newB=%p newMdl=%p oldB=%p oldMdl=%p len=%d\n",
                    /*&volCtx->Name,*/
                    newBuf,
                    newMdl,
                    iopb->Parameters.Read.ReadBuffer,
                    iopb->Parameters.Read.MdlAddress,
                    readLen) );
		LOG_PRINT( LOGFL_READ, ("Vguard!VguardPreRead: 		   End\n"));


        //  Update the buffer pointers and MDL address, mark we have changed something.
        iopb->Parameters.Read.ReadBuffer = newBuf;
        iopb->Parameters.Read.MdlAddress = newMdl;
        FltSetCallbackDataDirty( Data );

        //  Pass state to our post-operation callback.
        p2pCtx->SwappedBuffer = newBuf;
        p2pCtx->VolCtx = volCtx;
        *CompletionContext = p2pCtx;

        retValue = FLT_PREOP_SUCCESS_WITH_CALLBACK; //  Return we want a post-operation callback
    } finally {
        //  If we don't want a post-operation callback, then cleanup state.
        if (retValue != FLT_PREOP_SUCCESS_WITH_CALLBACK) {
            if (newBuf != NULL) {
                ExFreePool( newBuf );
            }
            if (newMdl != NULL) {
                IoFreeMdl( newMdl );
            }
            if (volCtx != NULL) {
                FltReleaseContext( volCtx );
            }
        }
    }
    return retValue;
}


FLT_POSTOP_CALLBACK_STATUS VguardPostRead(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags  )
/*++
Func: postRead buffer swap handling
Arguments:
    Data        - Pointer to the filter callbackData that is passed to us.
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and file object.
    CompletionContext - The completion context set in the pre-operation routine.
    Flags - Denotes whether the completion is successful or is being drained.
Return:
    FLT_POSTOP_FINISHED_PROCESSING
    FLT_POSTOP_MORE_PROCESSING_REQUIRED
--*/
{
    PVOID origBuf;
    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
    FLT_POSTOP_CALLBACK_STATUS retValue = FLT_POSTOP_FINISHED_PROCESSING;
    PPRE_2_POST_CONTEXT p2pCtx = CompletionContext;
    BOOLEAN cleanupAllocatedBuffer = TRUE;

    //  This system won't draining an operation with swapped buffers, verify the draining flag is not set.
    ASSERT(!FlagOn(Flags, FLTFL_POST_OPERATION_DRAINING));
	
    try {

        ////////////////// fanghui start

		LOG_PRINT( LOGFL_READ, ("Vguard!VguardPostRead: 		   Start\n"));
		
        ////////////////// fanghui start



		
        //  If the operation failed or the count is zero, there is no data to copy so just return now.
        if (!NT_SUCCESS(Data->IoStatus.Status) || (Data->IoStatus.Information == 0)) {
            LOG_PRINT( LOGFL_READ,
                       ("Vguard!VguardPostRead:            %wZ newB=%p No data read, status=%x, info=%x\n",
                        &p2pCtx->VolCtx->Name,
                        p2pCtx->SwappedBuffer,
                        Data->IoStatus.Status,
                        Data->IoStatus.Information) );
            leave;
        }

        //  We need to copy the read data back into the users buffer.  Note
        //  that the parameters passed in are for the users original buffers not our swapped buffers.
        if (iopb->Parameters.Read.MdlAddress != NULL) {
            //  There is a MDL defined for the original buffer, get a system address for it so we can 
            //  copy the data back to it.  We must do this because we don't know what thread context we are in.
            origBuf = MmGetSystemAddressForMdlSafe( iopb->Parameters.Read.MdlAddress, NormalPagePriority );
            if (origBuf == NULL) {
                LOG_PRINT( LOGFL_ERRORS,
                           ("Vguard!VguardPostRead:            %wZ Failed to get system address for MDL: %p\n",
                            &p2pCtx->VolCtx->Name,
                            iopb->Parameters.Read.MdlAddress) );

                //  If we failed to get a SYSTEM address, mark that the read failed and return.
                Data->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
                Data->IoStatus.Information = 0;
                leave;
            }
        } else if (FlagOn(Data->Flags,FLTFL_CALLBACK_DATA_SYSTEM_BUFFER) || FlagOn(Data->Flags,FLTFL_CALLBACK_DATA_FAST_IO_OPERATION)) {
            //  If this is a system buffer, just use the given address because it is valid in all thread contexts.
            //  If this is a FASTIO operation, we can just use the buffer (inside a try/except) since we know we are in
            //      the correct thread context (you can't pend FASTIO's).
            origBuf = iopb->Parameters.Read.ReadBuffer;
        } else {
            //  They don't have a MDL and this is not a system buffer or a fastio so this is probably some arbitrary user
            //  buffer.  We can not do the processing at DPC level so try and get to a safe IRQL so we can do the processing.
            if (FltDoCompletionProcessingWhenSafe( Data,
                                                   FltObjects,
                                                   CompletionContext,
                                                   Flags,
                                                   VguardPostReadWhenSafe,
                                                   &retValue )) {
                //  This operation has been moved to a safe IRQL, the called
                //  routine will do (or has done) the freeing so don't do it in our routine.
                cleanupAllocatedBuffer = FALSE;
            } else {
                //  We are in a state where we can not get to a safe IRQL and we do not have a MDL.  
                //  There is nothing we can do to safely copy the data back to the users buffer, fail the
                //  operation and return.  This shouldn't ever happen because in those situations where
                //  it is not safe to post, we should have a MDL.
                LOG_PRINT( LOGFL_ERRORS,
                           ("Vguard!VguardPostRead:            %wZ Unable to post to a safe IRQL\n",
                            &p2pCtx->VolCtx->Name) );

                Data->IoStatus.Status = STATUS_UNSUCCESSFUL;
                Data->IoStatus.Information = 0;
            }
            leave;
        }

        //  We either have a system buffer or this is a fastio operation
        //  so we are in the proper context.  Copy the data, handling an exception.
        try {
			LOG_PRINT( LOGFL_READ,
					   ("Vguard!VguardPostRead:            Decrypt buf=%p, len=%d, offset=%d\n",
						p2pCtx->SwappedBuffer,
						Data->IoStatus.Information,
						(ULONG)Data->Iopb->Parameters.Read.ByteOffset.QuadPart ));

            // byteoffset is a 64-bit LARGE_INTEGER
			//KeEnterCriticalRegion() ;
			DecryptBuffer( p2pCtx->SwappedBuffer, 
			               Data->IoStatus.Information, 
			               (ULONG)Data->Iopb->Parameters.Read.ByteOffset.QuadPart);
			//KeLeaveCriticalRegion() ; 	
			
            RtlCopyMemory( origBuf,  p2pCtx->SwappedBuffer, Data->IoStatus.Information );  
			LOG_PRINT( LOGFL_READ, ("Vguard!VguardPostRead:            Copying back to origBuf=%p\n",
				                    origBuf ));
		
        } except (EXCEPTION_EXECUTE_HANDLER) {
            //  The copy failed, return an error, failing the operation.
            Data->IoStatus.Status = GetExceptionCode();
            Data->IoStatus.Information = 0;
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPostRead:            %wZ Invalid user buffer, oldB=%p, status=%x\n",
                        &p2pCtx->VolCtx->Name,
                        origBuf,
                        Data->IoStatus.Status) );
        }
    } finally {
        //  If we are supposed to, then cleanup the allocated memory and release
        //  the volume context.  The freeing of the MDL (if there is one) is handled by FltMgr.
        if (cleanupAllocatedBuffer) {
            LOG_PRINT( LOGFL_READ,
                       ("Vguard!VguardPostRead:            Freeing swapped newB=%p info=%d\n",
                        /* &p2pCtx->VolCtx->Name,*/
                        p2pCtx->SwappedBuffer,
                        Data->IoStatus.Information) );

			LOG_PRINT( LOGFL_READ, ("Vguard!VguardPostRead: 		   End\n"));


            ExFreePool( p2pCtx->SwappedBuffer );
            FltReleaseContext( p2pCtx->VolCtx );
            ExFreeToNPagedLookasideList( &Pre2PostContextList, p2pCtx );
        }
    }
    return retValue;
}


FLT_POSTOP_CALLBACK_STATUS VguardPostReadWhenSafe (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags  )
/*++
Func: We had an arbitrary users buffer without a MDL so we needed to get
    to a safe IRQL so we could lock it and then copy the data.
Arguments:
    Data - Pointer to the filter callbackData that is passed to us.
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and file object.
    CompletionContext - Contains state from our PreOperation callback
    Flags - Denotes whether the completion is successful or is being drained.
Called by:  hooked in VguardPostRead by kernel function FltDoCompletionProcessingWhenSafe    
Return: always FLT_POSTOP_FINISHED_PROCESSING 
--*/
{
    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
    PPRE_2_POST_CONTEXT p2pCtx = CompletionContext;
    PVOID origBuf;
    NTSTATUS status;

    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );
    ASSERT(Data->IoStatus.Information != 0);


	LOG_PRINT( LOGFL_READ, ("Vguard!VguardPostReadWhenSafe: 		   Start\n"));



    //  This is some sort of user buffer without a MDL, lock the user buffer so we can access it.  This will create a MDL for it.
    status = FltLockUserBuffer( Data );
    if (!NT_SUCCESS(status)) {
        LOG_PRINT( LOGFL_ERRORS,
                   ("Vguard!VguardPostReadWhenSafe:    %wZ Could not lock user buffer, oldB=%p, status=%x\n",
                    &p2pCtx->VolCtx->Name,
                    iopb->Parameters.Read.ReadBuffer,
                    status) );
        //  If we can't lock the buffer, fail the operation
        Data->IoStatus.Status = status;
        Data->IoStatus.Information = 0;
    } else {
        //  Get a system address for this buffer.
        origBuf = MmGetSystemAddressForMdlSafe( iopb->Parameters.Read.MdlAddress, NormalPagePriority );
        if (origBuf == NULL) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPostReadWhenSafe:    %wZ Failed to get system address for MDL: %p\n",
                        &p2pCtx->VolCtx->Name,
                        iopb->Parameters.Read.MdlAddress) );

            //  If we couldn't get a SYSTEM buffer address, fail the operation
            Data->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
            Data->IoStatus.Information = 0;
        } else {
			LOG_PRINT( LOGFL_READ,
					   ("Vguard!VguardPostReadWhenSafe:				Decrypt buf=%p, len=%d, offset=%d\n",
						p2pCtx->SwappedBuffer,
						Data->IoStatus.Information,
						(ULONG)Data->Iopb->Parameters.Read.ByteOffset.QuadPart ));

            // byteoffset is a 64-bit LARGE_INTEGER
			//KeEnterCriticalRegion() ;
			DecryptBuffer( p2pCtx->SwappedBuffer, 
			               Data->IoStatus.Information, 
			               (ULONG)Data->Iopb->Parameters.Read.ByteOffset.QuadPart);
			//KeLeaveCriticalRegion() ; 	


            //  Copy the data back to the original buffer.  
            //  Note: don't need a try/except because we will always have a system buffer address.
            RtlCopyMemory( origBuf, p2pCtx->SwappedBuffer, Data->IoStatus.Information ); 

   		    LOG_PRINT( LOGFL_READ, ("Vguard!VguardPostReadWhenSafe:				Copying back to origBuf=%p\n",
								origBuf ));
			

        }
    }

    //  Free allocated memory and release the volume context
    LOG_PRINT( LOGFL_READ,
               ("Vguard!VguardPostReadWhenSafe:				Freeing swapped newB=%p info=%d\n",
                /* &p2pCtx->VolCtx->Name,*/
                p2pCtx->SwappedBuffer,
                Data->IoStatus.Information) );
	LOG_PRINT( LOGFL_READ, ("Vguard!VguardPostReadWhenSafe: 		   End\n"));



    ExFreePool( p2pCtx->SwappedBuffer );
    FltReleaseContext( p2pCtx->VolCtx );
    ExFreeToNPagedLookasideList( &Pre2PostContextList, p2pCtx );
    return FLT_POSTOP_FINISHED_PROCESSING;
}




FLT_PREOP_CALLBACK_STATUS VguardPreWrite(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext )
/*++
Func: swap buffers for the WRITE operation.
    Note that it handles all errors by simply not doing the buffer swap.
Arguments:
    Data - Pointer to the filter callbackData that is passed to us.
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and file object.
    CompletionContext - Receives the context that will be passed to the post-operation callback.
Return:
    FLT_PREOP_SUCCESS_WITH_CALLBACK - we want a postOpeation callback
    FLT_PREOP_SUCCESS_NO_CALLBACK - we don't want a postOperation callback
    FLT_PREOP_COMPLETE -
--*/
{
    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
    FLT_PREOP_CALLBACK_STATUS retValue = FLT_PREOP_SUCCESS_NO_CALLBACK;
    PVOID newBuf = NULL;
    PMDL newMdl = NULL;
    PVOLUME_CONTEXT volCtx = NULL;
    PPRE_2_POST_CONTEXT p2pCtx;
    PVOID origBuf;
    NTSTATUS status;
    ULONG writeLen = iopb->Parameters.Write.Length;

    try {

		/////////////////////////  fanghui start
		
		if ( !MonitorCurrentFileAccess( Data, FltObjects, LOGFL_WRITE ) ) {
			// simply return without post-op callback
			leave;	// will jump to finally (including finally block)
		}
		
		LOG_PRINT( LOGFL_WRITE, ("Vguard!VguardPreWrite:		   Start\n"));
		
		///////////////////////// fanghui end

		
        //  If they are trying to write ZERO bytes, then don't do anything and don't need a post-operation callback.
        if (writeLen == 0) {
            leave;
        }
		
        //  Get our volume context so we can display our volume name in the debug output.
        status = FltGetVolumeContext( FltObjects->Filter, FltObjects->Volume, &volCtx );
        if (!NT_SUCCESS(status)) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreWrite:            Error getting volume context, status=%x\n",
                        status) );
            leave;
        }

        //  If this is a non-cached I/O we need to round the length up to the sector size for this device.  
        //  We must do this because the file systems do this and we need to make sure our buffer is as big
        //  as they are expecting.
        if (FlagOn(IRP_NOCACHE,iopb->IrpFlags)) {
            writeLen = (ULONG)ROUND_TO_SIZE(writeLen,volCtx->SectorSize);
        }

        //  Allocate nonPaged memory for the buffer we are swapping to.
        //  If we fail to get the memory, just don't swap buffers on this operation.
        newBuf = ExAllocatePoolWithTag( NonPagedPool, writeLen, BUFFER_SWAP_TAG );
        if (newBuf == NULL) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreWrite:            %wZ Failed to allocate %d bytes of memory.\n",
                        &volCtx->Name,
                        writeLen) );
            leave;
        }

        //  We only need to build a MDL for IRP operations.  We don't need to
        //  do this for a FASTIO operation because it is a waste of time since
        //  the FASTIO interface has no parameter for passing the MDL to the file system.
        if (FlagOn(Data->Flags,FLTFL_CALLBACK_DATA_IRP_OPERATION)) {
            //  Allocate a MDL for the new allocated memory.  If we fail
            //  the MDL allocation then we won't swap buffer for this operation
            newMdl = IoAllocateMdl( newBuf, writeLen, FALSE, FALSE, NULL );
            if (newMdl == NULL) {
                LOG_PRINT( LOGFL_ERRORS,
                           ("Vguard!VguardPreWrite:            %wZ Failed to allocate MDL.\n",
                            &volCtx->Name) );
                leave;
            }

            MmBuildMdlForNonPagedPool( newMdl ); //  setup the MDL for the non-paged pool we just allocated
        }

        //  If the users original buffer had a MDL, get a system address.
        if (iopb->Parameters.Write.MdlAddress != NULL) {
            origBuf = MmGetSystemAddressForMdlSafe( iopb->Parameters.Write.MdlAddress, NormalPagePriority );
            if (origBuf == NULL) {
                LOG_PRINT( LOGFL_ERRORS,
                           ("Vguard!VguardPreWrite:            %wZ Failed to get system address for MDL: %p\n",
                            &volCtx->Name,
                            iopb->Parameters.Write.MdlAddress) );
                //  If we could not get a system address for the users buffer,
                //  then we are going to fail this operation.
                Data->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
                Data->IoStatus.Information = 0;
                retValue = FLT_PREOP_COMPLETE;
                leave;
            }
        } else {
            origBuf = iopb->Parameters.Write.WriteBuffer; //  There was no MDL defined, use the given buffer address.
        }

        //  Copy the memory, we must do this inside the try/except because we may be using a users buffer address
        try {

			LOG_PRINT( LOGFL_WRITE, ("Vguard!VguardPreWrite:            Copying to newBuf=%p len=%d\n",
				                    newBuf,
				                    writeLen));
            RtlCopyMemory( newBuf, origBuf, writeLen );


			LOG_PRINT( LOGFL_WRITE,
					   ("Vguard!VguardPreWrite:            Encrypt newBuf=%p, len=%d, offset=%d\n",
						newBuf,
						writeLen,
						(ULONG)Data->Iopb->Parameters.Write.ByteOffset.QuadPart ));

            // byteoffset is a 64-bit LARGE_INTEGER
			//KeEnterCriticalRegion() ;
			EncryptBuffer( newBuf, 
			               writeLen, 
			               (ULONG)Data->Iopb->Parameters.Write.ByteOffset.QuadPart);
			//KeLeaveCriticalRegion() ; 	
			
			
        } except (EXCEPTION_EXECUTE_HANDLER) {
            //  The copy failed, return an error, failing the operation.
            Data->IoStatus.Status = GetExceptionCode();
            Data->IoStatus.Information = 0;
            retValue = FLT_PREOP_COMPLETE;
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreWrite:            %wZ Invalid user buffer, oldB=%p, status=%x\n",
                        &volCtx->Name,
                        origBuf,
                        Data->IoStatus.Status) );
            leave;
        }

        //  We are ready to swap buffers, get a pre2Post context structure. We need it to pass 
        //  the volume context and the allocate memory buffer to the post operation callback.
        p2pCtx = ExAllocateFromNPagedLookasideList( &Pre2PostContextList );
        if (p2pCtx == NULL) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreWrite:            %wZ Failed to allocate pre2Post context structure\n",
                        &volCtx->Name) );
            leave;
        }


        LOG_PRINT( LOGFL_WRITE,
                   ("Vguard!VguardPreWrite:            Swapping newB=%p newMdl=%p oldB=%p oldMdl=%p len=%d\n",
                    /*&volCtx->Name,*/
                    newBuf,
                    newMdl,
                    iopb->Parameters.Write.WriteBuffer,  /* if Mdl = 0, then origBuf=WriteBuffer */
                    iopb->Parameters.Write.MdlAddress,
                    writeLen) );

		LOG_PRINT( LOGFL_WRITE, ("Vguard!VguardPreWrite: 		   End\n"));

        //  Set new buffers
        iopb->Parameters.Write.WriteBuffer = newBuf;
        iopb->Parameters.Write.MdlAddress = newMdl;
        FltSetCallbackDataDirty( Data );

        //  Pass state to our post-operation callback.
        p2pCtx->SwappedBuffer = newBuf;
        p2pCtx->VolCtx = volCtx;
        *CompletionContext = p2pCtx;
        retValue = FLT_PREOP_SUCCESS_WITH_CALLBACK; //  Return we want a post-operation callback
    } finally {
        //  If we don't want a post-operation callback, then free the buffer or MDL if it was allocated.
        if (retValue != FLT_PREOP_SUCCESS_WITH_CALLBACK) {
            if (newBuf != NULL) {
                ExFreePool( newBuf );
            }
            if (newMdl != NULL) {
                IoFreeMdl( newMdl );
            }
            if (volCtx != NULL) {
                FltReleaseContext( volCtx );
            }
        }
    }
    return retValue;
}

FLT_POSTOP_CALLBACK_STATUS VguardPostWrite(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags )
{
    PPRE_2_POST_CONTEXT p2pCtx = CompletionContext;
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    LOG_PRINT( LOGFL_WRITE,
               ("Vguard!VguardPostWrite:           Freeing newB=%p info=%d\n",
                /*&p2pCtx->VolCtx->Name,*/
                p2pCtx->SwappedBuffer,
                Data->IoStatus.Information) );


    //  Free allocate POOL and volume context
    ExFreePool( p2pCtx->SwappedBuffer );
    FltReleaseContext( p2pCtx->VolCtx );
    ExFreeToNPagedLookasideList( &Pre2PostContextList, p2pCtx );
    return FLT_POSTOP_FINISHED_PROCESSING;
}






FLT_PREOP_CALLBACK_STATUS VguardPreDirCtrl( 
	__inout PFLT_CALLBACK_DATA Data, 
	__in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext    )
/*++
Func: swap buffers for the Directory Control operations.  
    The reason this routine is here is because directory change notifications are long lived and this allows you to see how FltMgr
    handles long lived IRP operations that have swapped buffers when the mini-filter is unloaded.  It does this by canceling the IRP.

    Note that it handles all errors by simply not doing the  buffer swap.
Arguments:
    Data - Pointer to the filter callbackData that is passed to us.
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and file object.
    CompletionContext - Receives the context that will be passed to the post-operation callback.
Return:
    FLT_PREOP_SUCCESS_WITH_CALLBACK - we want a postOpeation callback
    FLT_PREOP_SUCCESS_NO_CALLBACK - we don't want a postOperation callback
--*/
{
    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
    FLT_PREOP_CALLBACK_STATUS retValue = FLT_PREOP_SUCCESS_NO_CALLBACK;
    PVOID newBuf = NULL;
    PMDL newMdl = NULL;
    PVOLUME_CONTEXT volCtx = NULL;
    PPRE_2_POST_CONTEXT p2pCtx;
    NTSTATUS status;

    try {
        //  If they are trying to get ZERO bytes, then don't do anything and don't need a post-operation callback.
        if (iopb->Parameters.DirectoryControl.QueryDirectory.Length == 0) {
            leave;
        }

        //  Get our volume context.  If we can't get it, just return.
        status = FltGetVolumeContext( FltObjects->Filter, FltObjects->Volume, &volCtx );
        if (!NT_SUCCESS(status)) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreDirCtrl:          Error getting volume context, status=%x\n",
                        status) );
            leave;
        }

        //  Allocate nonPaged memory for the buffer we are swapping to.
        //  If we fail to get the memory, just don't swap buffers on this operation.
        newBuf = ExAllocatePoolWithTag( NonPagedPool,
                                        iopb->Parameters.DirectoryControl.QueryDirectory.Length,
                                        BUFFER_SWAP_TAG );
        if (newBuf == NULL) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreDirCtrl:          %wZ Failed to allocate %d bytes of memory.\n",
                        &volCtx->Name,
                        iopb->Parameters.DirectoryControl.QueryDirectory.Length) );
            leave;
        }

        //  We need to build a MDL because Directory Control Operations are always IRP operations.  

        //  Allocate a MDL for the new allocated memory.  If we fail the MDL allocation then we won't swap buffer for this operation
        newMdl = IoAllocateMdl( newBuf,
                                iopb->Parameters.DirectoryControl.QueryDirectory.Length,
                                FALSE,
                                FALSE,
                                NULL );
        if (newMdl == NULL) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreDirCtrl:          %wZ Failed to allocate MDL.\n",
                        &volCtx->Name) );
           leave;
        }

        //  setup the MDL for the non-paged pool we just allocated
        MmBuildMdlForNonPagedPool( newMdl );

        //  We are ready to swap buffers, get a pre2Post context structure. We need it to pass
        //  the volume context and the allocate memory buffer to the post operation callback.
        p2pCtx = ExAllocateFromNPagedLookasideList( &Pre2PostContextList );
        if (p2pCtx == NULL) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPreDirCtrl:          %wZ Failed to allocate pre2Post context structure\n",
                        &volCtx->Name) );
            leave;
        }

        //  Log that we are swapping
        /*
              LOG_PRINT( LOGFL_DIRCTRL,
                   ("Vguard!VguardPreDirCtrl:          %wZ newB=%p newMdl=%p oldB=%p oldMdl=%p len=%d\n",
                    &volCtx->Name,
                    newBuf,
                    newMdl,
                    iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer,
                    iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress,
                    iopb->Parameters.DirectoryControl.QueryDirectory.Length) );
		*/

        //  Update the buffer pointers and MDL address
        iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer = newBuf;
        iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress = newMdl;
        FltSetCallbackDataDirty( Data );

        //  Pass state to our post-operation callback.
        p2pCtx->SwappedBuffer = newBuf;
        p2pCtx->VolCtx = volCtx;
        *CompletionContext = p2pCtx;

        retValue = FLT_PREOP_SUCCESS_WITH_CALLBACK;//  Return we want a post-operation callback
    } finally {
        //  If we don't want a post-operation callback, then cleanup state.
        if (retValue != FLT_PREOP_SUCCESS_WITH_CALLBACK) {
            if (newBuf != NULL) {
                ExFreePool( newBuf );
            }
            if (newMdl != NULL) {
                IoFreeMdl( newMdl );
            }
            if (volCtx != NULL) {
                FltReleaseContext( volCtx );
            }
        }
    }
    return retValue;
}

FLT_POSTOP_CALLBACK_STATUS VguardPostDirCtrl(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags  )
/*++
Func: post Directory Control buffer swap handling.
Arguments:
    Data - Pointer to the filter callbackData that is passed to us.
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and file object.
    CompletionContext - The completion context set in the pre-operation routine.
    Flags - Denotes whether the completion is successful or is being drained.
Return:
    FLT_POSTOP_FINISHED_PROCESSING
    FLT_POSTOP_MORE_PROCESSING_REQUIRED
--*/
{
    PVOID origBuf;
    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
    FLT_POSTOP_CALLBACK_STATUS retValue = FLT_POSTOP_FINISHED_PROCESSING;
    PPRE_2_POST_CONTEXT p2pCtx = CompletionContext;
    BOOLEAN cleanupAllocatedBuffer = TRUE;

    //  Verify we are not draining an operation with swapped buffers
    ASSERT(!FlagOn(Flags, FLTFL_POST_OPERATION_DRAINING));
    try {
        //  If the operation failed or the count is zero, there is no data to copy so just return now.
        if (!NT_SUCCESS(Data->IoStatus.Status) || (Data->IoStatus.Information == 0)) {
            LOG_PRINT( LOGFL_DIRCTRL,
                       ("Vguard!VguardPostDirCtrl:         %wZ newB=%p No data read, status=%x, info=%x\n",
                        &p2pCtx->VolCtx->Name,
                        p2pCtx->SwappedBuffer,
                        Data->IoStatus.Status,
                        Data->IoStatus.Information) );
            leave;
        }

        //  We need to copy the read data back into the users buffer.  Note
        //  that the parameters passed in are for the users original buffers not our swapped buffers
        if (iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress != NULL) {
            //  There is a MDL defined for the original buffer, get a
            //  system address for it so we can copy the data back to it.
            //  We must do this because we don't know what thread context  we are in.
            origBuf = MmGetSystemAddressForMdlSafe( iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress,
                                                    NormalPagePriority );
            if (origBuf == NULL) {
                LOG_PRINT( LOGFL_ERRORS,
                           ("Vguard!VguardPostDirCtrl:         %wZ Failed to get system address for MDL: %p\n",
                            &p2pCtx->VolCtx->Name,
                            iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress) );
                //  If we failed to get a SYSTEM address, mark that the operation failed and return.
                Data->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
                Data->IoStatus.Information = 0;
                leave;
            }
        } else if (FlagOn(Data->Flags,FLTFL_CALLBACK_DATA_SYSTEM_BUFFER) || FlagOn(Data->Flags,FLTFL_CALLBACK_DATA_FAST_IO_OPERATION)) {
            //  If this is a system buffer, just use the given address because it is valid in all thread contexts.
            //  If this is a FASTIO operation, we can just use the buffer (inside a try/except) since we know we are in
            //      the correct thread context.
            origBuf = iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;
        } else {
            //  They don't have a MDL and this is not a system buffer or a fastio so this is probably some
            //  arbitrary user buffer.  We can not do the processing at DPC level so
            //  try and get to a safe IRQL so we can do the processing.
            if (FltDoCompletionProcessingWhenSafe( Data,
                                                   FltObjects,
                                                   CompletionContext,
                                                   Flags,
                                                   VguardPostDirCtrlWhenSafe,
                                                   &retValue )) {
                //  This operation has been moved to a safe IRQL, the called
                //  routine will do (or has done) the freeing so don't do it in our routine.
                cleanupAllocatedBuffer = FALSE;
            } else {
                //  We are in a state where we can not get to a safe IRQL and we do not have a MDL.  
                //  There is nothing we can do to safely copy the data back to the users buffer, fail 
                //  the operation and return.  This shouldn't ever happen because in those
                //  situations where it is not safe to post, we should have a MDL.
                LOG_PRINT( LOGFL_ERRORS,
                           ("Vguard!VguardPostDirCtrl:         %wZ Unable to post to a safe IRQL\n",
                            &p2pCtx->VolCtx->Name) );
                Data->IoStatus.Status = STATUS_UNSUCCESSFUL;
                Data->IoStatus.Information = 0;
            }
            leave;
        }

        //  We either have a system buffer or this is a fastio operation
        //  so we are in the proper context.  Copy the data handling an exception.
        //
        //  NOTE:  Due to a bug in FASTFAT where it is returning the wrong
        //         length in the information field (it is sort) we are always
        //         going to copy the original buffer length.
        try {
            RtlCopyMemory( origBuf,
                           p2pCtx->SwappedBuffer,
                           /*Data->IoStatus.Information*/
                           iopb->Parameters.DirectoryControl.QueryDirectory.Length );
        } except (EXCEPTION_EXECUTE_HANDLER) {
            Data->IoStatus.Status = GetExceptionCode();
            Data->IoStatus.Information = 0;
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPostDirCtrl:         %wZ Invalid user buffer, oldB=%p, status=%x, info=%x\n",
                        &p2pCtx->VolCtx->Name,
                        origBuf,
                        Data->IoStatus.Status,
                        Data->IoStatus.Information) );
        }
    } finally {
        //  If we are supposed to, then cleanup the allocate memory and release
        //  the volume context.  The freeing of the MDL (if there is one) is handled by FltMgr.
        if (cleanupAllocatedBuffer) {
            LOG_PRINT( LOGFL_DIRCTRL,
                       ("Vguard!VguardPostDirCtrl:         %wZ newB=%p info=%d Freeing\n",
                        &p2pCtx->VolCtx->Name,
                        p2pCtx->SwappedBuffer,
                        Data->IoStatus.Information) );
            ExFreePool( p2pCtx->SwappedBuffer );
            FltReleaseContext( p2pCtx->VolCtx );
            ExFreeToNPagedLookasideList( &Pre2PostContextList, p2pCtx );
        }
    }
    return retValue;
}


FLT_POSTOP_CALLBACK_STATUS VguardPostDirCtrlWhenSafe (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags    )
/*++
Func: We had an arbitrary users buffer without a MDL so we needed to get
    to a safe IRQL so we could lock it and then copy the data.
Arguments:
    Data - Pointer to the filter callbackData that is passed to us.
    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.
    CompletionContext - The buffer we allocated and swapped to
    Flags - Denotes whether the completion is successful or is being drained.
Called by: hooked in VguardPostDirCtrl by kernel function FltDoCompletionProcessingWhenSafe 
Return:  always FLT_POSTOP_FINISHED_PROCESSING 
--*/
{
    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
    PPRE_2_POST_CONTEXT p2pCtx = CompletionContext;
    PVOID origBuf;
    NTSTATUS status;

    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );
    ASSERT(Data->IoStatus.Information != 0);

    //  This is some sort of user buffer without a MDL, lock the user buffer so we can access it
    status = FltLockUserBuffer( Data );
    if (!NT_SUCCESS(status)) {
        LOG_PRINT( LOGFL_ERRORS,
                   ("Vguard!VguardPostDirCtrlWhenSafe: %wZ Could not lock user buffer, oldB=%p, status=%x\n",
                    &p2pCtx->VolCtx->Name,
                    iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer,
                    status) );

        //  If we can't lock the buffer, fail the operation
        Data->IoStatus.Status = status;
        Data->IoStatus.Information = 0;
    } else {
        //  Get a system address for this buffer.
        origBuf = MmGetSystemAddressForMdlSafe( iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress,
                                                NormalPagePriority );
        if (origBuf == NULL) {
            LOG_PRINT( LOGFL_ERRORS,
                       ("Vguard!VguardPostDirCtrlWhenSafe: %wZ Failed to get System address for MDL: %p\n",
                        &p2pCtx->VolCtx->Name,
                        iopb->Parameters.DirectoryControl.QueryDirectory.MdlAddress) );
            //  If we couldn't get a SYSTEM buffer address, fail the operation
            Data->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
            Data->IoStatus.Information = 0;
        } else {
            //  Copy the data back to the original buffer
            //
            //  NOTE:  Due to a bug in FASTFAT where it is returning the wrong
            //         length in the information field (it is short) we are
            //         always going to copy the original buffer length.
            RtlCopyMemory( origBuf,
                           p2pCtx->SwappedBuffer,
                           /*Data->IoStatus.Information*/
                           iopb->Parameters.DirectoryControl.QueryDirectory.Length );
        }
    }

    //  Free the memory we allocated and return
    /*
    LOG_PRINT( LOGFL_DIRCTRL,
               ("Vguard!VguardPostDirCtrlWhenSafe: %wZ newB=%p info=%d Freeing\n",
                &p2pCtx->VolCtx->Name,
                p2pCtx->SwappedBuffer,
                Data->IoStatus.Information) );
	*/

    ExFreePool( p2pCtx->SwappedBuffer );
    FltReleaseContext( p2pCtx->VolCtx );
    ExFreeToNPagedLookasideList( &Pre2PostContextList,  p2pCtx );
    return FLT_POSTOP_FINISHED_PROCESSING;
}




// fanghui 20101229
FLT_PREOP_CALLBACK_STATUS VguardPreCleanup (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext )
{  
    BOOLEAN isDeleteCache;
    if ( !IsCurrentFileMonitored( Data, FltObjects )) {
		//leave;
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

	LOG_PRINT( LOGFL_VGUARD,
			   ("Vguard!VguardPreCleanup:	 		   to flush cache, status=%x\n",
				0) );

    	
	//flush cache and purge data in cache	
	Cc_ClearFileCache( Data->Iopb->TargetFileObject, TRUE, NULL, 0) ;

    if ( Data->Iopb->TargetFileObject->SectionObjectPointer != NULL ) {
		isDeleteCache = MmForceSectionClosed( Data->Iopb->TargetFileObject->SectionObjectPointer, FALSE);
		if (isDeleteCache) {
			LOG_PRINT( LOGFL_VGUARD,
					   ("Vguard!VguardPreCleanup:	 		   successfully flush cache, status=%x\n",
						0 ));
		}
    }

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

 
// fanghui 20101229
FLT_PREOP_CALLBACK_STATUS VguardPreClose (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext )
{   
    NTSTATUS status = STATUS_SUCCESS ;
	FLT_PREOP_CALLBACK_STATUS FltStatus = FLT_PREOP_SUCCESS_NO_CALLBACK ;

	HANDLE hFile = NULL ;
	PFILE_OBJECT FileObject = NULL ;

    BOOLEAN isDeleteCache;
    if ( !IsCurrentFileMonitored( Data, FltObjects )) {
		//leave;
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}

    LOG_PRINT( LOGFL_VGUARD,
		   ("Vguard!VguardPreClose:			   to flush cache, status=%x\n",
			0) );

	FileObject = Data->Iopb->TargetFileObject;
    	
	//flush cache and purge data in cache	
	Cc_ClearFileCache(FileObject, TRUE, NULL, 0) ;

    return FltStatus;
}







VOID ReadDriverParameters ( __in PUNICODE_STRING RegistryPath )
/*++
Func: read the driver-specific parameters from the registry.  
    These values will be found in the registry location indicated by the RegistryPath passed in.
Arguments:
    RegistryPath - the path key passed to the driver during driver entry.
--*/
{
    OBJECT_ATTRIBUTES attributes;
    HANDLE driverRegKey;
    NTSTATUS status;
    ULONG resultLength;
    UNICODE_STRING valueName;
    UCHAR buffer[sizeof( KEY_VALUE_PARTIAL_INFORMATION ) + sizeof( LONG )];

    //  If this value is not zero then somebody has already explicitly set it
    //  so don't override those settings.
    if (0 == LoggingFlags) {
        //  Open the desired registry key
        InitializeObjectAttributes( &attributes, RegistryPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );
        status = ZwOpenKey( &driverRegKey, KEY_READ, &attributes );
        if (!NT_SUCCESS( status )) {
            return;
        }

        // Read the given value from the registry.
        RtlInitUnicodeString( &valueName, L"DebugFlags" );
        status = ZwQueryValueKey( driverRegKey, 
			                      &valueName, 
			                      KeyValuePartialInformation, 
			                      buffer, 
			                      sizeof(buffer), 
			                      &resultLength );
        if (NT_SUCCESS( status )) {
            LoggingFlags = *((PULONG) &(((PKEY_VALUE_PARTIAL_INFORMATION)buffer)->Data));
        }

        ZwClose(driverRegKey); //  Close the registry entry
    }
}

