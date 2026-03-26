// Standard library utilities used throughout the program
#include <algorithm> // sort, shuffle
#include <fstream>
#include <functional>    // std::function
#include <iostream>      // cout, cerr
#include <random>        // random number generation
#include <string>        // std::string
#include <unordered_map> // hash map
#include <vector>        // dynamic arrays

// Third-party libraries

#include "json.hpp"         // nlohmann JSON library
#include "termcolor.hpp"    // colored terminal output (optional visual enhancement)
#include "usagePrinter.hpp" //

using namespace std;

// Alias to make JSON type shorter to write
using json = nlohmann::json;

// -------------------------------------------------------------
// Operation type
// -------------------------------------------------------------
// Represents a single operation that will be performed on a data structure.
// Example operations might be:
//
//   insert 42
//   contains 17
//   delete 9
//
// Each Op object stores:
//   - the operation name
//   - the integer value associated with the operation
//
struct Op {
    std::string op;
    int value;
};

// -------------------------------------------------------------
// JSON conversion helper
// -------------------------------------------------------------
// This function allows automatic conversion of Op -> JSON.
//
// Example:
//      Op o{"insert", 42};
//      json j = o;
//
// Result:
//      {"op":"insert","value":42}
//
void to_json(json &j, const Op &o) {
    j = json{{"op", o.op}, {"value", o.value}};
}

// -------------------------------------------------------------
// WorkloadGenerator
// -------------------------------------------------------------
//
// This class generates different types of workloads used for
// benchmarking data structures.
//
// A "workload" is simply a sequence of operations such as:
//
//      insert
//      contains
//      delete
//
// Different workloads simulate different real-world behaviors.
//
class WorkloadGenerator {
private:
    // Seed used for reproducible randomness
    int seed;

    // Multiplier controlling the range of random values
    int valueRangeMult;

    // ---------------------------------------------------------
    // RNG generator
    // ---------------------------------------------------------
    // Returns a fresh random number generator initialized with
    // the same seed each time.
    //
    // This ensures repeatability so that experiments can be
    // reproduced exactly.
    //
    std::mt19937 rng() const {
        return std::mt19937(seed);
    }

    // ---------------------------------------------------------
    // Generate random integer values
    // ---------------------------------------------------------
    //
    // Generates "count" random integers in the range:
    //
    //      [0 , valueRangeMult * n]
    //
    // Used by workloads to create insert / lookup values.
    //
    std::vector<int> genValues(int n, int count) const {

        std::mt19937 r = rng();

        int hi = valueRangeMult * n;

        std::uniform_int_distribution<int> dist(0, hi);

        std::vector<int> values;

        // Reserve memory in advance for efficiency
        values.reserve(count);

        for (int i = 0; i < count; i++) {
            values.push_back(dist(r));
        }

        return values;
    }

public:
    // ---------------------------------------------------------
    // Constructor
    // ---------------------------------------------------------
    WorkloadGenerator(int seed_ = 42, int valueRangeMult_ = 10)
        : seed(seed_), valueRangeMult(valueRangeMult_) {}

    // ---------------------------------------------------------
    // Workload A
    // ---------------------------------------------------------
    //
    // Random inserts followed by random lookups
    //
    // Pattern:
    //
    //      insert
    //      insert
    //      insert
    //      ...
    //      contains
    //      contains
    //
    std::vector<Op> workloadA(int n) const {

        auto inserts = genValues(n, n);
        auto lookups = genValues(n, n);

        std::vector<Op> ops;

        // Reserve capacity to avoid reallocation
        ops.reserve(2 * n);

        for (int x : inserts) {
            ops.push_back({"insert", x});
        }

        for (int x : lookups) {
            ops.push_back({"contains", x});
        }

        return ops;
    }

    // ---------------------------------------------------------
    // Workload B
    // ---------------------------------------------------------
    //
    // Same as workload A except inserts are sorted first.
    //
    // This often stresses ordered data structures differently
    // (e.g., BSTs without balancing).
    //
    std::vector<Op> workloadB(int n) const {

        auto inserts = genValues(n, n);

        // Sorted insertion pattern
        std::sort(inserts.begin(), inserts.end());

        auto lookups = genValues(n, n);

        std::vector<Op> ops;

        ops.reserve(2 * n);

        for (int x : inserts) {
            ops.push_back({"insert", x});
        }

        for (int x : lookups) {
            ops.push_back({"contains", x});
        }

        return ops;
    }

    // ---------------------------------------------------------
    // Workload C
    // ---------------------------------------------------------
    //
    // Mixed workload with random ordering of operations.
    //
    // Operation mix:
    //   50% contains
    //   25% insert
    //   25% delete
    //
    // Deletes only occur if something exists in the population.
    //
    std::vector<Op> workloadC(int n) const {

        std::mt19937 r = rng();

        int hi = valueRangeMult * n;

        std::uniform_int_distribution<int> dist(0, hi);

        int totalOps = 2 * n;

        int numContains = totalOps / 2;
        int numInsert = totalOps / 4;
        int numDelete = totalOps - numContains - numInsert;

        std::vector<std::string> opTypes;

        opTypes.reserve(totalOps);

        opTypes.insert(opTypes.end(), numContains, "contains");
        opTypes.insert(opTypes.end(), numInsert, "insert");
        opTypes.insert(opTypes.end(), numDelete, "delete");

        // Randomize the order of operations
        std::shuffle(opTypes.begin(), opTypes.end(), r);

        std::vector<Op> ops;

        ops.reserve(totalOps);

        // Keeps track of values that were inserted
        std::vector<int> population;

        for (const auto &op : opTypes) {

            int x = dist(r);

            if (op == "insert") {

                population.push_back(x);
                ops.push_back({"insert", x});

            } else if (op == "contains") {

                ops.push_back({"contains", x});

            } else { // delete

                // Only delete if something exists
                if (!population.empty()) {

                    int y = population.back();
                    population.pop_back();

                    ops.push_back({"delete", y});

                } else {

                    // fallback to contains if nothing to delete
                    ops.push_back({"contains", x});
                }
            }
        }

        return ops;
    }

    // ---------------------------------------------------------
    // Workload D
    // ---------------------------------------------------------
    //
    // Insert n items then perform heavy lookup activity.
    //
    // Pattern:
    //
    //      n inserts
    //      5n lookups
    //
    // Useful for read-heavy workloads.
    //
    std::vector<Op> workloadD(int n) const {

        auto inserts = genValues(n, n);
        auto lookups = genValues(n, 5 * n);

        std::vector<Op> ops;

        ops.reserve(6 * n);

        for (int x : inserts) {
            ops.push_back({"insert", x});
        }

        for (int x : lookups) {
            ops.push_back({"contains", x});
        }

        return ops;
    }
};

// -------------------------------------------------------------
// Tiny command-line parser
// -------------------------------------------------------------
//
// Parses arguments like:
//
//   ./workload_generator -w B -n 100 --preview 20 --json
//
struct Args {

    std::string workload = "A";

    int size = 10;

    int preview = 20;

    bool emitJson = false;

    std::string savefile = "";

    friend ostream &operator<<(ostream &os, const Args &c) {
        return os << "[w: " << c.workload << ",s: " << c.size << ",p: " << c.preview << ",ej: " << c.emitJson << ",s: " << c.savefile << "]";
    }
};

Args parseArgs(int argc, char *argv[]) {

    Args args;

    for (int i = 1; i < argc; i++) {

        std::string s = argv[i];

        if ((s == "-w" || s == "--workload") && i + 1 < argc) {

            args.workload = argv[++i];

        } else if ((s == "-n" || s == "--size") && i + 1 < argc) {

            args.size = std::stoi(argv[++i]);

        } else if ((s == "-p" || s == "--preview") && i + 1 < argc) {

            args.preview = std::stoi(argv[++i]);

        } else if (s == "--json") {

            args.emitJson = true;

        } else if (s == "--save") {

            args.savefile = argv[++i];

        } else if (s == "-h" || s == "--help") {

            // std::cout
            //     << "Usage:\n"
            //     << "  ./workload_generator [-w A|B|C|D] [-n SIZE] [--preview K] [--json]\n\n"
            //     << "Options:\n"
            //     << "  -w, --workload   Workload type (A, B, C, D)\n"
            //     << "  -n, --size       Base problem size n\n"
            //     << "  --preview        Number of operations to preview\n"
            //     << "  --json           Print all operations as JSON\n"

            UsagePrinter help;

            help.add("-w, --workload", "<TYPE>", "Workload type (A,B,C,D)");
            help.add("-n, --size", "<N>", "Base problem size N=int");
            help.add("--preview", "<K>", "Preview operations K=int");
            help.add("--json", "", "Emit JSON output");
            help.add("-s, ---save", "<FILENAME>", "Save output to a file");
            help.add("-h, --help", "", "Show help message");
            help.add("-p,--preview", "<n>", "Preview the data ");
            help.addExample("./workload_generator --json -w C -n 1000 > workload.json");
            help.addExample("./workload_generator --json -workload B -size 5000 --save workload_B_5000.json");
            help.print("workload_generator");

            std::exit(0);
        }
    }

    return args;
}

// ---------------------------------------------------------
// JSON output mode
//
// Why this exists
//
//      This mode is meant for other programs to consume, not humans.
//
// Example pipeline:
//
//      ./workload_generator --json -w C -n 1000 > workload.json
//
// Runs the generator and creates 1000 jobs of type C and writes them into a file called `workload.json`
//
//      ./workload_generator --json -w C -n 1000 | ./linkedListTest
//
// Runs the generator creating same workload as example 1, but is now piping the output to the linkelist
//  executable to be processed.
// ---------------------------------------------------------
//
void emitJson(auto ops) {

    json j = ops;

    for (auto op : j) {
        std::cout << op;
    }

    // std::cout << j.dump(2) << "\n";
}

// -------------------------------------------------------------
// main
// -------------------------------------------------------------
int main(int argc, char *argv[]) {

    // Parse command line arguments
    Args args = parseArgs(argc, argv);

    // Create workload generator
    WorkloadGenerator gen(42);

    // ---------------------------------------------------------
    // Workload dispatch table
    // ---------------------------------------------------------
    //
    // This map connects workload names to the functions that
    // generate them.
    //
    // Conceptually it behaves like:
    //
    //      workloads["A"] → gen.workloadA
    //      workloads["B"] → gen.workloadB
    //      workloads["C"] → gen.workloadC
    //      workloads["D"] → gen.workloadD
    //
    // This allows dynamic selection of a workload by name.
    //
    std::unordered_map<std::string, std::function<std::vector<Op>(int)>> workloads = {

        {"A", [&](int n) { return gen.workloadA(n); }},
        {"B", [&](int n) { return gen.workloadB(n); }},
        {"C", [&](int n) { return gen.workloadC(n); }},
        {"D", [&](int n) { return gen.workloadD(n); }}

    };

    // ---------------------------------------------------------
    // Validate workload selection
    // ---------------------------------------------------------
    // contains() requires C++20
    //
    if (!workloads.contains(args.workload)) {

        std::cerr << "Error: workload must be one of A, B, C, D\n";

        return 1;
    }

    // ---------------------------------------------------------
    // Generate workload operations
    // ---------------------------------------------------------
    //
    // Equivalent to calling:
    //
    //      gen.workloadA(n)
    //      gen.workloadB(n)
    //      gen.workloadC(n)
    //      gen.workloadD(n)
    //
    auto ops = workloads[args.workload](args.size);

    if (args.emitJson) {
        emitJson(ops);
    }

    if (args.preview && args.savefile.length() == 0) {
        // ---------------------------------------------------------
        // Preview mode
        // ---------------------------------------------------------
        //
        std::cout << "\n----------------------------------\n";
        std::cout << "Workload: " << args.workload << "\n";
        std::cout << "n: " << args.size << "\n";
        std::cout << "Total operations: " << ops.size() << "\n";
        std::cout << "----------------------------------\n\n";

        int preview = std::min<int>(args.preview, static_cast<int>(ops.size()));

        // Print first preview operations
        for (int i = 0; i < preview; i++) {

            json j = ops[i];

            std::cout << j.dump() << "\n";
        }

        if (preview < static_cast<int>(ops.size())) {
            std::cout << "...\n";
        }
    }

    // Example:
    //./work_generator -w A -n 1000 --save work_A_1000.json
    //
    // Will save a file called work_A_1000.json with jobtype A and 1000 jobs.
    if (args.savefile.length() > 0) {
        cout << "Saving json to: " << args.savefile << endl;
        json j = ops;
        // write prettified JSON to another file
        std::ofstream o(args.savefile);
        o << std::setw(4) << j << std::endl;
    }

    return 0;
}