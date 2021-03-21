#include "pyembed.h"

#include <string>
#include <iostream>


void PrintSysPath(){
	auto sysPath = PyEmbed::Import("sys").GetAttr("path");
    sysPath.GetAttr("append")({ PyEmbed::Type::String("./") });  // call sys.path.append method
	sysPath.Print();
}

PyEmbed::CPyObject PySqrt(double number){
    auto sqrt = PyEmbed::Import("math").GetAttr("sqrt");
    auto result = sqrt({ PyEmbed::Type::Float(number) });  // call math.sqrt method
    if (!result.Valid()) {
        throw PyEmbed::PyError();
    }
    return result;
}

int main(int argc, char* argv[])
{
    PyEmbed::PyInterpreterScoped guard{};

    // import unknown
    // No module named 'unknown'
    try{
        PyEmbed::Import("unknown");
    }
    catch(const PyEmbed::PyError& e){
        std::cout << e.what() << std::endl;
    }

    // import sys
    // print(sys.path)
    PrintSysPath();

    std::cout << "\n---------\n";

    if (argc > 1){
        const char* arg = argv[1];
        double raw_num;
        try{
            raw_num = std::stod(arg);
        }
        catch(std::invalid_argument & e){
            std::cout << arg << " can't be converted to a number, "
                      << e.what() << std::endl;
            raw_num = 0.0f;
        }   
        try{
            auto result = PySqrt(raw_num);
            std::cout << "The square root of " << raw_num << " is " 
                    << result.String() << std::endl;
        }
        catch(const PyEmbed::PyError& e){
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}