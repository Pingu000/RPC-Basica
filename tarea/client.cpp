#include <iostream>
#include <sstream>
#include <experimental/filesystem>
#include <fstream>
#include <vector>
#include "filemanager.h"

int main(int argc, char** argv)
{
    FileManager fm("FileManagerDir");
    std::string command;
    std::string fileName;
    std::string line;
    do {
        std::cout << "Enter command:" << std::endl;
        std::getline(std::cin, line);

        std::stringstream ss(line);
        ss >> command;

        if (command == "ls") {
            std::cout << "Listing files in local path" << std::endl;
            experimental::filesystem::path directorypath = "./";

            for (const auto &entry : experimental::filesystem::directory_iterator(directorypath)) {
                if (is_regular_file(entry))
                    std::cout << entry.path() << std::endl;
            }
            std::cout << std::endl;
        } else if (command == "lls") {
            std::cout << "Listing files fileManager path" << std::endl;
            auto files = fm.listFiles();

            for (const auto &entry : files) {
                std::cout << entry << std::endl;
            }
            std::cout << std::endl;
        } else if (command == "upload") {
            ss >> fileName;
            if (fileName.length() > 0) {
                std::cout << "Coping file " << fileName << " in to the FileManager path" << std::endl;
                std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);
                if (file.is_open()) {
                    std::vector<unsigned char> data;
                    std::ifstream::pos_type fileSize = file.tellg();
                    file.seekg(0, std::ios::beg);
                    data.resize(static_cast<size_t>(fileSize));
                    file.read(reinterpret_cast<char*>(data.data()), fileSize);
                    file.close();
                    std::cout << "Reading file: " << fileName << " " << data.size() << " bytes" << std::endl;

                    fm.writeFile(fileName, data);
                } else {
                    std::cout << "ERROR: No such file \"" << fileName << "\"found\n";
                }
                std::cout << std::endl;
            }
        } else if (command == "download") {
            ss >> fileName;
            if (fileName.length() > 0) {
                std::cout << "Coping file " << fileName << " from the FileManager path in to local path" << std::endl;
                std::ofstream file(fileName, std::ios::out | std::ios::binary);
                if (file.is_open()) {
                    std::vector<unsigned char> data;
                    fm.readFile(fileName, data);
                    file.write(reinterpret_cast<char*>(data.data()), data.size());
                    file.close();
                    std::cout << "Writting file: " << fileName << " " << data.size() << " bytes" << std::endl;
                } else {
                    std::cout << "ERROR: No such file \"" << fileName << "\"found\n";
                }
                std::cout << std::endl;
            }
        } else if (command == "lls") {
            std::cout << "Listing files fileManager path" << std::endl;
            auto files = fm.listFiles();

            for (const auto &entry : files) {
                std::cout << entry << std::endl;
            }
            std::cout << std::endl;
        } else if (command == "exit()")
            std::cout << "Exitting system" << std::endl;
        else {
            std::cout << "ERROR: command " << command << " not implemented" << std::endl;
        }

    } while (command != "exit()");
    return 0;
}
