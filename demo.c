#include "cli.h"

int main(int argc, char** argv) {
    char* oFileName = NULL;
    int threadCount = 4;
    double d_val = 0.0;

    App app = createApp("My Awesome Demo");
    app.AddOption("-o", "--output", "Output File", O_STRING, &oFileName, &app);
    app.AddOption("-t", "--threads", "Number of Concurrent Threads", O_INTEGER, &threadCount, &app);
    app.AddOption("-d", "--deposit", "Amount to Deposit", O_DOUBLE, &d_val, &app);

    parseCLI(&app, argc, argv);

    printf("Outfile -> %s\n", oFileName ? oFileName : "no output specified");
    printf("Thread Count -> %d\n", threadCount);
    printf("Deposit Value -> %f\n", d_val);
}