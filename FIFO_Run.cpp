#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h> // For POSIX directory operations
#include <sys/stat.h> // For stat() function

int main() {
    // Path to the folder containing trace files
    std::string folder_path = "trace_files";

    // Open the output file
    std::ofstream output_file("output.txt");

    // Check if the output file is opened successfully
    if (!output_file.is_open()) {
        std::cerr << "Error: Unable to open output file." << std::endl;
        return 1;
    }

    // Directory pointer
    DIR* dir;
    // dirent structure for directory entry
    struct dirent* entry;
    // Open the directory
    dir = opendir(folder_path.c_str());
    if (!dir) {
        std::cerr << "Error: Unable to open directory." << std::endl;
        return 1;
    }

    // Iterate over files in the directory
    while ((entry = readdir(dir)) != nullptr) {
        std::string file_name = entry->d_name;
        // Check if it's a file and ends with .tr extension
        if (entry->d_type == DT_REG && file_name.substr(file_name.find_last_of(".") + 1) == "tr") {
            // Construct the command
            std::string command = "./webcachesim " + folder_path + "/" + file_name + " FIFO 10";
            // Execute the command and redirect the output to the output file
            int result = system((command + " >> output.txt").c_str());
            // Check if the command execution was successful
            if (result != 0) {
                output_file << "Error executing command for file: " << file_name << std::endl;
            }
        }
    }

    // Close the directory
    closedir(dir);
    // Close the output file
    output_file.close();

    std::cout << "Command executed on all trace files. Output is written to output.txt" << std::endl;

    return 0;
}
