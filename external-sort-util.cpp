#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <bitset>

// 100 MB
const uint64_t kMemoryAvailable = 100 * 1024 * 1024;

// Shows help on command line arguments.
static void show_usage(std::string name) {
    std::cerr
        << "Options:\n"
        << "\t-h,--help\t\tShow this help message\n"
        << "\t--input_file INPUT_FILE\tSpecify path to the input file\n"
        << "\t--output_file OUTPUT_FILE\tSpecify path to the output file\n"
        << std::endl;
}

// Reads one bucket of numbers, sorts them and writes sorted
// sequence into file named intermediate_output_{bucket_number}.
// One bucket normally contains exactly bucket_size numbers except
// when input_file does not have that much numbers left.
void WriteBucket(std::ifstream& input_file, uint64_t bucket_size, size_t bucket_number) {
    // Read a bucket of numbers from the input file.
    std::vector<double> numbers;
    for (size_t i = 0; i < bucket_size && input_file; ++i) {
        double current_number;
        input_file >> current_number;
        // Check that end of input was not reached by the previous
        // operation.
        if (input_file) {
            numbers.push_back(current_number);
        }
    }

    // Sort the bucket.
    sort(numbers.begin(), numbers.end());

    // Write bucket into a file.
    std::ofstream output_file("intermediate_output_" + std::to_string(bucket_number));
    for (double number : numbers) {
        output_file << number << std::endl;
    }
}

// Merges multiple sorted files into one sorted file.
void MergeFiles(std::vector<std::ifstream>& input_files, std::ofstream& output_file) {
    size_t number_of_files = input_files.size();

    // Get first numbers from the files as current positions.
    // Assume that every file contains at least one number.
    std::vector<double> current_positions(number_of_files);
    for (size_t i = 0; i < number_of_files; ++i) {
        input_files[i] >> current_positions[i];
    }

    while (true) {
        // Get index of file containing the smallest number at
        // its current_position.
        size_t file_with_min = 0;
        // Check if all the files are at end.
        bool all_at_end = true;
        for (size_t i = 0; i < number_of_files; ++i) {
            // If the current file is not at end.
            if (input_files[i]) {
                // Always update file_with_min if all the previous files
                // were at end.
                if (all_at_end || current_positions[i] < current_positions[file_with_min]) {
                    file_with_min = i;
                }
                all_at_end = false;
            }
        }

        // Return if all the files are already processed.
        if (all_at_end) {
            return;
        }

        // Write the smallest number to the output file
        // and update the corresponding current_position.
        double number = current_positions[file_with_min];
        output_file << number << std::endl;
        input_files[file_with_min] >> current_positions[file_with_min];
    }
}

int main(int argc, char* argv[]) {
    // Parse command line options.
    if (argc < 4) {
        show_usage(argv[0]);
        return 1;
    }
    std::string input_file_path, output_file_path;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else if (arg == "--input_file") {
            if (i + 1 < argc) { //  Make sure we aren't at the end of argv
                input_file_path = argv[++i]; //  Increment i so we don't get the argument as the next argv[i].
            } else { //  Uh-oh, there was no argument to the input_file option.
                std::cerr << "--input_file option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "--output_file") {
            if (i + 1 < argc) { //  Make sure we aren't at the end of argv
                output_file_path = argv[++i]; //  Increment i so we don't get the argument as the next argv[i].
            } else { //  Uh-oh, there was no argument to the output_file option.
                std::cerr << "--output_file option requires one argument." << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Unknown option " << arg << std::endl;
            return 1;
        }
    }

    uint64_t bucket_size = (kMemoryAvailable / sizeof(double) - sizeof(std::vector<double>)) * 0.7;

    std::ifstream input_file(input_file_path);
    size_t number_of_buckets = 0;
    while (input_file) {
        WriteBucket(input_file, bucket_size, number_of_buckets);
        ++number_of_buckets;   
    }

    std::vector<std::ifstream> intermediate_outputs;
    for (size_t i = 0; i < number_of_buckets; ++i) {
        intermediate_outputs.push_back(std::ifstream("intermediate_output_" + std::to_string(i)));
    }
    
    std::ofstream output_file(output_file_path);

    MergeFiles(intermediate_outputs, output_file);
    output_file.close();
    
    for (size_t i = 0; i < number_of_buckets; ++i) {
        intermediate_outputs[i].close();
        remove(("intermediate_output_" + std::to_string(i)).c_str());
    }
}
