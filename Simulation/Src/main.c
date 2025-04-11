#include "App/Init.h"
#include "Simulation/InstructionCallback.h"
#include <string.h>

static void ParseArguments(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--realtime") == 0) {
      InstructionCallback_EnableRealTimeMode();
    }
  }
}

int main(int argc, char *argv[]) {
  ParseArguments(argc, argv);

  App_Init();
  App_Run();

  return 0;
}
