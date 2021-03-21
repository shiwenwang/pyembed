#pragma once

#include <Python.h>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>


namespace PyEmbed
{    
    class CPyObject;

    class PyInterpreterScoped
    {
    public:
        PyInterpreterScoped() {
            if (Py_IsInitialized()) {
                printf("Python interpreter is already running\n");
            }
            Py_Initialize();
        }
        ~PyInterpreterScoped() {
            Py_Finalize();
        }
    };
    CPyObject Import(const char* name);

    class CPyObject
    {
    public:
        CPyObject() = default;
        CPyObject(PyObject* obj) : m_pPyObj(obj) {}
        //CPyObject(const char* module_name);

        CPyObject(const CPyObject& other) : CPyObject(other.m_pPyObj) { ref = true; incRef(); }
        CPyObject& operator=(const CPyObject &other);

        CPyObject(CPyObject&& other) { ref = false; m_pPyObj = other.m_pPyObj; other.m_pPyObj = nullptr; }
        CPyObject& operator=(CPyObject&& other);

        ~CPyObject() { if (ref) decRef(); }

        const CPyObject& incRef() const & { Py_XINCREF(m_pPyObj); return *this; }
        const CPyObject& decRef() const & { Py_XDECREF(m_pPyObj); return *this; }

        bool HasAttr(const char* attr_name);
        bool HasAttr(PyObject* attr_name);
        CPyObject GetAttr(const char* attr_name);
        CPyObject GetAttr(PyObject* attr_name);
        bool Valid() { return m_pPyObj != nullptr; }
        void Print() { PyObject_Print(m_pPyObj, stdout, Py_PRINT_RAW); }
        std::string String();

        CPyObject operator()(const std::vector<CPyObject>& args_, const std::map<std::string, CPyObject>& kwargs_ = {});
        CPyObject operator()(std::vector<CPyObject>&& args_ = {}, std::map<std::string, CPyObject>&& kwargs_ = {});

        PyObject* GetPtr() const { return m_pPyObj; }
    protected:
        PyObject* m_pPyObj = nullptr;
        bool ref;
    };
    

    class PyError : public std::runtime_error
    {
    public:
        PyError() :
            std::runtime_error(std::string())
        {
            PyErr_Fetch(&m_pType, &m_pValue, &m_pTrace);
        }

        bool match(PyObject* exception) const { return PyErr_GivenExceptionMatches(m_pType, exception); }

        PyObject* type() const { return m_pType; }
        PyObject* value() const { return m_pValue; }
        PyObject* trace() const { return m_pTrace; }

        virtual char const* what() const override { return CPyObject(value()).String().c_str(); }

    private:
        PyObject* m_pType;
        PyObject* m_pValue;
        PyObject* m_pTrace;
    };

    namespace Type {

        class String : public CPyObject {
        public:
            String() = default;

            explicit String(const char* str) : CPyObject(PyUnicode_FromString(str)) {} // PyUnicode_FromString in python3, PyString_FromString in python2
            explicit String(const std::string& str) : CPyObject(PyUnicode_FromString(str.c_str())) {}
            CPyObject operator()() = delete;
        };

        class Integer : public CPyObject {
        public:
            Integer() = default;

            explicit Integer(int integer) : CPyObject(PyLong_FromLong(integer)) {}
            explicit Integer(long integer) : CPyObject(PyLong_FromLong(integer)) {}
            explicit Integer(unsigned long integer) : CPyObject(PyLong_FromUnsignedLong(integer)) {}
            explicit Integer(long long integer) : CPyObject(PyLong_FromLongLong(integer)) {}
            explicit Integer(unsigned long long integer) : CPyObject(PyLong_FromUnsignedLongLong(integer)) {}
            CPyObject operator()() = delete;
        };

        class Float : public CPyObject {
        public:
            Float() = default;

            explicit Float(double f) : CPyObject(PyFloat_FromDouble(f)) {}
            CPyObject operator()() = delete;
        };

        class Tuple : public CPyObject {
        public:
            Tuple() = default;

            Tuple(const std::vector<CPyObject>& vec) : CPyObject(PyTuple_New((ssize_t)vec.size())) {
                int counter = 0;
                for (const auto &ele : vec) {
                    PyTuple_SetItem(m_pPyObj, counter++, ele.GetPtr());
                }
            }

            size_t size() const { return (size_t)PyTuple_Size(m_pPyObj); }
            bool empty() const { return size() == 0; }

        };

        class List : public CPyObject {
        public:
            List() = default;

            List(size_t size = 0) : CPyObject(PyList_New((ssize_t)size)) {
                if (m_pPyObj == nullptr) {
                    throw PyError();
                }
            }

            List(const std::vector<CPyObject>& vec) : CPyObject(PyList_New((ssize_t)vec.size())) {
                int counter = 0;
                for (const auto &ele : vec) {
                    PyList_SetItem(m_pPyObj, counter++, ele.GetPtr());
                }
            }

            size_t size() const { return (size_t)PyList_Size(m_pPyObj); }
            bool empty() const { return size() == 0; }
            void append(const CPyObject& ele) { PyList_SetItem(m_pPyObj, size(), ele.GetPtr()); }
        };
    }
}