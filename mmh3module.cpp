#include <stdio.h>
#include <string.h>
#include <Python.h>
#include "murmur_hash_3.h"

#if defined(_MSC_VER)
// Visual C++
typedef signed char int8_t;
typedef signed long int32_t;
typedef signed __int64 int64_t;
typedef unsigned char uint8_t;
typedef unsigned long uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

static PyObject *
mmh3_hash(PyObject *self, PyObject *args, PyObject *keywds) {
    const char *target_str;
    int target_str_len;
    uint32_t seed = 0;
    int32_t result[1];

    static char *kwlist[] = {(char *) "key", (char *) "seed", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#|i", kwlist,
                                     &target_str, &target_str_len, &seed)) {
        return NULL;
    }

    MurmurHash3_x86_32(target_str, target_str_len, seed, result);
    return PyLong_FromLong(result[0]);
}

static PyObject *
mmh3_hash64(PyObject *self, PyObject *args, PyObject *keywds) {
    const char *target_str;
    int target_str_len;
    uint32_t seed = 0;
    int64_t result[2];
    char x64arch = 1;

    #ifdef _MSC_VER
    const char *format = "LL";
    #else
    const char *format = "ll";
    #endif

    static char *kwlist[] = {(char *) "key", (char *) "seed", (char *) "x64arch", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#|iB", kwlist,
                                     &target_str, &target_str_len, &seed, &x64arch)) {
        return NULL;
    }

    if (x64arch == 1) {
        MurmurHash3_x64_128(target_str, target_str_len, seed, result);
    } else {
        MurmurHash3_x86_128(target_str, target_str_len, seed, result);
    }

    PyObject *retval = Py_BuildValue(format, result[0], result[1]);
    return retval;
}

static PyObject *
mmh3_hash128(PyObject *self, PyObject *args, PyObject *keywds) {
    const char *target_str;
    int target_str_len;
    uint32_t seed = 0;
    uint64_t result[2];
    char x64arch = 1;

    static char *kwlist[] = {(char *) "key", (char *) "seed", (char *) "x64arch", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#|iB", kwlist,
                                     &target_str, &target_str_len, &seed, &x64arch)) {
        return NULL;
    }

    if (x64arch == 1) {
        MurmurHash3_x64_128(target_str, target_str_len, seed, result);
    } else {
        MurmurHash3_x86_128(target_str, target_str_len, seed, result);
    }


    // TODO: use Py_BuildValue instead
    PyObject *retval = _PyLong_FromByteArray((unsigned char *)result, 16, 1, 0);
    return retval;
}

static PyObject *
mmh3_hash_bytes(PyObject *self, PyObject *args, PyObject *keywds) {
    const char *target_str;
    int target_str_len;
    uint32_t seed = 0;
    uint32_t result[4];
    char x64arch = 1;

    static char *kwlist[] = {(char *) "key", (char *) "seed", (char *) "x64arch", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#|iB", kwlist,
                                     &target_str, &target_str_len, &seed, &x64arch)) {
        return NULL;
    }

    if (x64arch == 1) {
        MurmurHash3_x64_128(target_str, target_str_len, seed, result);
    } else {
        MurmurHash3_x86_128(target_str, target_str_len, seed, result);
    }

    char bytes[16];
    memcpy(bytes, result, 16);
    return PyBytes_FromStringAndSize(bytes, 16);
}

struct module_state {
    PyObject *error;
};

#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))

static PyMethodDef Mmh3Methods[] = {
    {
        "hash", (PyCFunction)mmh3_hash, METH_VARARGS | METH_KEYWORDS,
        "hash(key, seed=0) -> hash value\n Return a 32 bit integer."
    },
    {
        "hash64", (PyCFunction)mmh3_hash64, METH_VARARGS | METH_KEYWORDS,
        "hash64(key, seed=0, x64arch=True) -> (hash value 1, hash value 2)\n"
        "Return a tuple of two 64 bit integers for a string\n\n"
        "Optimized for the x64 bit architecture when x64arch=True, otherwise for x86."
    },
    {
        "hash128", (PyCFunction)mmh3_hash128, METH_VARARGS | METH_KEYWORDS,
        "hash128(key, seed=0, x64arch=True) -> hash value\n"
        "Return a 128 bit long integer\n\n"
        "Optimized for the x64 bit architecture when x64arch=True, otherwise for x86."
    },
    {
        "hash_bytes", (PyCFunction)mmh3_hash_bytes,
        METH_VARARGS | METH_KEYWORDS,
        "hash_bytes(key, seed=0, x64arch=True) -> bytes\n"
        "Return a 128 bit hash value as bytes for a string\n\n"
        "Optimized for the x64 bit architecture when x64arch=True, otherwise for the x86."
    },
    {NULL, NULL, 0, NULL}
};

static int mmh3_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int mmh3_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef mmh3module = {
    PyModuleDef_HEAD_INIT,
    "mmh3",
    "mmh3 is a Python frontend to MurmurHash3, a fast and robust hash library.\n",
    sizeof(struct module_state),
    Mmh3Methods,
    NULL,
    mmh3_traverse,
    mmh3_clear,
    NULL
};

extern "C" {
    PyMODINIT_FUNC
    PyInit_mmh3() {
        PyObject *module = PyModule_Create(&mmh3module);

        if (module == NULL)
            return NULL;

        PyModule_AddStringConstant(module, "__version__", "2.3");

        struct module_state *st = GETSTATE(module);

        st->error = PyErr_NewException((char *) "mmh3.Error", NULL, NULL);
        if (st->error == NULL) {
            Py_DECREF(module);
            return NULL;
        }

        return module;
    }
}
