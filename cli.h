#ifndef CLI
#define CLI

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    const char* description;
    const char* opString;
    const char* altString;
} Info;

typedef enum {
    tOption,
    tFlag
} dType;

typedef enum {
    F_BOOLEAN,
    F_INTEGER
} flag_t;
typedef enum {
    O_STRING,
    O_INTEGER,
    O_DOUBLE,
    O_FLOAT,
    O_SIZE_T
} opt_t;

typedef struct {
    Info info;
    opt_t option_type;
    void* target; // can be anything
} Option;

typedef struct {
    Info info;
    flag_t flag_type;
    void* target; // when dereferenced can only be a bool or int
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
            opt_t, void*, App*);
    void (*AddFlag)(
            const char*, const char*, const char*,
            flag_t, void*, App*);
};

//-------------------------------------------
//  Private Methods
//  |___ static keyword for methods means it
//       cannot be referenced outside this file

// lacking but multiple options attached to the same target seems
// unneccessary
static int matchingOptions(Option* a, Option* b) {
    if (!a || !b) return 0;
    if (!a->target || !b->target) return 0;
    return a->target == b->target;
}

static int matchingFlags(Flag* a, Flag* b) {
    if (!a || !b) return 0;
    if (!a->target || !b->target) return 0;
    return a->target == b->target;
}

static int Contains(void** collection, size_t count, dType t, void* elem) {
    if (!collection) return 1;
    if (!elem) return 1;

    for (size_t i = 0; i < count; ++i) {
        if (t == tOption) {
            // implicit casting performed
            Option* cOpt = (Option*)collection[i];
            Option* nOpt = (Option*)elem;

            if (matchingOptions(cOpt,nOpt)) return 1;
        } else if (t == tFlag) {
            Flag* cFlag = (Flag*)collection[i];
            Flag* nFlag = (Flag*)elem;
            
            if (matchingFlags(cFlag,nFlag)) return 1; // is contained
        }
    }
    return 0; // not contained
}

static void AddFlag(
        const char* optStr,
        const char* altStr,
        const char* desc,
        flag_t flag_type,
        void* tar,
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
    nFlag->flag_type = flag_type;
    nFlag->target = tar;

    // do not insert duplicate options
    if (
        app->optCount > 0 &&
        Contains(
            (void **)app->flags,   // collection to iterate
            app->flagCount,         // collection size
            tFlag,                  // collection element type
            nFlag                   // new flag we want to add
        ) == 1
    ) { return; }

    // perform dynamic memory actions
    Flag** tmp = app->flags;
    app->flags = malloc( (app->flagCount + 1) * sizeof(Flag*));
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

static void EnableFlag(Flag* flag) {
    if (!flag) return;

    if (flag->flag_type == F_INTEGER) {
        * (int*) (flag->target) = 1;
    } else if (flag->flag_type == F_BOOLEAN) {
        * (bool*) (flag->target) = true;
    }
}

static void AddOption(
        const char* optStr,
        const char* altStr,
        const char* desc,
        opt_t option_type,
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
    nOpt->option_type = option_type;
    nOpt->target = tar;

    // do not insert duplicate options
    if (
        app->optCount > 0 &&
        Contains(
            (void **)app->options, // collection to iterate
            app->optCount,          // collection size
            tOption,                // collection element type
            nOpt                    // new option we want to add
        ) == 1
    ) { return; }

    // perform dynamic memory actions
    Option** tmp = app->options;
    app->options = malloc( (app->optCount + 1) * sizeof(Option*));
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

static void ProcessOption(void* dest, opt_t option_type, char* target_value) {
    char* end;
    if (option_type == O_STRING) {
        *(char**)dest = target_value;
    } else if (option_type == O_INTEGER) {
        *(int*)dest = strtol(target_value, &end, 10);
    } else if (option_type == O_DOUBLE) {
        *(double*)dest = strtod(target_value, &end);
    } else if (option_type == O_FLOAT) {
        *(float*)dest = strtof(target_value, &end);
    } else if (option_type == O_SIZE_T) {
        *(size_t*)dest = (size_t)strtoull(target_value, &end, 10);
    }
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
    // check if help flag was past
    for (int i = 1; i < argc; ++i) {
        int shortHelp = strcmp(argv[i],"-h");
        int longHelp = strcmp(argv[i],"--help");
        
        if (shortHelp == 0 || longHelp == 0) {
            printHelp(app, argv[0]);
            exit(0);
        }
    }

    // set option targets
    for (int i = 1; i < argc; ++i) {
        char* c = argv[i];

        for (size_t k = 0; k < app->optCount; ++k) {
            // check if string c is either matching an options
            // optstring or altstring
            int isOptStr = strcmp(c, app->options[k]->info.opString);
            int isAltStr = strcmp(c, app->options[k]->info.altString);
            
            if (isOptStr == 0 || isAltStr == 0) {
                // option found but cannot assign a value to it
                // because the element is out of bounds
                if (i+1 >= argc) {
                    OptionErr(app->options[k]);
                    exit(1);
                }

                // look ahead and set the value to the option
                char* optionVal = argv[i+1];
                
                // if the option value is in bounds make sure it isnt a flag
                if (optionVal[0] == '-') {
                    OptionErr(app->options[k]);
                    exit(1);
                }

                // determine the type of next (string,int,double)
                // and set the option targets value to it
                ProcessOption(app->options[k]->target, app->options[k]->option_type, optionVal);

                break;
            }
        }
    }

    // enable flags
    for (int i = 1; i < argc; ++i) {
        char* c = argv[i];

        for (size_t k = 0; k < app->flagCount; ++k) {
            int isOptStr = strcmp(c, app->flags[k]->info.opString);
            int isAltStr = strcmp(c, app->flags[k]->info.altString);
            
            // flag is enabled
            if (isOptStr == 0 || isAltStr == 0) {
                EnableFlag(app->flags[k]);
                break;
            }
        }
    }
}

#endif