#include "pyembed.h"

#ifdef _MSC_VER
#include <windows.h>
#include <fcntl.h>
#endif

namespace PyEmbed
{
    CPyObject Import(const char* name)
    {
        PyObject* _module = PyImport_ImportModule(name);
        if (!_module) {
            throw PyError();
        }

        return CPyObject(_module);
    }

    CPyObject &CPyObject::operator=(const CPyObject &other)
    {
        ref = true;
        other.incRef();
        decRef();
        m_pPyObj = other.m_pPyObj;
        return *this;
    }

    CPyObject &CPyObject::operator=(CPyObject &&other)
    {
        ref = false;
        if (this != &other)
        {
            decRef();
            m_pPyObj = other.m_pPyObj;
            other.m_pPyObj = nullptr;
        }
        return *this;
    }

    bool CPyObject::HasAttr(const char *attr_name)
    {
        return PyObject_HasAttrString(m_pPyObj, attr_name) == 1;
    }

    bool CPyObject::HasAttr(PyObject *attr_name)
    {
        return PyObject_HasAttr(m_pPyObj, attr_name) == 1;
    }

    CPyObject CPyObject::GetAttr(const char *attr_name)
    {
        PyObject *_attr = PyObject_GetAttrString(m_pPyObj, attr_name);
        if (!_attr)
        {
            throw PyError();
        }
        return CPyObject(_attr);
    }

    CPyObject CPyObject::GetAttr(PyObject *attr_name)
    {
        PyObject *_attr = PyObject_GetAttr(m_pPyObj, attr_name);
        if (!_attr)
        {
            throw PyError();
        }
        return CPyObject(_attr);
    }

    std::string CPyObject::String()
    {
#ifdef _MSC_VER
        // Implementation on Windows platform
        HANDLE hPipeRead = INVALID_HANDLE_VALUE;
        HANDLE hPipeWrite = INVALID_HANDLE_VALUE;
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0))
        {
            printf("create pipe failed\n");
            Print();
        }
        int fhandle = _open_osfhandle((intptr_t)hPipeWrite, _O_WRONLY);
        FILE *pipef = _fdopen(fhandle, "w");

        char buf[4096];
        if (-1 != PyObject_Print(m_pPyObj, pipef, Py_PRINT_RAW))
        {
            fclose(pipef);
            DWORD readBytesTotal = 0;
            DWORD readBytes = 0;
            while (true)
            {
                if (ReadFile(hPipeRead, buf + readBytesTotal, 1, &readBytes, NULL))
                {
                    if (readBytes == 0)
                        break;
                    readBytesTotal += readBytes;
                }
                else
                {
                    break;
                }
            }
        }
        return std::string(buf);
#else
        std::cout << "Not Implemented." << std::endl;
        return std::string();        
#endif
    }

    CPyObject CPyObject::operator()(const std::vector<CPyObject> &args_, const std::map<std::string, CPyObject> &kwargs_)
    {
        CPyObject args{PyTuple_New((ssize_t)args_.size())};
        int counter = 0;
        for (const auto &arg : args_)
        {
            PyTuple_SetItem(args.m_pPyObj, counter++, arg.m_pPyObj);
        }

        CPyObject kwargs{PyDict_New()};
        for (const auto &arg : kwargs_)
        {
            PyDict_SetItemString(kwargs.m_pPyObj, arg.first.c_str(), arg.second.m_pPyObj);
        }

        PyObject *result = PyObject_Call(m_pPyObj, args.m_pPyObj, kwargs.m_pPyObj);
        if (result == nullptr)
        {
            throw PyError();
        }

        return CPyObject(result);
    }

    CPyObject CPyObject::operator()(std::vector<CPyObject> &&args_, std::map<std::string, CPyObject> &&kwargs_)
    {
        CPyObject args{PyTuple_New((ssize_t)args_.size())};
        int counter = 0;
        for (const auto &arg : args_)
        {
            PyTuple_SetItem(args.m_pPyObj, counter++, arg.m_pPyObj);
        }

        CPyObject kwargs{PyDict_New()};
        for (const auto &arg : kwargs_)
        {
            PyDict_SetItemString(kwargs.m_pPyObj, arg.first.c_str(), arg.second.m_pPyObj);
        }

        PyObject *result = PyObject_Call(m_pPyObj, args.m_pPyObj, kwargs.m_pPyObj);
        if (result == nullptr)
        {
            throw PyError();
        }

        return CPyObject(result);
    }
}