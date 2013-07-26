// No header guard

#ifdef MAIDSAFE_WIN32
#  pragma warning(push)
#  pragma warning(disable: 4125) /* decimal digit terminates octal escape sequence */
#  pragma warning(disable: 4127) /* conditional expression is constant */
// All the following are copied from qglobal.h, QT_NO_WARNINGS section.
#  pragma warning(disable: 4097) /* typedef-name 'identifier1' used as synonym for class-name 'identifier2' */
#  pragma warning(disable: 4231) /* nonstandard extension used : 'extern' before template explicit instantiation */
#  pragma warning(disable: 4244) /* 'conversion' conversion from 'type1' to 'type2', possible loss of data */
#  pragma warning(disable: 4251) /* class 'A' needs to have dll interface for to be used by clients of class 'B'. */
#  pragma warning(disable: 4275) /* non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier' */
#  pragma warning(disable: 4355) /* 'this' : used in base member initializer list */
#  pragma warning(disable: 4514) /* unreferenced inline/local function has been removed */
#  pragma warning(disable: 4530) /* C++ exception handler used, but unwind semantics are not enabled. Specify -GX */
#  pragma warning(disable: 4660) /* template-class specialization 'identifier' is already instantiated */
#  pragma warning(disable: 4706) /* assignment within conditional expression */
#  pragma warning(disable: 4710) /* function not inlined */
#  pragma warning(disable: 4786) /* truncating debug info after 255 characters */
#  pragma warning(disable: 4800) /* 'type' : forcing value to bool 'true' or 'false' (performance warning) */
#endif

#ifdef MAIDSAFE_APPLE
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#ifdef MAIDSAFE_LINUX
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Weffc++"
#ifndef __clang__
#  pragma GCC diagnostic ignored "-pedantic"
#endif
#  pragma GCC diagnostic ignored "-Wstrict-overflow"
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

// Add all required Qt headers here

#include "QtCore/QFile"
#include "QtWidgets/QApplication"
#include "QtWidgets/QMainWindow"
#include "QtWidgets/QMessageBox"
