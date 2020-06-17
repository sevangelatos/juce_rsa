#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include <cctype>

#include <juce_cryptography/juce_cryptography.h>
#include <juce_core/juce_core.h>

typedef struct PyRSAKey_struct
{
    PyObject_HEAD
    juce::RSAKey *rsa = nullptr;
} PyRSAKey;

static void
PyRSAKey_dealloc(PyRSAKey *self)
{
    delete self->rsa;
    self->rsa = nullptr;
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
PyRSAKey_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyRSAKey *self = (PyRSAKey *)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        self->rsa = new juce::RSAKey();
    }
    return (PyObject *)self;
}

static int
PyRSAKey_init(PyRSAKey *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {(char *)"s", NULL};
    const char *string_repr = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &string_repr))
        return -1;

    if (string_repr)
    {
        juce::String jstring(string_repr);
        if (!jstring.containsChar(','))
        {
            PyErr_SetString(PyExc_ValueError,
                            "String representation must be two comma-separated hex numbers");
            return -1;
        }
        *(self->rsa) = juce::RSAKey(string_repr);
    }
    return 0;
}

static bool is_hex_string(const char *str)
{
    // Empty string is not considered a HEX
    if (str == nullptr || str[0] == '\0')
    {
        return false;
    }

    for (size_t i = 0; str[i] != '\0'; i++)
    {
        if (!std::isxdigit(static_cast<unsigned char>(str[i])))
        {
            return false;
        }
    }
    return true;
}

static PyObject *
PyRSAKey_apply_unicode(PyRSAKey *self, PyObject *obj)
{
    const char *utfChars = PyUnicode_AsUTF8(obj);
    if (utfChars[0] && utfChars[0] == '0' &&
        utfChars[1] && std::tolower(utfChars[1]) == 'x')
    {
        utfChars += 2;
    }

    if (!is_hex_string(utfChars))
    {
        PyErr_SetString(PyExc_ValueError, "Invalid hex string");
        return NULL;
    }

    juce::BigInteger bigInt;
    bool ok = false;
    Py_BEGIN_ALLOW_THREADS;
    bigInt.parseString(utfChars, 16);
    ok = self->rsa->applyToValue(bigInt);
    Py_END_ALLOW_THREADS;
    if (!ok)
    {
        PyErr_SetString(PyExc_AssertionError, "Using an uninitialized key");
        return NULL;
    }
    auto jStr = "0x" + bigInt.toString(16);
    return PyUnicode_DecodeUTF8(jStr.toRawUTF8(), jStr.getNumBytesAsUTF8(), "strict");
}

static PyObject *
PyRSAKey_apply_long(PyRSAKey *self, PyObject *obj)
{
    const auto bits = _PyLong_NumBits(obj);
    // Always allocate extra to extend with zero
    size_t numBytes = (bits >> 3) + 1;
    juce::MemoryBlock memBlock(numBytes, true);
    unsigned char *data = static_cast<unsigned char *>(memBlock.getData());
    const int little_endian = 1;
    const int is_signed = 0;
    PyLongObject *v = reinterpret_cast<PyLongObject *>(obj);
    if (_PyLong_AsByteArray(v, data, memBlock.getSize(), little_endian, is_signed) == -1)
    {
        PyErr_SetString(PyExc_ValueError, "Could not convert to integer");
        return NULL;
    }
    bool ok = false;
    juce::BigInteger bigInt;
    Py_BEGIN_ALLOW_THREADS;
    bigInt.loadFromMemoryBlock(memBlock);
    ok = self->rsa->applyToValue(bigInt);
    Py_END_ALLOW_THREADS;
    if (!ok)
    {
        PyErr_SetString(PyExc_AssertionError, "Using an uninitialized key");
        return NULL;
    }

    memBlock = bigInt.toMemoryBlock();
    data = static_cast<unsigned char *>(memBlock.getData());
    return _PyLong_FromByteArray(data, memBlock.getSize(), little_endian, is_signed);
}

static PyObject *
PyRSAKey_apply(PyRSAKey *self, PyObject *const *params, Py_ssize_t count)
{
    if (count != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Apply needs one argument");
        return NULL;
    }
    auto obj = params[0];
    PyObject *result = nullptr;

    if (PyUnicode_Check(obj))
    {
        result = PyRSAKey_apply_unicode(self, obj);
    }
    else if (PyLong_Check(obj))
    {
        result = PyRSAKey_apply_long(self, obj);
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Hex string or integer expected");
        return NULL;
    }

    return result;
}

PyObject *PyRSAKey_repr(PyRSAKey *self)
{
    auto str = self->rsa->toString().replaceFirstOccurrenceOf(",", ", ");

    return PyUnicode_FromFormat("RSAKey(\"%s\")", str.toRawUTF8());
}

static PyMethodDef PyRSAKey_methods[] = {
    {"apply", (PyCFunction)PyRSAKey_apply, METH_FASTCALL,
     "Apply RSA to a value. Value can be an integer or a hexadecimal string"},
    {NULL} /* Sentinel */
};

static PyTypeObject PyRSAKeyType = {
    PyVarObject_HEAD_INIT(NULL, 0)                           // ob_base
    "juce_rsa.RSAKey",                                       // tp_name
    sizeof(PyRSAKey),                                        // tp_basicsize
    0,                                                       // tp_itemsize
    (destructor)PyRSAKey_dealloc,                            // tp_dealloc
    0,                                                       // tp_vectorcall_offset
    NULL,                                                    // tp_getattr
    NULL,                                                    // tp_setattr
    NULL,                                                    // tp_as_async
    (reprfunc)PyRSAKey_repr,                                 // tp_repr
    NULL,                                                    // tp_as_number
    NULL,                                                    // tp_as_sequence
    NULL,                                                    // tp_as_mapping
    NULL,                                                    // tp_hash
    NULL,                                                    // tp_call
    (reprfunc)PyRSAKey_repr,                                 // tp_str
    NULL,                                                    // tp_getattro
    NULL,                                                    // tp_setattro
    NULL,                                                    // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                                      // tp_flags
    "RSAKey object can be used to encypt/decrypt using RSA", // tp_doc
    NULL,                                                    // tp_traverse
    NULL,                                                    // tp_clear
    NULL,                                                    // tp_richcompare
    0,                                                       // tp_weaklistoffset
    NULL,                                                    // tp_iter
    NULL,                                                    // tp_iternext
    PyRSAKey_methods,                                        // tp_methods
    NULL,                                                    // tp_members
    NULL,                                                    // tp_getset
    NULL,                                                    // tp_base
    NULL,                                                    // tp_dict
    NULL,                                                    // tp_descr_get
    NULL,                                                    // tp_descr_set
    0,                                                       // tp_dictoffset
    (initproc)PyRSAKey_init,                                 // tp_init
    NULL,                                                    // tp_alloc
    PyRSAKey_new,                                            // tp_new
};

static bool is_positive_power_of_2(int a)
{
    return (a > 0) && !((a - 1) & a);
}

static PyObject *juce_rsa_create_key_pair(PyObject *self, PyObject *args)
{
    int numBits = 0;
    if (!PyArg_ParseTuple(args, "i", &numBits))
    {
        return NULL;
    }

    if (!is_positive_power_of_2(numBits) || numBits > 16384)
    {
        PyErr_SetString(PyExc_ValueError, "key size must be a power of 2 in the [2^4, 2^14] range");
        return NULL;
    }

    auto pub_obj = (PyRSAKey *)PyObject_CallObject((PyObject *)&PyRSAKeyType, NULL);
    auto priv_obj = (PyRSAKey *)PyObject_CallObject((PyObject *)&PyRSAKeyType, NULL);
    juce::RSAKey &priv = *(priv_obj->rsa);
    juce::RSAKey &pub = *(pub_obj->rsa);

    Py_BEGIN_ALLOW_THREADS;
    juce::RSAKey::createKeyPair(pub, priv, numBits);
    Py_END_ALLOW_THREADS;

    return PyTuple_Pack(2, pub_obj, priv_obj);
}

static const char * rsa_key_pair_doc = 
"Create a key pair\n\n"
"\tParameters:\n"
"\tn (int): Key size in bits. eg. 2048\n"
"\n"
"\tReturns: (public, private) RSAKey pair\n";

static PyMethodDef juceRsaMethods[] = {
    {"create_key_pair", juce_rsa_create_key_pair, METH_VARARGS, rsa_key_pair_doc},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef juce_rsa_module = {
    PyModuleDef_HEAD_INIT,               // PyModuleDef_Base m_base;
    "juce_rsa",                          // const char* m_name;
    "Python bindings for juce::RSAKey.", // const char* m_doc;
    -1,                                  // Py_ssize_t m_size; size of per-interpreter module state or -1 if state in global variables.
    juceRsaMethods,                      //PyMethodDef *m_methods;
    NULL,                                // struct PyModuleDef_Slot* m_slots;
    NULL,                                // traverseproc m_traverse;
    NULL,                                // inquiry m_clear;
    NULL,                                // freefunc m_free;
};

PyMODINIT_FUNC
PyInit_juce_rsa(void)
{
    PyObject *m;
    if (PyType_Ready(&PyRSAKeyType) < 0)
        return NULL;

    m = PyModule_Create(&juce_rsa_module);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PyRSAKeyType);
    if (PyModule_AddObject(m, "RSAKey", (PyObject *)&PyRSAKeyType) < 0)
    {
        Py_DECREF(&PyRSAKeyType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
