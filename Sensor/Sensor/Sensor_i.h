

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Fri Aug 29 17:34:42 2014
 */
/* Compiler settings for Sensor.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __Sensor_i_h__
#define __Sensor_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IProcessSensor_FWD_DEFINED__
#define __IProcessSensor_FWD_DEFINED__
typedef interface IProcessSensor IProcessSensor;

#endif 	/* __IProcessSensor_FWD_DEFINED__ */


#ifndef __ProcessSensor_FWD_DEFINED__
#define __ProcessSensor_FWD_DEFINED__

#ifdef __cplusplus
typedef class ProcessSensor ProcessSensor;
#else
typedef struct ProcessSensor ProcessSensor;
#endif /* __cplusplus */

#endif 	/* __ProcessSensor_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IProcessSensor_INTERFACE_DEFINED__
#define __IProcessSensor_INTERFACE_DEFINED__

/* interface IProcessSensor */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IProcessSensor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("86C268DE-A3EC-4C77-9C5A-87B199310D21")
    IProcessSensor : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterProcess( 
            LONG ProcessID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnregisterProcess( 
            LONG ProcessID) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IProcessSensorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProcessSensor * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProcessSensor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProcessSensor * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IProcessSensor * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IProcessSensor * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IProcessSensor * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IProcessSensor * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterProcess )( 
            IProcessSensor * This,
            LONG ProcessID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterProcess )( 
            IProcessSensor * This,
            LONG ProcessID);
        
        END_INTERFACE
    } IProcessSensorVtbl;

    interface IProcessSensor
    {
        CONST_VTBL struct IProcessSensorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProcessSensor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProcessSensor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProcessSensor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProcessSensor_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IProcessSensor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IProcessSensor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IProcessSensor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IProcessSensor_RegisterProcess(This,ProcessID)	\
    ( (This)->lpVtbl -> RegisterProcess(This,ProcessID) ) 

#define IProcessSensor_UnregisterProcess(This,ProcessID)	\
    ( (This)->lpVtbl -> UnregisterProcess(This,ProcessID) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProcessSensor_INTERFACE_DEFINED__ */



#ifndef __SensorLib_LIBRARY_DEFINED__
#define __SensorLib_LIBRARY_DEFINED__

/* library SensorLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_SensorLib;

EXTERN_C const CLSID CLSID_ProcessSensor;

#ifdef __cplusplus

class DECLSPEC_UUID("AE1CCDC5-030D-450B-8FF1-2CF5D53CEB17")
ProcessSensor;
#endif
#endif /* __SensorLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


