
#ifndef __VLOG_H__
#define __VLOG_H__


/************************************************************************* Debug tracing information**********/
//  Definitions to display log messages.  The registry DWORD entry:
//  "hklm\system\CurrentControlSet\Services\Vguard\DebugFlags" defines
//  the default state of these logging flags
#define LOGFL_ERRORS    0x00000001  // if set, display error messages
#define LOGFL_READ      0x00000002  // if set, display READ operation info
#define LOGFL_WRITE     0x00000004  // if set, display WRITE operation info
#define LOGFL_DIRCTRL   0x00000008  // if set, display DIRCTRL operation info
#define LOGFL_VOLCTX    0x00000010  // if set, display VOLCTX operation info

#define LOGFL_VGUARD    0x00000020  // if set, display VGUARD related info. fanghui


extern ULONG LoggingFlags;   // defined in vguard.c


#define LOG_PRINT( _logFlag, _string )                              \
    (FlagOn(LoggingFlags,(_logFlag)) ?                              \
        DbgPrint _string  :                                         \
        ((int)0))


#endif /* __VLOG_H__ */

