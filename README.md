# CCLI
Simple Command Line Parser allowing C programs to have a simple CLI, inspired by the CLI11 project for C++

## Usage
All you need to do is download the [cli.h](https://raw.githubusercontent.com/EnderHubris/CCLI/refs/heads/main/cli.h) file and include it in your C main.
```c
#include "cli.h"

int main(int argc, char** argv) {
    int verbose = 0;
    char* oFileName = NULL;

    App app = createApp("My Awesome Program");

    app.AddFlag("-v", "--verbose", "Show Full Output", F_INTEGER, &verbose, &app);
    app.AddOption("-o", "--output", "Output File", O_STRING, &oFileName, &app);

    parseCLI(&app, argc, argv);

    if (verbose == 1) {
        printf("Hello Verbose\n");
    }
}
```
This project is not perfect, there may be bugs, feel free to post an issue with details on how to recreate the bug you are experiencing to make the debugging process quick and effective.

## Documentation
For flexibility ccli accepts `option` targets as (void\*), what does this mean? For string-types such as `char*, const char*` you must pass these targets as a pointer
via (&), ie if your passing say `char* name` variable into an option the argument should be `&name`.

For `flag` targets they must be address-of integers or bools, it is a basic computer science principle that for ints 0 - False and 1 - True, this is true for conditionals in C, non-zero values are resolved as True while the converse is False.

**NOTE**: When setting options/flags you must specify the [type of the target](#option-and-flag-types) it is modifying, see the examples below.

```c
#include "cli.h"
// gcc demo.c -o demo
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
```

## Option and Flag Types
### Flags:
- F_BOOLEAN
- F_INTEGER

### Options:
- O_STRING
- O_INTEGER
- O_DOUBLE
- O_FLOAT
- O_SIZE_T