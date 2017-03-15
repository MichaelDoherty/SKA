//-----------------------------------------------------------------------------
// SystemConfiguration.h
//    Various flags and variables to configure system, such as:
//    - Flags to control compilation of operating system dependent code.
//    - Flags to enable code dependent on external libraries.
//-----------------------------------------------------------------------------
// This software is part of the Skeleton Animation Toolkit (SKA) developed 
// at the University of the Pacific, under the guidance of Michael Doherty.
// For information please contact mdoherty@pacific.edu.
//-----------------------------------------------------------------------------
// This is open software. You are free to use it as you see fit.
// The University of the Pacific and identified authors would appreciate
// being credited for any significant use, particularly if used for
// commercial projects or academic research publications.
//-----------------------------------------------------------------------------

#ifndef SYSTEMCONFIGURATION_DOT_H
#define SYSTEMCONFIGURATION_DOT_H

// Version identification for this SKA library
#define SKA_VERSION "4.0"
#define SKA_MAJOR_VERSION_NUMBER 4
#define SKA_MINOR_VERSION_NUMBER 0

// Operating system and compiler checks
// _WIN32 - Windows 32 bit or 64 bit
// _MSC_VER - Microsoft Visual C++ version
//
// __APPLE__ or __MACH__ - Apple OSX
// __linux__ or __linux - some linux
// __unix__ or __unix - some unix
// __GNUC__ - GNU C++ compiler (major) version

// Determine how to handle library declaration specifications (SKA_LIB_DECLSPEC)
// SKA_LIB_BUILD should be set as a compiler flag when building a Windows DLL
#if defined(_MSC_VER)
#if defined(SKA_LIB_BUILD)
#define SKA_LIB_DECLSPEC __declspec(dllexport)  // Windows DLL export (for the library)
#else
#define SKA_LIB_DECLSPEC __declspec(dllimport)  // Windows DLL import (for the application)
#endif
#else
#define SKA_LIB_DECLSPEC                        // non-Windows, ignore DECLSPEC
#endif

// Disable MS Visual C++ warnings about unsafe standard C string functions
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// ENABLE_FFTW: Enable code that depends on FFTW library.
//   Enabling this code requires installation of
//   "Fastest Fourier Transform in the West" (www.fftw.org)
//   Requires including <fftw3.h> (see FFT.cpp)
//   Requires linking to libfftw3-3.lib
// 0 = disable FFTW
// 1 = enable FFTW
//   This flag can be overridden with a compiler flag.
#ifndef ENABLE_FFTW
#define ENABLE_FFTW 0
#endif

#endif

