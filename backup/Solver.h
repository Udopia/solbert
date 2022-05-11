/*************************************************************************************************
MIT License

Solbert -- Copyright (c) 2022, Markus Iser, Algorithm Engineering, KIT-Department of Informatics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 **************************************************************************************************/

#include <iostream>
#include <vector>
#include <algorithm>

#include "structmember.h"

#include "lib/ipasir.h"

#include "src/util/PyUtil.h"

#ifndef SRC_APPS_SOLVER_H_
#define SRC_APPS_SOLVER_H_

typedef struct Solver {
    PyObject_HEAD
    void* solver;
} Solver;

/**
 * @brief Create new solver object
 * 
 * @param type 
 * @param args 
 * @param kwargs 
 * @return PyObject* 
 */
static PyObject* solver_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    Solver* self = (Solver*) type->tp_alloc(type, 0);
    self->solver = ipasir_init();
    return (PyObject*) self;
}

static PyObject* solver_delete(Solver* self) {
    ipasir_release(self->solver);
    Py_RETURN_NONE;
}

/**
 * @brief Add clauses
 * 
 * @param self 
 * @param args formula: list of list of int
 * @return PyObject* 
 */
static PyObject* add(Solver* self, PyObject* args) {
    PyObject* pyformula;
    PyArg_ParseTuple(args, "O", &pyformula);
    std::vector<std::vector<int>> formula = list_to_formula(pyformula);

    for (auto& clause : formula) {
        for (int lit : clause) {
            ipasir_add(self->solver, lit);        
        }
    }
    ipasir_add(self->solver, 0);

    Py_RETURN_NONE;
}

/**
 * @brief solve formula under assumptions
 * 
 * @param self 
 * @return PyObject* list of assumption literals
 */
static PyObject* solve(Solver* self, PyObject* args) {
    PyObject* pyassumps;
    PyArg_ParseTuple(args, "O", &pyassumps);
    std::vector<int> assumptions = list_to_vec(pyassumps);

    for (int lit : assumptions) {
        ipasir_assume(self->solver, lit); // fix end of extensible disjunction
    }

    int res = ipasir_solve(self->solver);
    if (res == 10) {
        Py_RETURN_TRUE;
    } else if (res == 20) {
        Py_RETURN_FALSE;        
    } else {
        Py_RETURN_NONE;
    }
}

static PyMethodDef solver_methods[] = {
    { "add", (PyCFunction) add, METH_VARARGS, "add clauses" },
    { "solve", (PyCFunction) solve, METH_VARARGS, "solver under assumptions" },
    { NULL }  /* Sentinel */
};

static PyTypeObject SolverType = {
    PyVarObject_HEAD_INIT(nullptr, 0)
    "solbert.Solver", /*tp_name*/
    sizeof(Solver), /*tp_basicsize*/
    0, /*tp_itemsize*/ 
    (destructor) solver_delete, /*tp_dealloc*/ 
    0, /*tp_print*/ 0, /*tp_getattr*/ 0, /*tp_setattr*/ 0, /*tp_compare*/ 0, /*tp_repr*/ 0, /*tp_as_number*/ 0, /*tp_as_sequence*/ 
    0, /*tp_as_mapping*/ 0, /*tp_hash */ 0, /*tp_call*/ 0, /*tp_str*/ 0, /*tp_getattro*/ 0, /*tp_setattro*/ 0, /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /* tp_flags: Py_TPFLAGS_HAVE_ITER tells python to use tp_iter and tp_iternext fields. */
    PyDoc_STR("solbert solver object."), /* tp_doc */
    0, /* tp_traverse */ 0, /* tp_clear */ 0, /* tp_richcompare */ 0, /* tp_weaklistoffset */
    0, /* tp_iter: __iter__() method */ 0, /* tp_iternext: next() method */
    solver_methods, /* tp_methods */ 
    0, /* tp_members */ 
    0, /* tp_getset */ 0, /* tp_base */ 0, /* tp_dict */
    0, /* tp_descr_get */ 0, /* tp_descr_set */ 0, /* tp_dictoffset */ 
    0, /* tp_init */
    PyType_GenericAlloc, /* tp_alloc */
    solver_new, /* tp_new */
};

#endif  // SRC_APPS_SOLVER_H_