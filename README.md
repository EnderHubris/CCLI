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

  app.AddFlag("-v", "--verbose", "Show Full Output", &verbose, &app);
  app.AddOption("-o", "--output", "Output File", oFileName, &app);

  parseCLI(&app, argc, argv);
}
```
This project is not perfect, there may be bugs, feel free to post an issue with details on how to recreate the bug you are experiencing to make the debugging process quick and effective.

## Documentation
For flexibility ccli accepts targets as (void\*), what does this mean? For string-types such as `char*, const char*` you must pass the pointer as the target, for all over primative data-types you must pass them as address-of (&). User-Defined typedefs may work so long as they are not created via dynamic allocation.
```c
#include "cli.h"
// ./program -o output.txt -t 25 -d 3.14
int main(int argc, char** argv) {
  char* oFileName = NULL;
  int threadCount = 4;
  double d_val = 0.0;

  App app = createApp("My Awesome Program");
  app.AddOption("-o", "--output", "Output File", oFileName, &app);
  app.AddOption("-t", "--threads", "Number of Concurrent Threads", &threadCount, &app);
  app.AddOption("-d", "--deposit", "Amount to Deposit", &d_val, &app);

  parseCLI(&app, argc, argv);
}
```
