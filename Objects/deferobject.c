/* Defer object implementation */

#include "Python.h"
#include "pycore_modsupport.h"    // _PyArg_NoKeywords()
#include "pycore_object.h"

PyObject *
PyDefer_New(PyObject *obj)
{
    PyDeferObject *op;

    if (!PyCallable_Check(obj)) {
        PyErr_SetString(PyExc_TypeError, "Defer argument needs to be callable with 0 arguments");
    }

    op = (PyDeferObject *)PyObject_GC_New(PyDeferObject, &PyDefer_Type);
    if (op == NULL)
        return NULL;
    op->ob_callable = Py_XNewRef(obj);

    _PyObject_GC_TRACK(op);
    return (PyObject *)op;
}

PyDoc_STRVAR(defer_new_doc,
"Defer(contents)\n"
"--\n"
"\n"
"Create a new defer object.\n"
"\n"
"  contents\n"
"    the callable wrapped in the the defer.");


static PyObject *
defer_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    PyObject *return_value = NULL;
    PyObject *obj = NULL;

    if (!_PyArg_NoKeywords("Defer", kwargs)) {
        goto exit;
    }
    if (!PyArg_UnpackTuple(args, "Defer", 1, 1, &obj)) {
        goto exit;
    }
    return_value = PyDefer_New(obj);

exit:
    return return_value;
}

PyObject* PyDefer_Get(PyObject *op) {
    if (!PyDefer_Check(op)) {
        PyErr_BadInternalCall();
        return NULL;
    }
    PyDeferObject *defer;
    defer = _Py_CAST(PyDeferObject*, op);
    return PyObject_CallNoArgs(defer->ob_callable);
}

static void
defer_dealloc(PyDeferObject *op)
{
    _PyObject_GC_UNTRACK(op);
    Py_XDECREF(op->ob_callable);
    PyObject_GC_Del(op);
}


static PyObject *
defer_repr(PyDeferObject *op)
{
    if (op->ob_callable == NULL)
        return PyUnicode_FromFormat("<Defer at %p: empty>", op);

    return PyUnicode_FromFormat("<Defer at %p: %.80s object at %p>",
                               op, Py_TYPE(op->ob_callable)->tp_name,
                               op->ob_callable);
}

static int
defer_traverse(PyDeferObject *op, visitproc visit, void *arg)
{
    Py_VISIT(op->ob_callable);
    return 0;
}

static int
defer_clear(PyDeferObject *op)
{
    Py_CLEAR(op->ob_callable);
    return 0;
}

static PyObject *
defer_get_contents(PyDeferObject *op, void *closure)
{
    if (op->ob_callable == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Defer is empty");
        return NULL;
    }
    return Py_NewRef(op->ob_callable);
}


static PyObject *
defer_call(PyObject *_defer, PyObject *args, PyObject *kwargs) {
    PyDeferObject * defer = (PyDeferObject*) _defer;

    PyObject *return_value = NULL;

    if (!_PyArg_NoPositional("Defer", args)) {
        goto exit;
    }
    if (!_PyArg_NoKeywords("Defer", kwargs)) {
        goto exit;
    }
    return_value = PyDefer_Get(defer);

    exit:
    return return_value;
}


static PyGetSetDef defer_getsetlist[] = {
    {"callable", (getter)defer_get_contents,
                      NULL, NULL},
    {NULL} /* sentinel */
};

PyTypeObject PyDefer_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "Defer",
    sizeof(PyDeferObject),
    0,
    (destructor)defer_dealloc,                  /* tp_dealloc */
    0,                                          /* tp_vectorcall_offset */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_as_async */
    (reprfunc)defer_repr,                       /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    defer_call,                                 /* tp_call */
    0,                                          /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,    /* tp_flags */
    defer_new_doc,                              /* tp_doc */
    (traverseproc)defer_traverse,               /* tp_traverse */
    (inquiry)defer_clear,                       /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    0,                                          /* tp_methods */
    0,                                          /* tp_members */
    defer_getsetlist,                           /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* tp_dictoffset */
    0,                                          /* tp_init */
    0,                                          /* tp_alloc */
    (newfunc)defer_new,                         /* tp_new */
    0,                                          /* tp_free */
};
