#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "AferFormat.h"
#include <string_view>

std::string loadData(std::string_view filename) {
    std::ifstream in(filename.data(), std::ios::in | std::ios::binary);
    if (!in) return "";

    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
}

int entered_unknown_command(int argc, char** argv) {
    puts("parseSite -h\tdisplay possible options");
    return 1;
}

int print_help(char** argv) {
    if (strcmp(argv[1], "-h")!=0)
        return entered_unknown_command(1, argv);
    puts("parseSite -h\tdisplay possible options");
    puts("parseSite <path to html documet> <path to format document> [output file]\textract entities in standart datastream of file");
    return 0;
}

std::vector<AferEntity> extract(std::string_view html_file, std::string_view format_file) {
    std::string page = loadData(html_file);
    std::string format = loadData(format_file);

    if (page.empty())
        puts("Incorrect html documet.");
    if (format.empty())
        puts("Incorrect format document.");

    if (page.empty() || format.empty())
        return {};

    return AferFormat().get_formated_enteties(format.c_str(), page.c_str());
}

int perform_in_console(char** argv) {
    auto res = extract(argv[1], argv[2]);
    if (res.empty())
        return 1;
    for (const auto& el: res) {
        for (const auto& pair : el)
            std::cout << pair.first << " = " << pair.second << std::endl;
        std::cout << std::endl;
    }
    return 0;
}
int perform_in_file(char** argv) {
    std::ofstream out(argv[3], std::ios::out);
    if (!out) {
        puts("incorrect output file.");
        return 1;
    }

    auto res = extract(argv[1], argv[2]);
    if (res.empty())
        return 1;

    for (const auto& el : res) {
        for (const auto& pair : el)
            out << pair.first << " = " << pair.second << std::endl;
        out << std::endl;
    }
    return 0;
}


int main(int argc, char** argv) {
    if (argc == 2)
        return print_help(argv);
    else if (argc == 3)
        return perform_in_console(argv);
    else if (argc == 4)
        return perform_in_file(argv);
    else
        return entered_unknown_command(argc, argv);
        
  /*  std::string page = loadData("./Examples/barnesandnobel/page.html");
    std::string format= loadData("./Examples/barnesandnobel/format.html");

    auto res = AferFormat().get_formated_enteties(format.c_str(), page.c_str());
    for (const auto& ent : res) {
        for (const auto& pair : ent)
            std::cout << pair.first << " = " << pair.second << std::endl;
        std::cout << std::endl;
    }*/
}
