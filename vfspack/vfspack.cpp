#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <argparse_nowarn.hpp>

void writeSource(std::ostream& sourceWriter, const std::string& bundleName, const std::string& namespaceName, const std::vector<std::string>& files);
void writeHeader(std::ostream& headerWriter, const std::string& bundleName, const std::string& namespaceName);

std::vector<std::string> unpackPath(const std::string& path);

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("vfspack");

    program.add_description("Program to take in a list of input files and collate them together into a C++ source file and header to be used with vfs");

    program.add_argument("output_source")
        .help("The path of the generated source file");

    program.add_argument("output_header")
        .help("The path of the generated header file");

    program.add_argument("--bundle_name")
        .default_value(std::string("bundle"))
        .help("The variable name given to the bundle");

    program.add_argument("--namespace_name")
        .default_value(std::string("gen"))
        .help("The namespace that will be used to enclose the bundle");

    program.add_argument("--recursive")
        .default_value(false)
        .implicit_value(true)
        .help("Recursively descends into directories and packs all files within them");

    program.add_argument("input_files")
        .default_value(std::vector<std::string>{})
        .remaining()
        .help("The files that will be packed into the generated source");

    try
    {
        program.parse_args(argc, argv);
    }
    catch(const std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
        std::cout << program;
        return 1;
    }

    std::ofstream sourceWriter{program.get<std::string>("output_source")};
    std::ofstream headerWriter{program.get<std::string>("output_header")};

    auto files = program.get<std::vector<std::string>>("input_files");
    
    std::string bundleName = program.get<std::string>("--bundle_name");
    std::string namespaceName = program.get<std::string>("--namespace_name");
    
    if(program.get<bool>("--recursive"))
    {
        std::vector<std::string> newFiles;
        for(const auto& file : files)
        {
            auto paths = unpackPath(file);
            newFiles.insert(newFiles.end(), paths.begin(), paths.end());            
        }

        files = newFiles;
    }

    writeHeader(headerWriter, bundleName, namespaceName);
    writeSource(sourceWriter, bundleName, namespaceName, files);

    return 0;
}

static std::size_t getFileSize(const std::string& path)
{
    std::ifstream file{path, std::ios::binary};
    file.seekg(0, std::ios::end);
    auto size = file.tellg();

    if(size < 0)
    {
        throw std::runtime_error("Could not determine size of file!");
    }

    return static_cast<std::size_t>(size);
}

std::vector<std::string> unpackPath(const std::string& path)
{
    std::vector<std::string> outputPaths;

    if(std::filesystem::is_directory(path))
    {
        auto itr = std::filesystem::recursive_directory_iterator(path);
        for(const auto& childPath : itr)
        {
            if(!std::filesystem::is_directory(childPath))
            {
                outputPaths.push_back(childPath.path().generic_string());
            }
        }
    }
    else
    {
        outputPaths.push_back(path);
    }

    return outputPaths;
}

void writeHeader(std::ostream& headerWriter, const std::string& bundleName, const std::string& namespaceName)
{
    headerWriter << "#pragma once\n";
    headerWriter << "#include <vfs_bundle_def.hpp>\n\n";
    headerWriter << "namespace " << namespaceName << "\n{\n";
    headerWriter << "\textern vfs::Bundle " << bundleName << ";\n";
    headerWriter << "}\n";
}

void writeSource(std::ostream& sourceWriter, const std::string& bundleName, const std::string& namespaceName, const std::vector<std::string>& files)
{
    std::vector<std::size_t> fileSizes;
    fileSizes.reserve(files.size());

    std::size_t total = 0;
    for(const auto& path : files)
    {
        std::size_t size = getFileSize(path);
        total += size;

        fileSizes.push_back(size);
    }

    // preamble
    sourceWriter << "#include <array>\n";
    sourceWriter << "#include <vfs_bundle_def.hpp>\n\n";
    sourceWriter << "namespace " << namespaceName << "\n{\n";
    sourceWriter << "\tstatic std::array<vfs::byte_t," << total << "> " << bundleName << "_blob = {";

    //data
    for(const auto& path : files)
    {
        std::ifstream file{path, std::ios::binary};

        char c;
        while(file.get(c))
        {
            sourceWriter << "0x" << std::hex << (static_cast<int>(c) & 0xFF) << ",";
        }
    }

    sourceWriter << std::dec;

    // postamble
    sourceWriter << "};\n";

    sourceWriter << "\tvfs::Bundle " << bundleName << "{ " << bundleName << "_blob, {";
    
    std::size_t lastFileEnd = 0;

    for(std::size_t i = 0; i < files.size(); i++)
    {
        std::cout << "packing file: \"" << files.at(i) << "\"" << std::endl;

        auto size = fileSizes.at(i);
        sourceWriter << "{\"" << files.at(i)  <<  "\", {" << lastFileEnd << "," << size << "}},";
        lastFileEnd += size;
    }

    sourceWriter << "} };\n";
    sourceWriter << "}";
}
