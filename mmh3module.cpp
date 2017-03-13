#include <stdio.h>
#include <string.h>
#include <Python.h>
#include "murmur_hash_3.hpp"

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

    static char *kwlist[] = {(char *)"key", (char *)"seed", NULL};

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

    #ifdef _MSC_VER
    const char *format = "LL";
    #else
    const char *format = "ll";
    #endif

    static char *kwlist[] = {(char *)"key", (char *)"seed", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#|i", kwlist,
                                     &target_str, &target_str_len, &seed)) {
        return NULL;
    }

    MurmurHash3_x64_128(target_str, target_str_len, seed, result);

    PyObject *retval = Py_BuildValue(format, result[0], result[1]);
    return retval;
}

static PyObject *
mmh3_hash128(PyObject *self, PyObject *args, PyObject *keywds) {
    const char *target_str;
    int target_str_len;
    uint32_t seed = 0;
    uint64_t result[2];

    static char *kwlist[] = {(char *)"key", (char *)"seed", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#|i", kwlist,
                                     &target_str, &target_str_len, &seed)) {
        return NULL;
    }

    MurmurHash3_x64_128(target_str, target_str_len, seed, result);

    PyObject *retval = _PyLong_FromByteArray((unsigned char *)result, 16, 1, 0);
    return retval;
}

static PyObject *
mmh3_hash_bytes(PyObject *self, PyObject *args, PyObject *keywds) {
    const char *target_str;
    int target_str_len;
    uint32_t seed = 0;
    uint32_t result[4];

    static char *kwlist[] = {(char *)"key", (char *)"seed", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#|i", kwlist,
                                     &target_str, &target_str_len, &seed)) {
        return NULL;
    }

    MurmurHash3_x64_128(target_str, target_str_len, seed, result);

    char bytes[16];
    memcpy(bytes, result, 16);
    return PyBytes_FromStringAndSize(bytes, 16);
}

struct module_state {
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static PyMethodDef Mmh3Methods[] = {
    {
        "hash", (PyCFunction)mmh3_hash, METH_VARARGS | METH_KEYWORDS,
        "hash(key, seed=0) -> int32\n"
        "Return a 32 bit integer hash value."
    },
    {
        "hash64", (PyCFunction)mmh3_hash64, METH_VARARGS | METH_KEYWORDS,
        "hash64(key, seed=0) -> (int64, int64)\n"
        "Return a tuple of two 64 bit integer hash values for a string."
    },
    {
        "hash128", (PyCFunction)mmh3_hash128, METH_VARARGS | METH_KEYWORDS,
        "hash128(key, seed=0) -> int128\n"
        "Return a 128 bit long integer hash value."
    },
    {
        "hash_bytes", (PyCFunction)mmh3_hash_bytes, METH_VARARGS | METH_KEYWORDS,
        "hash_bytes(key, seed=0, x64arch=True) -> bytes\n"
        "Return a 128 bit hash value as bytes for a string"
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

#if PY_MAJOR_VERSION >= 3
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

PyMODINIT_FUNC PyInit_mmh3(void) {
    PyObject *module = PyModule_Create(&mmh3module);
    if (module == NULL)
        return NULL;

    PyModule_AddStringConstant(module, "__version__", "2.3");

    struct module_state *st = GETSTATE(module);

    st->error = PyErr_NewException("mmh3.Error", NULL, NULL);
    if (st->error == NULL) {
        Py_DECREF(module);
        return NULL;
    }

    return module;
}
#else
PyMODINIT_FUNC initmmh3(void) {
    PyObject *module = Py_InitModule3(
        "mmh3",
        Mmh3Methods,
        "mmh3 is a Python frontend to MurmurHash3, a fast and robust hash library.\n"
    );
}
#endif
