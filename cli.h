#ifndef CLI
#define CLI

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char* description;
    const char* opString;
    const char* altString;
} Info;

typedef enum {
    tOption,
    tFlag
} dType;

typedef struct {
    Info info;
    void* target;       // can be anything
} Option;

typedef struct {
    Info info;
    int* target; // 0 - false & 1 - true
} Flag;

typedef struct App App; // Forward Definition so App can reference itself without using struct keyword
struct App {
    const char* description;

    Option** options;
    size_t optCount;

    Flag** flags;
    size_t flagCount;

    // function ptrs
    void (*AddOption)(
            const char*, const char*, const char*,
            void*, App*);
    void (*AddFlag)(
            const char*, const char*, const char*,
            int*, App*);
};

//-------------------------------------------
//  Private Methods
//  |___ static keyword for methods means it
//       cannot be referenced outside this file

// lacking but multiple options attached to the same target seems
// unneccessary
static int matchingOptions(Option* a, Option* b) {
    return (a->target == b->target) ? 1 : 0;
}

static int matchingFlags(Flag* a, Flag* b) {
    return (a->target == b->target) ? 1 : 0;
}

static int Contains(void** collection, size_t count, dType t, void* elem) {
    if (!collection) return 1;
    if (!elem) return 1;

    for (size_t i = 0; i < count; ++i) {
        if (t == tOption) {
            // implicit casting performed
            Option* cOpt = collection[i];
            Option* nOpt = elem;

            if (matchingOptions(cOpt,nOpt)) return 1;
        } else if (t == tFlag) {
            Flag* cFlag = collection[i];
            Flag* nFlag = elem;
            
            if (matchingFlags(cFlag,nFlag)) return 1; // is contained
        }
    }
    return 0; // not contained
}

static void AddOption(
        const char* optStr,
        const char* altStr,
        const char* desc,
        void* tar,
        App* app
    ) {
    if (!app) { return; }

    Option* nOpt = malloc(sizeof(Option));
    if (!nOpt) {
        return;
    }

    // init new option
    nOpt->info = (Info){desc,optStr,altStr};
    nOpt->target = tar;

    // do not insert duplicate options
    if (
        app->optCount > 0 &&
        Contains(
            (void **)&app->options, // collection to iterate
            app->optCount,          // collection size
            tOption,                // collection element type
            nOpt                    // new option we want to add
        ) == 1
    ) { return; }

    // perform dynamic memory actions
    Option** tmp = app->options;
    app->options = malloc( (app->optCount + 1) * sizeof(Option));
    if (!app->options) {
        app->options = tmp;
        return;
    }

    // move original data back into main structure
    for (size_t i = 0; i < app->optCount; ++i) {
        app->options[i] = tmp[i];
    }
    
    free(tmp);

    // append new option
    app->options[app->optCount] = nOpt;
    ++(app->optCount);
}

static void AddFlag(
        const char* optStr,
        const char* altStr,
        const char* desc,
        int* tar,
        App* app
    ) {
    if (!app) { return; }
    if (!tar) { return; }

    Flag* nFlag = malloc(sizeof(Flag));
    if (!nFlag) {
        return;
    }

    // init new flag
    nFlag->info = (Info){desc,optStr,altStr};
    nFlag->target = tar;

    // do not insert duplicate options
    if (
        app->optCount > 0 &&
        Contains(
            (void **)&app->flags,   // collection to iterate
            app->flagCount,         // collection size
            tFlag,                  // collection element type
            nFlag                   // new flag we want to add
        ) == 1
    ) { return; }

    // perform dynamic memory actions
    Flag** tmp = app->flags;
    app->flags = malloc( (app->flagCount + 1) * sizeof(Flag));
    if (!app->flags) {
        app->flags = tmp;
        return;
    }

    // move original data back into main structure
    for (size_t i = 0; i < app->flagCount; ++i) {
        app->flags[i] = tmp[i];
    }

    free(tmp);

    // append new flag
    app->flags[app->flagCount] = nFlag;
    ++(app->flagCount);
}

static void ProcessOption(void* target, char* str) {
    // check if string contains '.' or 'e'/'E' => treat as double
    int isDouble = 0;
    for (const char *p = str; *p; p++) {
        if (*p == '.' || *p == 'e' || *p == 'E') {
            isDouble = 1;
            break;
        }
    }

    char *end;

    if (isDouble) {
        double d = strtod(str, &end);
        
        if (end != str && *end == '\0') {
            // str converted to double
            *(double*)target = d;
            return;
        }
    } else {
        long i = strtol(str, &end, 10);
        
        if (end != str && *end == '\0') {
            // str converted to integer
            *(int*)target = i;
            return;
        } else {
            // fallback: maybe it's a double like "3e2"
            double d = strtod(str, &end);

            if (end != str && *end == '\0') {
                // str is a e-formated double
                *(double*)target = d;
                return;
            }
        }
    }

    // if the conversions fail, assume type is string
    char* t = (char*)target;
    t = str;
}

static void printHelp(App* app, char* name) {
    if (!app) return;
    printf("%s\n", app->description);
    printf("Usage: %s [OPTIONS]\n\n", name);
    printf("Options:\n");

    for (size_t i = 0; i < app->optCount; ++i) {
        printf("  %s,%s\t%s\n",
            app->options[i]->info.opString,
            app->options[i]->info.altString,
            app->options[i]->info.description
        );
    }

    for (size_t i = 0; i < app->flagCount; ++i) {
        printf("  %s,%s\t%s\n",
            app->flags[i]->info.opString,
            app->flags[i]->info.altString,
            app->flags[i]->info.description
        );
    }
}

static void OptionErr(Option* option) {
    if (!option) return;
    printf("Missing value for option: %s,%s\n",
        option->info.opString,
        option->info.altString
    );
};

//---------------------------------------------------------
// Public Methods

App createApp(const char* desc) {
    App app = { desc, NULL, 0, NULL, 0 };
   
    // connect methods to function ptrs 
    app.AddFlag = AddFlag;
    app.AddOption = AddOption;

    return app;
}

void parseCLI(App* app, int argc, char** argv) {
    if (argc == 1) {
        printHelp(app, argv[0]);
        return;
    }

    // set option targets
    for (int i = 0; i < argc; ++i) {
        size_t optIndex = -1;
        char* c = argv[i];

        for (size_t k = 0; k < app->optCount; ++k) {
            // check if string c is either matching an options
            // optstring or altstring
            int isOptStr = strcmp(c, app->options[k]->info.opString);
            int isAltStr = strcmp(c, app->options[k]->info.altString);
            
            if (isOptStr == 0 || isAltStr == 0) {
                optIndex = k;
                break;
            }
        }

        // argument not recognized in options list
        if (optIndex == -1) continue;
        // option found but cannot assign a value to it
        if (i >= argc-1) {
            OptionErr(app->options[optIndex]);
            exit(1);
        }

        // look ahead and set the value to the option
        char* next = argv[i+1];

        // determine the type of next (string,int,double)
        // and set the option targets value to it
        ProcessOption(app->options[optIndex]->target, next);
    }

    // enable flags
    for (size_t i = 0; i < app->flagCount; ++i) {
        *(app->flags[i]->target) = 1;
    }
}

#endif
