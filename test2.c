#include "cli.h"
#include <stdbool.h>

int main(int argc, char** argv) {
    bool verbose = false;
    char* oFileName = NULL;

    App app = createApp("My Second Awesome Program");

    app.AddFlag("-v", "--verbose", "Show Full Output", F_BOOLEAN, &verbose, &app);
    app.AddOption("-o", "--output", "Output File", O_STRING, oFileName, &app);

    parseCLI(&app, argc, argv);

    if (verbose) {
        printf("Hello Verbose!\n");
    }
}
