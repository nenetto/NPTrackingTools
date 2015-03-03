#ifndef OPTITRACK_MACROS_H
#define OPTITRACK_MACROS_H

#define BiiGOptitrackControlClassMacro(className,SuperClassName) \
typedef className        Self; \
typedef SuperClassName      Superclass; \
typedef itk::SmartPointer<Self> Pointer; \
typedef itk::SmartPointer<const Self>  ConstPointer; \
itkTypeMacro(className,SuperClassName)

#endif