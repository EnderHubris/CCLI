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
For flexibility ccli accepts `option` targets as (void\*), what does this mean? For string-types such as `char*, const char*` you must pass the pointer as the target, for all over primative data-types you must pass them as address-of (&).

For `flag` targets they must be address-of integers, this is a design choice as while there exists stdbool.h it is basic computer science principle that 0 - False and 1 - True, this is true for conditionals in C, non-zero values are resolved as True while the converse is False.

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
