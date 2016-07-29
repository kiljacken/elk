// elk_args.h - v0.1 - a dead simple parser in plain c.
//
// Copyright (c) 2016 Emil Andersen Lauridsen
// Distributed under the MIT License, see LICENSE
//
// Currently only support parsing plain flags (boolean style) and flags with
// arguments (currently only stored as a string).
//
// TODO:
// - Handle non flag arguments
//
// Example usage:
//   #include <stdio.h>
//   #define ELKA_IMPLEMENTATION
//   #include "elk_args.h"
//   int
//   main(int argc, char** argv)
//   {
//       elka_state args = {0};
//
//       elka_flag verbose = {0};
//       elka_add_flag(&args, &verbose, "verbose", 'v', elka_bool);
//
//       elka_flag message = {0};
//       elka_add_flag(&args, &message, 0, 'm', elka_string);
//
//       elka_parse(&args, argc, argv);
//
//       printf("Verbose: %d\n", verbose.Value.AsBool);
//       printf("Message: %s\n", message.Value.AsString);
//   }
//
// Example output:
//   $ elka_test --verbose -m "Hello, World!"
//   Verbose: 1
//   Message: Hello, World!
//
// Changelog:
// v0.1:
// - Released to public under MIT license

#ifndef ELK_ARGS_H
#define ELK_ARGS_H
typedef enum elka_type
{
    elka_bool,
    elka_string,
} elka_type;

typedef struct elka_flag
{
    struct elka_flag* Next;
    
    char* LongName;
    char ShortName;
    
    elka_type Type;
    union {
        int AsBool;
        char* AsString;
    } Value;
} elka_flag;

typedef struct elka_state
{
    elka_flag* First;
} elka_state;

extern void elka_add_flag(elka_state* State, elka_flag* Flag, char* LongName,
                          char ShortName, elka_type Type);
// Add's a flag to be parsed.

extern int elka_parse(elka_state* State, int Count, char** Data);
#endif // ELKA_ARGS_H

#ifdef ELKA_IMPLEMENTATION

// Define ELKA_STRCMP if you already have your own strcmp.
// As long as equality returns 0 we'll be happy.
#ifndef ELKA_STRCMP
  #include <string.h>
  #define ELKA_STRCMP strcmp
#endif

void elka_add_flag(elka_state* State, elka_flag* Flag,
              char* LongName, char ShortName, elka_type Type)
{
    Flag->LongName = LongName;
    Flag->ShortName = ShortName;
    Flag->Type = Type;
    
    Flag->Next = State->First;
    State->First = Flag;        
}

int elka_parse(elka_state* State, int Count, char** Data)
{
    for (int Index = 1; Index < Count; Index++)
    {
        char* Arg = Data[Index];
        if (Arg[0] == '-')
        {
            int IsLong = (Arg[1] == '-');
            for (elka_flag* Flag = State->First; Flag; Flag = Flag->Next)
            {
                int Match = 0;
                if (IsLong)
                {
                    Match = Flag->LongName && ELKA_STRCMP(Arg + 2, Flag->LongName) == 0;
                }
                else
                {
                    Match = Flag->ShortName && Flag->ShortName == Arg[1];
                }
                
                if (Match)
                {
                    switch (Flag->Type)
                    {
                        case elka_bool: {
                            Flag->Value.AsBool = 1;
                        } break;
                        
                        case elka_string: {
                            Flag->Value.AsString = Data[++Index];
                        } break;                       
                    }
                    break;
                }
            }
        }
        else
        {
            // TODO(kiljacken): Handle plain arguments
        }
    }
    return(0);
}

#endif // ELKA_IMPLEMENTATION
