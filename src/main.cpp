#include "sexpression.h"
#include <unistd.h>

static void processChunk(const string &src) {
    Reader reader(src);

    try {
        while (reader.hasMoreStuff()) {
            shared_ptr<SExpression> result = eval(reader.read());
            
            if (isVoid(result)) continue;

            print(result);

            cout << "\n";
        }
    } catch (const exception &ex) {
        cerr << "there was an error -> " << ex.what() << endl;
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

        bool inString = false;
        for (size_t i = 0; i < line.size(); i++) {
            char c = line[i];

            if (inString) {
                if (c == '\\') i++;
                else if (c == '"') inString = false;
            }
            else if (c == '"') inString = true;
            else if (c == ';') break;
            else if (c == '(') depth++;
            else if (c == ')') depth--;
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