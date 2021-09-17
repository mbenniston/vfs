#include <iostream>
#include <vfs.hpp>

#include "myBundle.hpp"

int main()
{
    vfs::VirtualFS fs;
    fs.addGlobalBundle(gen::bundle);

    std::cout << "printing files in bundle: " << std::endl;
    for(auto file : gen::bundle.files)
    {
        auto filePtr = fs.getFile(std::string(file.first));
        std::cout << file.first << std::endl;
        for(vfs::byte_t b : filePtr.read().data())
        {
            std::cout << static_cast<char>(b);
        }
        std::cout << std::endl;
    }

    return 0;
}