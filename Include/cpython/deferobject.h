/* Defer object interface */

#ifndef Py_LIMITED_API
#ifndef Py_DEFEROBJECT_H
#define Py_DEFEROBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    PyObject_HEAD
    /* Content of the defer or NULL when empty */
    PyObject *ob_callable;
} PyDeferObject;

PyAPI_DATA(PyTypeObject) PyDefer_Type;

#define PyDefer_Check(op) Py_IS_TYPE((op), &PyDefer_Type)

PyAPI_FUNC(PyObject *) PyDefer_New(PyObject *);
PyAPI_FUNC(PyObject *) PyDefer_Get(PyObject *);

#ifdef __cplusplus
}
#endif
#endif /* !Py_TUPLEOBJECT_H */
#endif /* Py_LIMITED_API */
