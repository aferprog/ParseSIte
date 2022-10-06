
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "gumbo.h"
#include "AferGumboNode.h"
#include "AferFormat.h"

std::string loadData(std::string filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        std::cout << "File " << filename << " not found!\n";
        exit(EXIT_FAILURE);
    }

    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
}


int main(int argc, char** argv) {
    
    std::string page = loadData("./html_tests/yakaboo_2.html");
    std::string format= loadData("./parsing_format/yakaboo_1.html");
    {
        auto books = AferFormat().get_formated_enteties(format.c_str(), page.c_str());

        for (const auto& book : books) {
            for (const auto& pair : book)
                std::cout << pair.first << " = " << pair.second << std::endl;
            std::cout << std::endl;
        }
    }

}