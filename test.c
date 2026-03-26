#include "cli.h"

int main(int argc, char** argv) {
    int verbose = 0;
    char* oFileName = NULL;

    App app = createApp("My Awesome Program");

    app.AddFlag("-v", "--verbose", "Show Full Output", &verbose, &app);
    app.AddOption("-o", "--output", "Output File", oFileName, &app);

    parseCLI(&app, argc, argv);
}