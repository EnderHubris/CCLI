#include "cli.h"

int main(int argc, char** argv) {
    int verbose = 0;
    char* oFileName = NULL;

    App app = createApp("My Awesome Program");

    app.AddFlag("-v", "--verbose", "Show Full Output", F_INTEGER, &verbose, &app);
    app.AddOption("-o", "--output", "Output File", O_STRING, oFileName, &app);

    parseCLI(&app, argc, argv);

    if (verbose == 1) {
        printf("Hello Verbose\n");
    }
}