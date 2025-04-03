#include <errno.h>
#include <stdio.h>
#include <string.h>

struct Args {
  const char *inputFileName;
  const char *outputFileName;
};

static void PrintHelp(const char *ProgramName) {
  fprintf(stderr, "Usage: %s [-i <input>] [-o <output>]\n", ProgramName);
  fprintf(stderr, "  -i <input>   Specify an input file to read from\n");
  fprintf(stderr, "  -o <output>  Specify an output file to write the "
                  "patched ASM to\n");
  fprintf(stderr, "\nThis program also supports reading from stdin and "
                  "writing to stdout.\nFor example:\n");
  fprintf(stderr, "  gcc -S -o- in.c | %s | gcc -o out.o -xc -\n", ProgramName);
}

static int ParseArgs(int argc, char *argv[], struct Args *args) {
  const char *ProgramName = argv[0];

  while (++argv, --argc) {
    if (strcmp(*argv, "-h") == 0 || strcmp(*argv, "--help") == 0) {
      PrintHelp(ProgramName);
      return -1;
    }

    if (strcmp(*argv, "-i") == 0) {
      if ((++argv, --argc) == 0) {
        fprintf(stderr, "Missing <input> filename to argument \"-i\"\n");
        return -1;
      }
      args->inputFileName = *argv;
    }

    if (strcmp(*argv, "-o") == 0) {
      if ((++argv, --argc) == 0) {
        fprintf(stderr, "Missing <output> filename to argument \"-o\"\n");
        return -1;
      }
      args->outputFileName = *argv;
    }
  }

  return 0;
}

static int FindMnemonic(const char *line, int len) {
  static const char *mnemonics[] = {"movl", "movq", "jmp", NULL};
  for (const char **mnemonic = mnemonics; *mnemonic; ++mnemonic) {
    const char *needle = *mnemonic;
    const char *haystack = line;
    int needle_len = strlen(needle);
    int haystack_len = len;
    for (int i = 0; i < haystack_len - needle_len; ++i) {
      if (memcmp(&haystack[i], needle, needle_len) == 0) {
        return 1;
      }
    }
  }

  return 0;
}

static int ProcessFile(FILE *in, FILE *out) {
  static char line[4096];
  char c, *p = line;
  while ((c = getc(in)) != EOF) {
    if (p - line + 1 == sizeof(line)) {
      if (fwrite(line, p - line, 1, out) != 1) {
        return -1;
      }
      fprintf(stderr, "Warning: Line was too long for line buffer\n");
      p = line;
      continue;
    }

    *p++ = c;

    if (c == '\n') {
      if (FindMnemonic(line, p - line)) {
        static const char *patch =
            "        call instruction_callback_invisible\n";
        if (fwrite(patch, strlen(patch), 1, out) != 1) {
          return -1;
        }
      }
      if (fwrite(line, p - line, 1, out) != 1) {
        return -1;
      }
      p = line;
    }
  }

  if (fwrite(line, p - line, 1, out) != 1) {
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  int retval = 1;
  struct Args args = {};
  if (ParseArgs(argc, argv, &args) != 0)
    goto parse_args_failed;

  FILE *in = args.inputFileName ? fopen(args.inputFileName, "rb") : stdin;
  if (in == NULL) {
    fprintf(stderr, "Failed to open input file \"%s\" for reading: %s\n",
            args.inputFileName, strerror(errno));
    goto open_in_failed;
  }

  FILE *out = args.outputFileName ? fopen(args.outputFileName, "wb") : stdout;
  if (out == NULL) {
    fprintf(stderr, "Failed to open output file \"%s\" for writing: %s\n",
            args.inputFileName, strerror(errno));
    goto open_out_failed;
  }

  if (ProcessFile(in, out) == 0) {
    retval = 0;
  }

  if (args.outputFileName) {
    fclose(out);
  }
open_out_failed:
  if (args.inputFileName) {
    fclose(in);
  }
open_in_failed:
parse_args_failed:
  return retval;
}
