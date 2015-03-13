#ifndef OPTITRACK_EXTRADEFINITIONS_H
#define OPTITRACK_EXTRADEFINITIONS_H

#include <itkMutexLockHolder.h>
#include <itkFastMutexLock.h>
#include <itkSmartPointer.h>

/**
* \brief MutexHolder to keep rest of Mutex
*/
typedef itk::MutexLockHolder<itk::FastMutexLock> MutexLockHolder;

/**
* \brief Different options for the result of the functions
*/
typedef enum
{
    FAILURE = 0,
    SUCCESS = 1
} ResultType;


#define BiiGOptitrackControlClassMacro(className,SuperClassName) \
typedef className        Self; \
typedef SuperClassName      Superclass; \
typedef itk::SmartPointer<Self> Pointer; \
typedef itk::SmartPointer<const Self>  ConstPointer; \
itkTypeMacro(className,SuperClassName)

#endif