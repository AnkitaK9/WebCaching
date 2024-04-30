#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // For std::min_element
#include <limits> // For std::numeric_limits
#include <dirent.h> // For POSIX directory operations

// Function to execute a command and return execution time
double execute_command(const std::string& command) {
    // Execute the command and get execution time
    // For simplicity, let's assume the command execution time is a random number between 0 and 10
    double execution_time = static_cast<double>(rand()) / RAND_MAX * 10.0;
    return execution_time;
}

// Function to find the command with the minimum execution time
std::pair<double, std::string> find_min_execution_time(const std::vector<std::pair<double, std::string>>& execution_times) {
    auto min_execution_time_pair = *std::min_element(execution_times.begin(), execution_times.end());
    return min_execution_time_pair;
}

int main() {
    // Path to the folder containing trace files
    std::string folder_path = "trace_files_wiki";

    // Open the output file
    std::ofstream output_file("outputwikimedia.txt");
    
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
            std::string file_path = folder_path + "/" + file_name;
            std::cout << "Processing file: " << file_path << std::endl;

            // Vector to store command execution times
            std::vector<std::pair<double, std::string>> execution_times; // Pair of execution time and command with label

            // Construct the commands with labels
            std::vector<std::pair<std::string, std::string>> commands_with_labels = {
                {"./webcachesim " + file_path + " LRU 20000", "LRU"},
                {"./webcachesim " + file_path + " FIFO 20000", "FIFO"},
                {"./webcachesim " + file_path + " Filter 20000 n=10", "Filter"},
                {"./webcachesim " + file_path + " ThLRU 20000 t=19", "ThLRU"},
                {"./webcachesim " + file_path + " ExpLRU 20000 c=18", "ExpLRU"},
                {"./webcachesim " + file_path + " LRUK 20000 k=4", "LRUK"},
                {"./webcachesim " + file_path + " AdaptSize 20000 t=1000000 i=5", "AdaptSize"}
                // Add more commands as needed
            };

            // Execute each command and store execution time along with command and label
            for (const auto& command_label_pair : commands_with_labels) {
                double execution_time = execute_command(command_label_pair.first);
                execution_times.push_back(std::make_pair(execution_time, command_label_pair.second));
            }

            // Find the command with the least execution time
            auto min_execution_time_pair = find_min_execution_time(execution_times);

            // Append the output to the output file
            output_file << "File: " << file_name << std::endl;
            // for (const auto& command_label_pair : commands_with_labels) {
            //     output_file << "Execution time for command (" << command_label_pair.second << "): " << command_label_pair.first << " is " << execution_times.front().first << " seconds." << std::endl;
            // }
            output_file << "Least execution time: " << min_execution_time_pair.second << " - " << min_execution_time_pair.first << " seconds." << std::endl;
            output_file << std::endl;
        }
    }

    // Close the directory
    closedir(dir);

    // Close the output file
    output_file.close();
    std::cout << "Output is saved to outputwikimedia.txt" << std::endl;

    return 0;
}
