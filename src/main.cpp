#include "sexpression.h"
#include <unistd.h>

static void processChunk(const string &src) {
    Reader reader(src);

    try {
        while (reader.hasMoreStuff()) {
            print(reader.read());
            cout << "\n";
        }
    } catch (const exception &ex) {
        cerr << "there was an error: " << ex.what() << endl;
    }
}


int main() {
    const bool interactive = isatty(STDIN_FILENO);

    string pending;
    int depth = 0;
    string line;

    if (interactive) cout << "=> " << flush;

    while (getline(cin, line)) {
        pending += line;
        pending += '\n';

        for (char c : line) {
            if (c == '(') {
                depth++;
            }
            else if (c == ')') {
                depth--;
            }
        }

        if (depth <= 0) {
            processChunk(pending);
            pending.clear();
            depth = 0;
            if (interactive) {
                cout << "=> " << flush;
            }
        }

    }

    if (!pending.empty()) {
        processChunk(pending);
    }

    if (interactive) {
        cout << "\n";
    }

    return 0;
}