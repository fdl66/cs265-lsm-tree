#include <iostream>

#include "lsm_tree.h"
#include "sys.h"
#include "unistd.h"

using namespace std;

void command_loop(LSMTree& tree) {
    char command;
    KEY_t key_a, key_b;
    VAL_t val;
    string file_path;

    while (cin >> command) {
        switch (command) {
        case 'p':
            cin >> key_a >> val;

            if (val < VAL_MIN || val > VAL_MAX) {
                die("Could not insert value " + to_string(val) + ": out of range.");
            } else {
                tree.put(key_a, val);
            }

            break;
        case 'g':
            cin >> key_a;
            tree.get(key_a);
            break;
        case 'r':
            cin >> key_a >> key_b;
            tree.range(key_a, key_b);
            break;
        case 'd':
            cin >> key_a;
            tree.del(key_a);
            break;
        case 'l':
            cin.ignore();
            getline(cin, file_path);
            // Trim quotes
            tree.load(file_path.substr(1, file_path.size() - 2));
            break;
        case 's':
            tree.stats();
            break;
        default:
            die("Invalid command.");
        }
    }
}

int main(int argc, char *argv[]) {
    int opt, buffer_num_pages, buffer_max_entries, depth, fanout, num_threads;
    float merge_ratio;

    buffer_num_pages = DEFAULT_BUFFER_NUM_PAGES;
    depth = DEFAULT_TREE_DEPTH;
    fanout = DEFAULT_TREE_FANOUT;
    num_threads = DEFAULT_THREAD_COUNT;
    merge_ratio = DEFAULT_MERGE_RATIO;

    while ((opt = getopt(argc, argv, "b:d:f:t:m:")) != -1) {
        switch (opt) {
        case 'b':
            buffer_num_pages = atoi(optarg);
            break;
        case 'd':
            depth = atoi(optarg);
            break;
        case 'f':
            fanout = atoi(optarg);
            break;
        case 't':
            num_threads = atoi(optarg);
            break;
        case 'm':
            merge_ratio = atof(optarg);
            break;
        default:
            die("Usage: " + string(argv[0]) + " "
                "[-b number of pages in buffer] "
                "[-d number of levels] "
                "[-f level fanout] "
                "[-t number of threads] "
                "[-m level merge ratio] "
                "<[workload]");
        }
    }

    buffer_max_entries = buffer_num_pages * getpagesize() / sizeof(entry_t);
    LSMTree tree(buffer_max_entries, depth, fanout, num_threads, merge_ratio);
    command_loop(tree);

    return 0;
}
