// This file contains helper functions that are automatically created from
// templates.

#include "nuitka/prelude.h"

extern PyObject *callPythonFunction( PyObject *func, PyObject **args, int count );


PyObject *CALL_FUNCTION_WITH_ARGS1(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 1; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 1 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 1; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 1 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 1 * sizeof(PyObject *));
            memcpy(python_pars + 1, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 1)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 1 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 1; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 1 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 1 * sizeof(PyObject *));
                memcpy(python_pars+1 + 1, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 1)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 1 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (1 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 1 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (1 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 1);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 1, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 1);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            1
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 1);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS2(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 2; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 2 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 2; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 2 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 2 * sizeof(PyObject *));
            memcpy(python_pars + 2, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 2)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 2 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 2; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 2 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 2 * sizeof(PyObject *));
                memcpy(python_pars+1 + 2, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 2)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 2 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (2 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 2 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (2 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 2);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 2, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 2);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            2
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 2);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS3(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 3; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 3 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 3; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 3 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 3 * sizeof(PyObject *));
            memcpy(python_pars + 3, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 3)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 3 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 3; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 3 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 3 * sizeof(PyObject *));
                memcpy(python_pars+1 + 3, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 3)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 3 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (3 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 3 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (3 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 3);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 3, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 3);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            3
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 3);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS4(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 4; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 4 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 4; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 4 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 4 * sizeof(PyObject *));
            memcpy(python_pars + 4, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 4)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 4 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 4; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 4 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 4 * sizeof(PyObject *));
                memcpy(python_pars+1 + 4, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 4)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 4 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (4 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 4 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (4 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 4);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 4, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 4);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            4
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 4);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS5(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 5; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 5 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 5; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 5 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 5 * sizeof(PyObject *));
            memcpy(python_pars + 5, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 5)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 5 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 5; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 5 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 5 * sizeof(PyObject *));
                memcpy(python_pars+1 + 5, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 5)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 5 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (5 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 5 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (5 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 5);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 5, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 5);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            5
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 5);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_METHOD_WITH_ARGS1(PyObject *source, PyObject *attr_name, PyObject **args) {
    CHECK_OBJECT(source);
    CHECK_OBJECT(attr_name);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 1; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    PyTypeObject *type = Py_TYPE(source);

    if (type->tp_getattro == PyObject_GenericGetAttr) {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely(type->tp_dict == NULL)) {
            if (unlikely(PyType_Ready(type) < 0)) {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup(type, attr_name);
        descrgetfunc func = NULL;

        if (descr != NULL)
        {
            Py_INCREF(descr);

#if PYTHON_VERSION < 300
            if (PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_HAVE_CLASS)) {
#endif
                func = Py_TYPE(descr)->tp_descr_get;

                if (func != NULL && PyDescr_IsData(descr))
                {
                    PyObject *called_object = func(descr, source, (PyObject *)type);
                    Py_DECREF(descr);

                    PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                        called_object,
                        args
                    );
                    Py_DECREF(called_object);
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if (dict != NULL)
        {
            CHECK_OBJECT(dict);

            Py_INCREF(dict);

            PyObject *called_object = PyDict_GetItem(dict, attr_name);

            if (called_object != NULL)
            {
                Py_INCREF(called_object);
                Py_XDECREF(descr);
                Py_DECREF(dict);

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF(called_object);
                return result;
            }

            Py_DECREF(dict);
        }

        if (func != NULL) {
            if (func == Nuitka_Function_Type.tp_descr_get) {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    1
                );

                Py_DECREF(descr);

                return result;
            } else {
                PyObject *called_object = func(descr, source, (PyObject *)type);
                CHECK_OBJECT(called_object);

                Py_DECREF(descr);

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF(called_object);

                return result;
            }
        }

        if (descr != NULL) {
            CHECK_OBJECT(descr);

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                descr,
                args
            );
            Py_DECREF(descr);

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if (type == &PyInstance_Type) {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert(attr_name != const_str_plain___dict__);
        assert(attr_name != const_str_plain___class__);

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            return CALL_FUNCTION_WITH_ARGS1(called_object, args);
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            descrgetfunc descr_get = Py_TYPE(called_object)->tp_descr_get;

            if (descr_get == Nuitka_Function_Type.tp_descr_get) {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    1
                );
            } else if (descr_get != NULL) {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely(method == NULL)) {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(method, args);
                Py_DECREF(method);
                return result;
            } else {
                return CALL_FUNCTION_WITH_ARGS1(called_object, args);
            }

        } else if (unlikely(source_instance->in_class->cl_getattr == NULL)) {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        } else {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely(called_object == NULL))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                called_object,
                args
            );
            Py_DECREF(called_object);
            return result;
        }
    }
#endif
    else if (type->tp_getattro != NULL) {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely(called_object == NULL)) {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else if (type->tp_getattr != NULL) {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked(attr_name)
        );

        if (unlikely(called_object == NULL))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked(attr_name)
        );

        return NULL;
    }
}
/* Code to register embedded modules for meta path based loading if any. */

#include "nuitka/unfreezing.h"

/* Table for lookup to find compiled or bytecode modules included in this
 * binary or module, or put along this binary as extension modules. We do
 * our own loading for each of these.
 */
extern PyObject *modulecode___main__(PyObject *);
static struct Nuitka_MetaPathBasedLoaderEntry meta_path_loader_entries[] = {
    {"__main__", modulecode___main__, 0, 0, },
    {"_asyncio", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_bz2", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_cn", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_hk", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_iso2022", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_jp", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_kr", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_codecs_tw", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_contextvars", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_crypt", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_csv", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_ctypes", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_curses", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_curses_panel", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_dbm", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_decimal", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_hashlib", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_json", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_lsprof", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_lzma", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_multibytecodec", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_multiprocessing", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_opcode", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_queue", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_sqlite3", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_ssl", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_tkinter", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_uuid", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"audioop", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"mmap", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"operator", NULL, 1341, 13866, NUITKA_BYTECODE_FLAG},
    {"os", NULL, 15207, 29659, NUITKA_BYTECODE_FLAG},
    {"readline", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"site", NULL, 44866, 13430, NUITKA_BYTECODE_FLAG},
    {"termios", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"types", NULL, 58296, 8942, NUITKA_BYTECODE_FLAG},
    {"__future__", NULL, 67238, 4098, NUITKA_BYTECODE_FLAG},
    {"_bootlocale", NULL, 71336, 1215, NUITKA_BYTECODE_FLAG},
    {"_collections_abc", NULL, 72551, 28908, NUITKA_BYTECODE_FLAG},
    {"_compat_pickle", NULL, 101459, 5774, NUITKA_BYTECODE_FLAG},
    {"_compression", NULL, 107233, 4090, NUITKA_BYTECODE_FLAG},
    {"_dummy_thread", NULL, 111323, 4819, NUITKA_BYTECODE_FLAG},
    {"_markupbase", NULL, 116142, 7751, NUITKA_BYTECODE_FLAG},
    {"_osx_support", NULL, 123893, 9544, NUITKA_BYTECODE_FLAG},
    {"_py_abc", NULL, 133437, 4620, NUITKA_BYTECODE_FLAG},
    {"_pyio", NULL, 138057, 72784, NUITKA_BYTECODE_FLAG},
    {"_sitebuiltins", NULL, 210841, 3431, NUITKA_BYTECODE_FLAG},
    {"_strptime", NULL, 214272, 16070, NUITKA_BYTECODE_FLAG},
    {"_sysconfigdata_m_linux_arm-linux-gnueabihf", NULL, 230342, 20194, NUITKA_BYTECODE_FLAG},
    {"_threading_local", NULL, 250536, 6378, NUITKA_BYTECODE_FLAG},
    {"_weakrefset", NULL, 256914, 7428, NUITKA_BYTECODE_FLAG},
    {"abc", NULL, 264342, 6417, NUITKA_BYTECODE_FLAG},
    {"aifc", NULL, 270759, 26109, NUITKA_BYTECODE_FLAG},
    {"argparse", NULL, 296868, 62145, NUITKA_BYTECODE_FLAG},
    {"ast", NULL, 359013, 12046, NUITKA_BYTECODE_FLAG},
    {"asynchat", NULL, 371059, 6800, NUITKA_BYTECODE_FLAG},
    {"asyncio", NULL, 377859, 656, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"asyncio.base_events", NULL, 378515, 48082, NUITKA_BYTECODE_FLAG},
    {"asyncio.base_futures", NULL, 426597, 2068, NUITKA_BYTECODE_FLAG},
    {"asyncio.base_subprocess", NULL, 428665, 9157, NUITKA_BYTECODE_FLAG},
    {"asyncio.base_tasks", NULL, 437822, 1832, NUITKA_BYTECODE_FLAG},
    {"asyncio.constants", NULL, 439654, 557, NUITKA_BYTECODE_FLAG},
    {"asyncio.coroutines", NULL, 440211, 6344, NUITKA_BYTECODE_FLAG},
    {"asyncio.events", NULL, 446555, 27821, NUITKA_BYTECODE_FLAG},
    {"asyncio.format_helpers", NULL, 474376, 2283, NUITKA_BYTECODE_FLAG},
    {"asyncio.futures", NULL, 476659, 10711, NUITKA_BYTECODE_FLAG},
    {"asyncio.locks", NULL, 487370, 15879, NUITKA_BYTECODE_FLAG},
    {"asyncio.log", NULL, 503249, 206, NUITKA_BYTECODE_FLAG},
    {"asyncio.proactor_events", NULL, 503455, 20077, NUITKA_BYTECODE_FLAG},
    {"asyncio.protocols", NULL, 523532, 8694, NUITKA_BYTECODE_FLAG},
    {"asyncio.queues", NULL, 532226, 8139, NUITKA_BYTECODE_FLAG},
    {"asyncio.runners", NULL, 540365, 1880, NUITKA_BYTECODE_FLAG},
    {"asyncio.selector_events", NULL, 542245, 28089, NUITKA_BYTECODE_FLAG},
    {"asyncio.sslproto", NULL, 570334, 21177, NUITKA_BYTECODE_FLAG},
    {"asyncio.streams", NULL, 591511, 20254, NUITKA_BYTECODE_FLAG},
    {"asyncio.subprocess", NULL, 611765, 6718, NUITKA_BYTECODE_FLAG},
    {"asyncio.tasks", NULL, 618483, 21832, NUITKA_BYTECODE_FLAG},
    {"asyncio.transports", NULL, 640315, 12177, NUITKA_BYTECODE_FLAG},
    {"asyncio.unix_events", NULL, 652492, 32072, NUITKA_BYTECODE_FLAG},
    {"asyncore", NULL, 684564, 15810, NUITKA_BYTECODE_FLAG},
    {"base64", NULL, 700374, 16954, NUITKA_BYTECODE_FLAG},
    {"bdb", NULL, 717328, 24360, NUITKA_BYTECODE_FLAG},
    {"binhex", NULL, 741688, 12025, NUITKA_BYTECODE_FLAG},
    {"bisect", NULL, 753713, 2664, NUITKA_BYTECODE_FLAG},
    {"bz2", NULL, 756377, 11147, NUITKA_BYTECODE_FLAG},
    {"cProfile", NULL, 767524, 4517, NUITKA_BYTECODE_FLAG},
    {"calendar", NULL, 772041, 27390, NUITKA_BYTECODE_FLAG},
    {"cgi", NULL, 799431, 27157, NUITKA_BYTECODE_FLAG},
    {"cgitb", NULL, 826588, 10078, NUITKA_BYTECODE_FLAG},
    {"chunk", NULL, 836666, 4885, NUITKA_BYTECODE_FLAG},
    {"cmd", NULL, 841551, 12556, NUITKA_BYTECODE_FLAG},
    {"code", NULL, 854107, 9824, NUITKA_BYTECODE_FLAG},
    {"codecs", NULL, 863931, 33864, NUITKA_BYTECODE_FLAG},
    {"codeop", NULL, 897795, 6256, NUITKA_BYTECODE_FLAG},
    {"collections", NULL, 904051, 46576, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"collections.abc", NULL, 72551, 28908, NUITKA_BYTECODE_FLAG},
    {"colorsys", NULL, 950627, 3263, NUITKA_BYTECODE_FLAG},
    {"compileall", NULL, 953890, 9072, NUITKA_BYTECODE_FLAG},
    {"concurrent", NULL, 962962, 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"concurrent.futures", NULL, 963074, 1052, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"concurrent.futures._base", NULL, 964126, 20849, NUITKA_BYTECODE_FLAG},
    {"concurrent.futures.process", NULL, 984975, 19527, NUITKA_BYTECODE_FLAG},
    {"concurrent.futures.thread", NULL, 1004502, 5140, NUITKA_BYTECODE_FLAG},
    {"configparser", NULL, 1009642, 45846, NUITKA_BYTECODE_FLAG},
    {"contextlib", NULL, 1055488, 19866, NUITKA_BYTECODE_FLAG},
    {"contextvars", NULL, 1075354, 223, NUITKA_BYTECODE_FLAG},
    {"copy", NULL, 1075577, 7067, NUITKA_BYTECODE_FLAG},
    {"copyreg", NULL, 1082644, 4210, NUITKA_BYTECODE_FLAG},
    {"crypt", NULL, 1086854, 3100, NUITKA_BYTECODE_FLAG},
    {"csv", NULL, 1089954, 11798, NUITKA_BYTECODE_FLAG},
    {"ctypes", NULL, 1101752, 16115, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"ctypes._aix", NULL, 1117867, 9789, NUITKA_BYTECODE_FLAG},
    {"ctypes._endian", NULL, 1127656, 1922, NUITKA_BYTECODE_FLAG},
    {"ctypes.util", NULL, 1129578, 7785, NUITKA_BYTECODE_FLAG},
    {"curses", NULL, 1137363, 1761, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"curses.ascii", NULL, 1139124, 3869, NUITKA_BYTECODE_FLAG},
    {"curses.has_key", NULL, 1142993, 4233, NUITKA_BYTECODE_FLAG},
    {"curses.panel", NULL, 1147226, 203, NUITKA_BYTECODE_FLAG},
    {"curses.textpad", NULL, 1147429, 5872, NUITKA_BYTECODE_FLAG},
    {"dataclasses", NULL, 1153301, 22431, NUITKA_BYTECODE_FLAG},
    {"datetime", NULL, 1175732, 57215, NUITKA_BYTECODE_FLAG},
    {"dbm", NULL, 1232947, 4132, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"dbm.dumb", NULL, 1237079, 8366, NUITKA_BYTECODE_FLAG},
    {"dbm.ndbm", NULL, 1245445, 182, NUITKA_BYTECODE_FLAG},
    {"decimal", NULL, 1245627, 162138, NUITKA_BYTECODE_FLAG},
    {"difflib", NULL, 1407765, 59404, NUITKA_BYTECODE_FLAG},
    {"dis", NULL, 1467169, 15171, NUITKA_BYTECODE_FLAG},
    {"distutils", NULL, 1482340, 364, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"distutils.archive_util", NULL, 1482704, 6498, NUITKA_BYTECODE_FLAG},
    {"distutils.bcppcompiler", NULL, 1489202, 6467, NUITKA_BYTECODE_FLAG},
    {"distutils.ccompiler", NULL, 1495669, 33182, NUITKA_BYTECODE_FLAG},
    {"distutils.cmd", NULL, 1528851, 13874, NUITKA_BYTECODE_FLAG},
    {"distutils.command", NULL, 1542725, 521, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"distutils.command.bdist", NULL, 1543246, 3620, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_dumb", NULL, 1546866, 3535, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_rpm", NULL, 1550401, 12460, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_wininst", NULL, 1562861, 8135, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build", NULL, 1570996, 3801, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_clib", NULL, 1574797, 4851, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_ext", NULL, 1579648, 15770, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_py", NULL, 1595418, 10357, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_scripts", NULL, 1605775, 4256, NUITKA_BYTECODE_FLAG},
    {"distutils.command.check", NULL, 1610031, 4776, NUITKA_BYTECODE_FLAG},
    {"distutils.command.clean", NULL, 1614807, 2058, NUITKA_BYTECODE_FLAG},
    {"distutils.command.config", NULL, 1616865, 10162, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install", NULL, 1627027, 14527, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_data", NULL, 1641554, 2251, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_egg_info", NULL, 1643805, 3359, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_headers", NULL, 1647164, 1666, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_lib", NULL, 1648830, 5164, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_scripts", NULL, 1653994, 2108, NUITKA_BYTECODE_FLAG},
    {"distutils.command.register", NULL, 1656102, 8443, NUITKA_BYTECODE_FLAG},
    {"distutils.command.sdist", NULL, 1664545, 14474, NUITKA_BYTECODE_FLAG},
    {"distutils.command.upload", NULL, 1679019, 5056, NUITKA_BYTECODE_FLAG},
    {"distutils.config", NULL, 1684075, 3451, NUITKA_BYTECODE_FLAG},
    {"distutils.core", NULL, 1687526, 6576, NUITKA_BYTECODE_FLAG},
    {"distutils.cygwinccompiler", NULL, 1694102, 8494, NUITKA_BYTECODE_FLAG},
    {"distutils.debug", NULL, 1702596, 174, NUITKA_BYTECODE_FLAG},
    {"distutils.dep_util", NULL, 1702770, 2690, NUITKA_BYTECODE_FLAG},
    {"distutils.dir_util", NULL, 1705460, 6140, NUITKA_BYTECODE_FLAG},
    {"distutils.dist", NULL, 1711600, 34405, NUITKA_BYTECODE_FLAG},
    {"distutils.errors", NULL, 1746005, 5460, NUITKA_BYTECODE_FLAG},
    {"distutils.extension", NULL, 1751465, 6871, NUITKA_BYTECODE_FLAG},
    {"distutils.fancy_getopt", NULL, 1758336, 10583, NUITKA_BYTECODE_FLAG},
    {"distutils.file_util", NULL, 1768919, 5869, NUITKA_BYTECODE_FLAG},
    {"distutils.filelist", NULL, 1774788, 9804, NUITKA_BYTECODE_FLAG},
    {"distutils.log", NULL, 1784592, 2285, NUITKA_BYTECODE_FLAG},
    {"distutils.msvccompiler", NULL, 1786877, 14537, NUITKA_BYTECODE_FLAG},
    {"distutils.spawn", NULL, 1801414, 4975, NUITKA_BYTECODE_FLAG},
    {"distutils.sysconfig", NULL, 1806389, 12666, NUITKA_BYTECODE_FLAG},
    {"distutils.text_file", NULL, 1819055, 8411, NUITKA_BYTECODE_FLAG},
    {"distutils.unixccompiler", NULL, 1827466, 6971, NUITKA_BYTECODE_FLAG},
    {"distutils.util", NULL, 1834437, 15067, NUITKA_BYTECODE_FLAG},
    {"distutils.version", NULL, 1849504, 7322, NUITKA_BYTECODE_FLAG},
    {"distutils.versionpredicate", NULL, 1856826, 5069, NUITKA_BYTECODE_FLAG},
    {"doctest", NULL, 1861895, 75400, NUITKA_BYTECODE_FLAG},
    {"dummy_threading", NULL, 1937295, 1090, NUITKA_BYTECODE_FLAG},
    {"email", NULL, 1938385, 1657, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"email._encoded_words", NULL, 1940042, 5574, NUITKA_BYTECODE_FLAG},
    {"email._header_value_parser", NULL, 1945616, 75647, NUITKA_BYTECODE_FLAG},
    {"email._parseaddr", NULL, 2021263, 12371, NUITKA_BYTECODE_FLAG},
    {"email._policybase", NULL, 2033634, 14816, NUITKA_BYTECODE_FLAG},
    {"email.base64mime", NULL, 2048450, 3201, NUITKA_BYTECODE_FLAG},
    {"email.charset", NULL, 2051651, 11495, NUITKA_BYTECODE_FLAG},
    {"email.contentmanager", NULL, 2063146, 7261, NUITKA_BYTECODE_FLAG},
    {"email.encoders", NULL, 2070407, 1630, NUITKA_BYTECODE_FLAG},
    {"email.errors", NULL, 2072037, 6157, NUITKA_BYTECODE_FLAG},
    {"email.feedparser", NULL, 2078194, 10591, NUITKA_BYTECODE_FLAG},
    {"email.generator", NULL, 2088785, 12466, NUITKA_BYTECODE_FLAG},
    {"email.header", NULL, 2101251, 16346, NUITKA_BYTECODE_FLAG},
    {"email.headerregistry", NULL, 2117597, 21263, NUITKA_BYTECODE_FLAG},
    {"email.iterators", NULL, 2138860, 1898, NUITKA_BYTECODE_FLAG},
    {"email.message", NULL, 2140758, 37906, NUITKA_BYTECODE_FLAG},
    {"email.mime", NULL, 2178664, 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"email.mime.application", NULL, 2178776, 1423, NUITKA_BYTECODE_FLAG},
    {"email.mime.audio", NULL, 2180199, 2582, NUITKA_BYTECODE_FLAG},
    {"email.mime.base", NULL, 2182781, 1048, NUITKA_BYTECODE_FLAG},
    {"email.mime.image", NULL, 2183829, 1868, NUITKA_BYTECODE_FLAG},
    {"email.mime.message", NULL, 2185697, 1297, NUITKA_BYTECODE_FLAG},
    {"email.mime.multipart", NULL, 2186994, 1519, NUITKA_BYTECODE_FLAG},
    {"email.mime.nonmultipart", NULL, 2188513, 734, NUITKA_BYTECODE_FLAG},
    {"email.mime.text", NULL, 2189247, 1281, NUITKA_BYTECODE_FLAG},
    {"email.parser", NULL, 2190528, 5713, NUITKA_BYTECODE_FLAG},
    {"email.policy", NULL, 2196241, 9606, NUITKA_BYTECODE_FLAG},
    {"email.quoprimime", NULL, 2205847, 7630, NUITKA_BYTECODE_FLAG},
    {"email.utils", NULL, 2213477, 9433, NUITKA_BYTECODE_FLAG},
    {"encodings", NULL, 2222910, 3913, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"encodings.aliases", NULL, 2226823, 6262, NUITKA_BYTECODE_FLAG},
    {"encodings.ascii", NULL, 2233085, 1850, NUITKA_BYTECODE_FLAG},
    {"encodings.base64_codec", NULL, 2234935, 2389, NUITKA_BYTECODE_FLAG},
    {"encodings.big5", NULL, 2237324, 1410, NUITKA_BYTECODE_FLAG},
    {"encodings.big5hkscs", NULL, 2238734, 1420, NUITKA_BYTECODE_FLAG},
    {"encodings.bz2_codec", NULL, 2240154, 3251, NUITKA_BYTECODE_FLAG},
    {"encodings.charmap", NULL, 2243405, 2903, NUITKA_BYTECODE_FLAG},
    {"encodings.cp037", NULL, 2246308, 2395, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1006", NULL, 2248703, 2471, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1026", NULL, 2251174, 2399, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1125", NULL, 2253573, 8092, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1140", NULL, 2261665, 2385, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1250", NULL, 2264050, 2422, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1251", NULL, 2266472, 2419, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1252", NULL, 2268891, 2422, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1253", NULL, 2271313, 2435, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1254", NULL, 2273748, 2424, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1255", NULL, 2276172, 2443, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1256", NULL, 2278615, 2421, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1257", NULL, 2281036, 2429, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1258", NULL, 2283465, 2427, NUITKA_BYTECODE_FLAG},
    {"encodings.cp273", NULL, 2285892, 2381, NUITKA_BYTECODE_FLAG},
    {"encodings.cp424", NULL, 2288273, 2425, NUITKA_BYTECODE_FLAG},
    {"encodings.cp437", NULL, 2290698, 7809, NUITKA_BYTECODE_FLAG},
    {"encodings.cp500", NULL, 2298507, 2395, NUITKA_BYTECODE_FLAG},
    {"encodings.cp720", NULL, 2300902, 2492, NUITKA_BYTECODE_FLAG},
    {"encodings.cp737", NULL, 2303394, 8131, NUITKA_BYTECODE_FLAG},
    {"encodings.cp775", NULL, 2311525, 7839, NUITKA_BYTECODE_FLAG},
    {"encodings.cp850", NULL, 2319364, 7470, NUITKA_BYTECODE_FLAG},
    {"encodings.cp852", NULL, 2326834, 7847, NUITKA_BYTECODE_FLAG},
    {"encodings.cp855", NULL, 2334681, 8100, NUITKA_BYTECODE_FLAG},
    {"encodings.cp856", NULL, 2342781, 2457, NUITKA_BYTECODE_FLAG},
    {"encodings.cp857", NULL, 2345238, 7452, NUITKA_BYTECODE_FLAG},
    {"encodings.cp858", NULL, 2352690, 7440, NUITKA_BYTECODE_FLAG},
    {"encodings.cp860", NULL, 2360130, 7788, NUITKA_BYTECODE_FLAG},
    {"encodings.cp861", NULL, 2367918, 7803, NUITKA_BYTECODE_FLAG},
    {"encodings.cp862", NULL, 2375721, 7992, NUITKA_BYTECODE_FLAG},
    {"encodings.cp863", NULL, 2383713, 7803, NUITKA_BYTECODE_FLAG},
    {"encodings.cp864", NULL, 2391516, 7949, NUITKA_BYTECODE_FLAG},
    {"encodings.cp865", NULL, 2399465, 7803, NUITKA_BYTECODE_FLAG},
    {"encodings.cp866", NULL, 2407268, 8136, NUITKA_BYTECODE_FLAG},
    {"encodings.cp869", NULL, 2415404, 7829, NUITKA_BYTECODE_FLAG},
    {"encodings.cp874", NULL, 2423233, 2523, NUITKA_BYTECODE_FLAG},
    {"encodings.cp875", NULL, 2425756, 2392, NUITKA_BYTECODE_FLAG},
    {"encodings.cp932", NULL, 2428148, 1412, NUITKA_BYTECODE_FLAG},
    {"encodings.cp949", NULL, 2429560, 1412, NUITKA_BYTECODE_FLAG},
    {"encodings.cp950", NULL, 2430972, 1412, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_jis_2004", NULL, 2432384, 1426, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_jisx0213", NULL, 2433810, 1426, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_jp", NULL, 2435236, 1414, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_kr", NULL, 2436650, 1414, NUITKA_BYTECODE_FLAG},
    {"encodings.gb18030", NULL, 2438064, 1416, NUITKA_BYTECODE_FLAG},
    {"encodings.gb2312", NULL, 2439480, 1414, NUITKA_BYTECODE_FLAG},
    {"encodings.gbk", NULL, 2440894, 1408, NUITKA_BYTECODE_FLAG},
    {"encodings.hex_codec", NULL, 2442302, 2376, NUITKA_BYTECODE_FLAG},
    {"encodings.hp_roman8", NULL, 2444678, 2596, NUITKA_BYTECODE_FLAG},
    {"encodings.hz", NULL, 2447274, 1406, NUITKA_BYTECODE_FLAG},
    {"encodings.idna", NULL, 2448680, 5690, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp", NULL, 2454370, 1427, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_1", NULL, 2455797, 1431, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_2", NULL, 2457228, 1431, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_2004", NULL, 2458659, 1437, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_3", NULL, 2460096, 1431, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_ext", NULL, 2461527, 1435, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_kr", NULL, 2462962, 1427, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_1", NULL, 2464389, 2394, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_10", NULL, 2466783, 2399, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_11", NULL, 2469182, 2493, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_13", NULL, 2471675, 2402, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_14", NULL, 2474077, 2420, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_15", NULL, 2476497, 2399, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_16", NULL, 2478896, 2401, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_2", NULL, 2481297, 2394, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_3", NULL, 2483691, 2401, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_4", NULL, 2486092, 2394, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_5", NULL, 2488486, 2395, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_6", NULL, 2490881, 2439, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_7", NULL, 2493320, 2402, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_8", NULL, 2495722, 2433, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_9", NULL, 2498155, 2394, NUITKA_BYTECODE_FLAG},
    {"encodings.johab", NULL, 2500549, 1412, NUITKA_BYTECODE_FLAG},
    {"encodings.koi8_r", NULL, 2501961, 2446, NUITKA_BYTECODE_FLAG},
    {"encodings.koi8_t", NULL, 2504407, 2357, NUITKA_BYTECODE_FLAG},
    {"encodings.koi8_u", NULL, 2506764, 2432, NUITKA_BYTECODE_FLAG},
    {"encodings.kz1048", NULL, 2509196, 2409, NUITKA_BYTECODE_FLAG},
    {"encodings.latin_1", NULL, 2511605, 1862, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_arabic", NULL, 2513467, 7703, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_centeuro", NULL, 2521170, 2433, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_croatian", NULL, 2523603, 2441, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_cyrillic", NULL, 2526044, 2431, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_farsi", NULL, 2528475, 2375, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_greek", NULL, 2530850, 2415, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_iceland", NULL, 2533265, 2434, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_latin2", NULL, 2535699, 2575, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_roman", NULL, 2538274, 2432, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_romanian", NULL, 2540706, 2442, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_turkish", NULL, 2543148, 2435, NUITKA_BYTECODE_FLAG},
    {"encodings.palmos", NULL, 2545583, 2422, NUITKA_BYTECODE_FLAG},
    {"encodings.ptcp154", NULL, 2548005, 2516, NUITKA_BYTECODE_FLAG},
    {"encodings.punycode", NULL, 2550521, 6383, NUITKA_BYTECODE_FLAG},
    {"encodings.quopri_codec", NULL, 2556904, 2409, NUITKA_BYTECODE_FLAG},
    {"encodings.raw_unicode_escape", NULL, 2559313, 1735, NUITKA_BYTECODE_FLAG},
    {"encodings.rot_13", NULL, 2561048, 2995, NUITKA_BYTECODE_FLAG},
    {"encodings.shift_jis", NULL, 2564043, 1420, NUITKA_BYTECODE_FLAG},
    {"encodings.shift_jis_2004", NULL, 2565463, 1430, NUITKA_BYTECODE_FLAG},
    {"encodings.shift_jisx0213", NULL, 2566893, 1430, NUITKA_BYTECODE_FLAG},
    {"encodings.tis_620", NULL, 2568323, 2484, NUITKA_BYTECODE_FLAG},
    {"encodings.undefined", NULL, 2570807, 2129, NUITKA_BYTECODE_FLAG},
    {"encodings.unicode_escape", NULL, 2572936, 1715, NUITKA_BYTECODE_FLAG},
    {"encodings.unicode_internal", NULL, 2574651, 1725, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_16", NULL, 2576376, 4799, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_16_be", NULL, 2581175, 1600, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_16_le", NULL, 2582775, 1600, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_32", NULL, 2584375, 4692, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_32_be", NULL, 2589067, 1493, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_32_le", NULL, 2590560, 1493, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_7", NULL, 2592053, 1521, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_8", NULL, 2593574, 1580, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_8_sig", NULL, 2595154, 4482, NUITKA_BYTECODE_FLAG},
    {"encodings.uu_codec", NULL, 2599636, 3191, NUITKA_BYTECODE_FLAG},
    {"encodings.zlib_codec", NULL, 2602827, 3089, NUITKA_BYTECODE_FLAG},
    {"enum", NULL, 2605916, 24237, NUITKA_BYTECODE_FLAG},
    {"filecmp", NULL, 2630153, 8273, NUITKA_BYTECODE_FLAG},
    {"fileinput", NULL, 2638426, 13164, NUITKA_BYTECODE_FLAG},
    {"fnmatch", NULL, 2651590, 3303, NUITKA_BYTECODE_FLAG},
    {"formatter", NULL, 2654893, 17519, NUITKA_BYTECODE_FLAG},
    {"fractions", NULL, 2672412, 18394, NUITKA_BYTECODE_FLAG},
    {"ftplib", NULL, 2690806, 28033, NUITKA_BYTECODE_FLAG},
    {"functools", NULL, 2718839, 23921, NUITKA_BYTECODE_FLAG},
    {"genericpath", NULL, 2742760, 3714, NUITKA_BYTECODE_FLAG},
    {"getopt", NULL, 2746474, 6205, NUITKA_BYTECODE_FLAG},
    {"getpass", NULL, 2752679, 4130, NUITKA_BYTECODE_FLAG},
    {"gettext", NULL, 2756809, 14241, NUITKA_BYTECODE_FLAG},
    {"glob", NULL, 2771050, 4225, NUITKA_BYTECODE_FLAG},
    {"gzip", NULL, 2775275, 17151, NUITKA_BYTECODE_FLAG},
    {"hashlib", NULL, 2792426, 6557, NUITKA_BYTECODE_FLAG},
    {"heapq", NULL, 2798983, 14328, NUITKA_BYTECODE_FLAG},
    {"hmac", NULL, 2813311, 6079, NUITKA_BYTECODE_FLAG},
    {"html", NULL, 2819390, 3363, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"html.entities", NULL, 2822753, 50435, NUITKA_BYTECODE_FLAG},
    {"html.parser", NULL, 2873188, 11073, NUITKA_BYTECODE_FLAG},
    {"http", NULL, 2884261, 5888, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"http.client", NULL, 2890149, 34210, NUITKA_BYTECODE_FLAG},
    {"http.cookiejar", NULL, 2924359, 53551, NUITKA_BYTECODE_FLAG},
    {"http.cookies", NULL, 2977910, 15210, NUITKA_BYTECODE_FLAG},
    {"http.server", NULL, 2993120, 33333, NUITKA_BYTECODE_FLAG},
    {"imaplib", NULL, 3026453, 41396, NUITKA_BYTECODE_FLAG},
    {"imghdr", NULL, 3067849, 4108, NUITKA_BYTECODE_FLAG},
    {"imp", NULL, 3071957, 9719, NUITKA_BYTECODE_FLAG},
    {"importlib", NULL, 3081676, 3698, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"importlib._bootstrap", NULL, 3085374, 29144, NUITKA_BYTECODE_FLAG},
    {"importlib._bootstrap_external", NULL, 3114518, 41784, NUITKA_BYTECODE_FLAG},
    {"importlib.abc", NULL, 3156302, 13453, NUITKA_BYTECODE_FLAG},
    {"importlib.machinery", NULL, 3169755, 938, NUITKA_BYTECODE_FLAG},
    {"importlib.resources", NULL, 3170693, 8305, NUITKA_BYTECODE_FLAG},
    {"importlib.util", NULL, 3178998, 9322, NUITKA_BYTECODE_FLAG},
    {"inspect", NULL, 3188320, 79998, NUITKA_BYTECODE_FLAG},
    {"io", NULL, 3268318, 3375, NUITKA_BYTECODE_FLAG},
    {"ipaddress", NULL, 3271693, 62960, NUITKA_BYTECODE_FLAG},
    {"json", NULL, 3334653, 12304, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"json.decoder", NULL, 3346957, 9790, NUITKA_BYTECODE_FLAG},
    {"json.encoder", NULL, 3356747, 11272, NUITKA_BYTECODE_FLAG},
    {"json.scanner", NULL, 3368019, 1962, NUITKA_BYTECODE_FLAG},
    {"json.tool", NULL, 3369981, 1443, NUITKA_BYTECODE_FLAG},
    {"keyword", NULL, 3371424, 1775, NUITKA_BYTECODE_FLAG},
    {"lib2to3", NULL, 3373199, 109, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.btm_matcher", NULL, 3373308, 4859, NUITKA_BYTECODE_FLAG},
    {"lib2to3.btm_utils", NULL, 3378167, 6109, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixer_base", NULL, 3384276, 6201, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixer_util", NULL, 3390477, 12012, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes", NULL, 3402489, 115, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.fixes.fix_apply", NULL, 3402604, 1653, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_asserts", NULL, 3404257, 1238, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_basestring", NULL, 3405495, 628, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_buffer", NULL, 3406123, 773, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_dict", NULL, 3406896, 3281, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_except", NULL, 3410177, 2783, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_exec", NULL, 3412960, 1114, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_execfile", NULL, 3414074, 1658, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_exitfunc", NULL, 3415732, 2261, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_filter", NULL, 3417993, 2326, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_funcattrs", NULL, 3420319, 939, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_future", NULL, 3421258, 749, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_getcwdu", NULL, 3422007, 753, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_has_key", NULL, 3422760, 2883, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_idioms", NULL, 3425643, 3868, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_import", NULL, 3429511, 2750, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_imports", NULL, 3432261, 4314, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_imports2", NULL, 3436575, 513, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_input", NULL, 3437088, 915, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_intern", NULL, 3438003, 1122, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_isinstance", NULL, 3439125, 1520, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_itertools", NULL, 3440645, 1509, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_itertools_imports", NULL, 3442154, 1545, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_long", NULL, 3443699, 670, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_map", NULL, 3444369, 3058, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_metaclass", NULL, 3447427, 5311, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_methodattrs", NULL, 3452738, 901, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_ne", NULL, 3453639, 772, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_next", NULL, 3454411, 3025, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_nonzero", NULL, 3457436, 888, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_numliterals", NULL, 3458324, 984, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_operator", NULL, 3459308, 4201, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_paren", NULL, 3463509, 1355, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_print", NULL, 3464864, 2290, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_raise", NULL, 3467154, 2214, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_raw_input", NULL, 3469368, 760, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_reduce", NULL, 3470128, 1095, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_reload", NULL, 3471223, 1134, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_renames", NULL, 3472357, 1958, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_repr", NULL, 3474315, 810, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_set_literal", NULL, 3475125, 1642, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_standarderror", NULL, 3476767, 685, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_sys_exc", NULL, 3477452, 1366, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_throw", NULL, 3478818, 1767, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_tuple_params", NULL, 3480585, 4561, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_types", NULL, 3485146, 1794, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_unicode", NULL, 3486940, 1506, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_urllib", NULL, 3488446, 5926, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_ws_comma", NULL, 3494372, 1088, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_xrange", NULL, 3495460, 2501, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_xreadlines", NULL, 3497961, 1082, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_zip", NULL, 3499043, 1546, NUITKA_BYTECODE_FLAG},
    {"lib2to3.main", NULL, 3500589, 8502, NUITKA_BYTECODE_FLAG},
    {"lib2to3.patcomp", NULL, 3509091, 5577, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2", NULL, 3514668, 145, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.pgen2.driver", NULL, 3514813, 4950, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.grammar", NULL, 3519763, 6982, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.literals", NULL, 3526745, 1524, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.parse", NULL, 3528269, 6270, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.pgen", NULL, 3534539, 9746, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.token", NULL, 3544285, 1838, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.tokenize", NULL, 3546123, 15106, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pygram", NULL, 3561229, 1164, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pytree", NULL, 3562393, 24969, NUITKA_BYTECODE_FLAG},
    {"lib2to3.refactor", NULL, 3587362, 20370, NUITKA_BYTECODE_FLAG},
    {"linecache", NULL, 3607732, 3755, NUITKA_BYTECODE_FLAG},
    {"locale", NULL, 3611487, 34506, NUITKA_BYTECODE_FLAG},
    {"logging", NULL, 3645993, 62524, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"logging.config", NULL, 3708517, 22981, NUITKA_BYTECODE_FLAG},
    {"logging.handlers", NULL, 3731498, 42949, NUITKA_BYTECODE_FLAG},
    {"lzma", NULL, 3774447, 11905, NUITKA_BYTECODE_FLAG},
    {"macpath", NULL, 3786352, 5773, NUITKA_BYTECODE_FLAG},
    {"mailbox", NULL, 3792125, 63614, NUITKA_BYTECODE_FLAG},
    {"mailcap", NULL, 3855739, 6450, NUITKA_BYTECODE_FLAG},
    {"mimetypes", NULL, 3862189, 15444, NUITKA_BYTECODE_FLAG},
    {"modulefinder", NULL, 3877633, 15318, NUITKA_BYTECODE_FLAG},
    {"multiprocessing", NULL, 3892951, 493, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"multiprocessing.connection", NULL, 3893444, 24898, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.context", NULL, 3918342, 13079, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.dummy", NULL, 3931421, 3771, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"multiprocessing.dummy.connection", NULL, 3935192, 2479, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.forkserver", NULL, 3937671, 7730, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.heap", NULL, 3945401, 6390, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.managers", NULL, 3951791, 33970, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.pool", NULL, 3985761, 21202, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.popen_fork", NULL, 4006963, 2492, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.popen_forkserver", NULL, 4009455, 2325, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.popen_spawn_posix", NULL, 4011780, 2107, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.process", NULL, 4013887, 9392, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.queues", NULL, 4023279, 9403, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.reduction", NULL, 4032682, 7984, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.resource_sharer", NULL, 4040666, 5169, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.semaphore_tracker", NULL, 4045835, 3706, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.sharedctypes", NULL, 4049541, 6883, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.spawn", NULL, 4056424, 6434, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.synchronize", NULL, 4062858, 11149, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.util", NULL, 4074007, 9913, NUITKA_BYTECODE_FLAG},
    {"netrc", NULL, 4083920, 3729, NUITKA_BYTECODE_FLAG},
    {"nntplib", NULL, 4087649, 33717, NUITKA_BYTECODE_FLAG},
    {"ntpath", NULL, 4121366, 12970, NUITKA_BYTECODE_FLAG},
    {"nturl2path", NULL, 4134336, 1581, NUITKA_BYTECODE_FLAG},
    {"numbers", NULL, 4135917, 12158, NUITKA_BYTECODE_FLAG},
    {"opcode", NULL, 4148075, 5344, NUITKA_BYTECODE_FLAG},
    {"operator", NULL, 1341, 13866, NUITKA_BYTECODE_FLAG},
    {"optparse", NULL, 4153419, 47859, NUITKA_BYTECODE_FLAG},
    {"os", NULL, 15207, 29659, NUITKA_BYTECODE_FLAG},
    {"pathlib", NULL, 4201278, 41598, NUITKA_BYTECODE_FLAG},
    {"pdb", NULL, 4242876, 46842, NUITKA_BYTECODE_FLAG},
    {"pickle", NULL, 4289718, 42932, NUITKA_BYTECODE_FLAG},
    {"pickletools", NULL, 4332650, 65297, NUITKA_BYTECODE_FLAG},
    {"pipes", NULL, 4397947, 7769, NUITKA_BYTECODE_FLAG},
    {"pkgutil", NULL, 4405716, 16326, NUITKA_BYTECODE_FLAG},
    {"platform", NULL, 4422042, 28747, NUITKA_BYTECODE_FLAG},
    {"plistlib", NULL, 4450789, 25061, NUITKA_BYTECODE_FLAG},
    {"poplib", NULL, 4475850, 13302, NUITKA_BYTECODE_FLAG},
    {"posixpath", NULL, 4489152, 10396, NUITKA_BYTECODE_FLAG},
    {"pprint", NULL, 4499548, 15799, NUITKA_BYTECODE_FLAG},
    {"profile", NULL, 4515347, 13807, NUITKA_BYTECODE_FLAG},
    {"pstats", NULL, 4529154, 22260, NUITKA_BYTECODE_FLAG},
    {"pty", NULL, 4551414, 3849, NUITKA_BYTECODE_FLAG},
    {"py_compile", NULL, 4555263, 7157, NUITKA_BYTECODE_FLAG},
    {"pyclbr", NULL, 4562420, 10339, NUITKA_BYTECODE_FLAG},
    {"pydoc", NULL, 4572759, 84669, NUITKA_BYTECODE_FLAG},
    {"pydoc_data", NULL, 4657428, 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"pydoc_data.topics", NULL, 4657540, 411562, NUITKA_BYTECODE_FLAG},
    {"queue", NULL, 5069102, 11438, NUITKA_BYTECODE_FLAG},
    {"quopri", NULL, 5080540, 5737, NUITKA_BYTECODE_FLAG},
    {"random", NULL, 5086277, 19374, NUITKA_BYTECODE_FLAG},
    {"re", NULL, 5105651, 13770, NUITKA_BYTECODE_FLAG},
    {"reprlib", NULL, 5119421, 5316, NUITKA_BYTECODE_FLAG},
    {"rlcompleter", NULL, 5124737, 5713, NUITKA_BYTECODE_FLAG},
    {"runpy", NULL, 5130450, 7911, NUITKA_BYTECODE_FLAG},
    {"sched", NULL, 5138361, 6487, NUITKA_BYTECODE_FLAG},
    {"secrets", NULL, 5144848, 2150, NUITKA_BYTECODE_FLAG},
    {"selectors", NULL, 5146998, 16914, NUITKA_BYTECODE_FLAG},
    {"shelve", NULL, 5163912, 9472, NUITKA_BYTECODE_FLAG},
    {"shlex", NULL, 5173384, 6956, NUITKA_BYTECODE_FLAG},
    {"shutil", NULL, 5180340, 30830, NUITKA_BYTECODE_FLAG},
    {"signal", NULL, 5211170, 2478, NUITKA_BYTECODE_FLAG},
    {"site", NULL, 44866, 13430, NUITKA_BYTECODE_FLAG},
    {"sitecustomize", NULL, 5213648, 200, NUITKA_BYTECODE_FLAG},
    {"smtpd", NULL, 5213848, 26570, NUITKA_BYTECODE_FLAG},
    {"smtplib", NULL, 5240418, 35316, NUITKA_BYTECODE_FLAG},
    {"sndhdr", NULL, 5275734, 6869, NUITKA_BYTECODE_FLAG},
    {"socket", NULL, 5282603, 21987, NUITKA_BYTECODE_FLAG},
    {"socketserver", NULL, 5304590, 24165, NUITKA_BYTECODE_FLAG},
    {"sqlite3", NULL, 5328755, 140, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"sqlite3.dbapi2", NULL, 5328895, 2459, NUITKA_BYTECODE_FLAG},
    {"sqlite3.dump", NULL, 5331354, 1902, NUITKA_BYTECODE_FLAG},
    {"sre_compile", NULL, 5333256, 15169, NUITKA_BYTECODE_FLAG},
    {"sre_constants", NULL, 5348425, 6257, NUITKA_BYTECODE_FLAG},
    {"sre_parse", NULL, 5354682, 21323, NUITKA_BYTECODE_FLAG},
    {"ssl", NULL, 5376005, 39737, NUITKA_BYTECODE_FLAG},
    {"stat", NULL, 5415742, 3839, NUITKA_BYTECODE_FLAG},
    {"statistics", NULL, 5419581, 18130, NUITKA_BYTECODE_FLAG},
    {"string", NULL, 5437711, 7801, NUITKA_BYTECODE_FLAG},
    {"stringprep", NULL, 5445512, 9998, NUITKA_BYTECODE_FLAG},
    {"struct", NULL, 5455510, 300, NUITKA_BYTECODE_FLAG},
    {"subprocess", NULL, 5455810, 38731, NUITKA_BYTECODE_FLAG},
    {"sunau", NULL, 5494541, 17177, NUITKA_BYTECODE_FLAG},
    {"symbol", NULL, 5511718, 2531, NUITKA_BYTECODE_FLAG},
    {"symtable", NULL, 5514249, 10411, NUITKA_BYTECODE_FLAG},
    {"sysconfig", NULL, 5524660, 15495, NUITKA_BYTECODE_FLAG},
    {"tabnanny", NULL, 5540155, 6944, NUITKA_BYTECODE_FLAG},
    {"tarfile", NULL, 5547099, 61807, NUITKA_BYTECODE_FLAG},
    {"telnetlib", NULL, 5608906, 18068, NUITKA_BYTECODE_FLAG},
    {"tempfile", NULL, 5626974, 25208, NUITKA_BYTECODE_FLAG},
    {"textwrap", NULL, 5652182, 13578, NUITKA_BYTECODE_FLAG},
    {"this", NULL, 5665760, 1243, NUITKA_BYTECODE_FLAG},
    {"threading", NULL, 5667003, 37668, NUITKA_BYTECODE_FLAG},
    {"timeit", NULL, 5704671, 11613, NUITKA_BYTECODE_FLAG},
    {"tkinter", NULL, 5716284, 179141, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"tkinter.colorchooser", NULL, 5895425, 1101, NUITKA_BYTECODE_FLAG},
    {"tkinter.commondialog", NULL, 5896526, 1082, NUITKA_BYTECODE_FLAG},
    {"tkinter.constants", NULL, 5897608, 1634, NUITKA_BYTECODE_FLAG},
    {"tkinter.dialog", NULL, 5899242, 1410, NUITKA_BYTECODE_FLAG},
    {"tkinter.dnd", NULL, 5900652, 11152, NUITKA_BYTECODE_FLAG},
    {"tkinter.filedialog", NULL, 5911804, 12236, NUITKA_BYTECODE_FLAG},
    {"tkinter.font", NULL, 5924040, 6153, NUITKA_BYTECODE_FLAG},
    {"tkinter.messagebox", NULL, 5930193, 3018, NUITKA_BYTECODE_FLAG},
    {"tkinter.scrolledtext", NULL, 5933211, 2145, NUITKA_BYTECODE_FLAG},
    {"tkinter.simpledialog", NULL, 5935356, 10512, NUITKA_BYTECODE_FLAG},
    {"tkinter.tix", NULL, 5945868, 83632, NUITKA_BYTECODE_FLAG},
    {"tkinter.ttk", NULL, 6029500, 57816, NUITKA_BYTECODE_FLAG},
    {"token", NULL, 6087316, 3565, NUITKA_BYTECODE_FLAG},
    {"tokenize", NULL, 6090881, 17797, NUITKA_BYTECODE_FLAG},
    {"trace", NULL, 6108678, 19113, NUITKA_BYTECODE_FLAG},
    {"traceback", NULL, 6127791, 19589, NUITKA_BYTECODE_FLAG},
    {"tracemalloc", NULL, 6147380, 17242, NUITKA_BYTECODE_FLAG},
    {"tty", NULL, 6164622, 1060, NUITKA_BYTECODE_FLAG},
    {"turtle", NULL, 6165682, 130757, NUITKA_BYTECODE_FLAG},
    {"types", NULL, 58296, 8942, NUITKA_BYTECODE_FLAG},
    {"typing", NULL, 6296439, 49924, NUITKA_BYTECODE_FLAG},
    {"unittest", NULL, 6346363, 2977, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"unittest.case", NULL, 6349340, 48049, NUITKA_BYTECODE_FLAG},
    {"unittest.loader", NULL, 6397389, 14242, NUITKA_BYTECODE_FLAG},
    {"unittest.main", NULL, 6411631, 7404, NUITKA_BYTECODE_FLAG},
    {"unittest.mock", NULL, 6419035, 63172, NUITKA_BYTECODE_FLAG},
    {"unittest.result", NULL, 6482207, 7218, NUITKA_BYTECODE_FLAG},
    {"unittest.runner", NULL, 6489425, 6961, NUITKA_BYTECODE_FLAG},
    {"unittest.signals", NULL, 6496386, 2160, NUITKA_BYTECODE_FLAG},
    {"unittest.suite", NULL, 6498546, 9169, NUITKA_BYTECODE_FLAG},
    {"unittest.util", NULL, 6507715, 4487, NUITKA_BYTECODE_FLAG},
    {"urllib", NULL, 6512202, 108, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"urllib.error", NULL, 6512310, 2742, NUITKA_BYTECODE_FLAG},
    {"urllib.parse", NULL, 6515052, 30454, NUITKA_BYTECODE_FLAG},
    {"urllib.request", NULL, 6545506, 72285, NUITKA_BYTECODE_FLAG},
    {"urllib.response", NULL, 6617791, 3215, NUITKA_BYTECODE_FLAG},
    {"urllib.robotparser", NULL, 6621006, 7027, NUITKA_BYTECODE_FLAG},
    {"uu", NULL, 6628033, 3538, NUITKA_BYTECODE_FLAG},
    {"uuid", NULL, 6631571, 23165, NUITKA_BYTECODE_FLAG},
    {"venv", NULL, 6654736, 14314, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"warnings", NULL, 6669050, 13906, NUITKA_BYTECODE_FLAG},
    {"wave", NULL, 6682956, 18254, NUITKA_BYTECODE_FLAG},
    {"weakref", NULL, 6701210, 19184, NUITKA_BYTECODE_FLAG},
    {"webbrowser", NULL, 6720394, 16340, NUITKA_BYTECODE_FLAG},
    {"wsgiref", NULL, 6736734, 704, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"wsgiref.handlers", NULL, 6737438, 16113, NUITKA_BYTECODE_FLAG},
    {"wsgiref.headers", NULL, 6753551, 7725, NUITKA_BYTECODE_FLAG},
    {"wsgiref.simple_server", NULL, 6761276, 5171, NUITKA_BYTECODE_FLAG},
    {"wsgiref.util", NULL, 6766447, 5146, NUITKA_BYTECODE_FLAG},
    {"wsgiref.validate", NULL, 6771593, 14642, NUITKA_BYTECODE_FLAG},
    {"xdrlib", NULL, 6786235, 8290, NUITKA_BYTECODE_FLAG},
    {"xml", NULL, 6794525, 672, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.dom", NULL, 6795197, 5424, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.dom.NodeFilter", NULL, 6800621, 939, NUITKA_BYTECODE_FLAG},
    {"xml.dom.domreg", NULL, 6801560, 2816, NUITKA_BYTECODE_FLAG},
    {"xml.dom.expatbuilder", NULL, 6804376, 26986, NUITKA_BYTECODE_FLAG},
    {"xml.dom.minicompat", NULL, 6831362, 2785, NUITKA_BYTECODE_FLAG},
    {"xml.dom.minidom", NULL, 6834147, 55585, NUITKA_BYTECODE_FLAG},
    {"xml.dom.pulldom", NULL, 6889732, 10458, NUITKA_BYTECODE_FLAG},
    {"xml.dom.xmlbuilder", NULL, 6900190, 12405, NUITKA_BYTECODE_FLAG},
    {"xml.etree", NULL, 6912595, 111, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.etree.ElementInclude", NULL, 6912706, 1547, NUITKA_BYTECODE_FLAG},
    {"xml.etree.ElementPath", NULL, 6914253, 6315, NUITKA_BYTECODE_FLAG},
    {"xml.etree.ElementTree", NULL, 6920568, 44779, NUITKA_BYTECODE_FLAG},
    {"xml.etree.cElementTree", NULL, 6965347, 153, NUITKA_BYTECODE_FLAG},
    {"xml.parsers", NULL, 6965500, 285, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.parsers.expat", NULL, 6965785, 314, NUITKA_BYTECODE_FLAG},
    {"xml.sax", NULL, 6966099, 3144, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.sax._exceptions", NULL, 6969243, 5453, NUITKA_BYTECODE_FLAG},
    {"xml.sax.expatreader", NULL, 6974696, 12356, NUITKA_BYTECODE_FLAG},
    {"xml.sax.handler", NULL, 6987052, 12329, NUITKA_BYTECODE_FLAG},
    {"xml.sax.saxutils", NULL, 6999381, 12782, NUITKA_BYTECODE_FLAG},
    {"xml.sax.xmlreader", NULL, 7012163, 16890, NUITKA_BYTECODE_FLAG},
    {"xmlrpc", NULL, 7029053, 108, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xmlrpc.client", NULL, 7029161, 34514, NUITKA_BYTECODE_FLAG},
    {"xmlrpc.server", NULL, 7063675, 29388, NUITKA_BYTECODE_FLAG},
    {"zipapp", NULL, 7093063, 5769, NUITKA_BYTECODE_FLAG},
    {"zipfile", NULL, 7098832, 49838, NUITKA_BYTECODE_FLAG},
    {NULL, NULL, 0, 0, 0}
};

void setupMetaPathBasedLoader(void) {
    static bool init_done = false;

    if (init_done == false) {
        registerMetaPathBasedUnfreezer(meta_path_loader_entries);
        init_done = true;
    }
}
