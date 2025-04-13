#include "App/Init.h"
#include "FreeRTOS.h"
#include "Simulation/InstructionCallback.h"
#include "queue.h"
#include "task.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>

static void ParseArguments(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--realtime") == 0) {
      InstructionCallback_EnableRealTimeMode();
    }
  }
}

static void handle_sigint(int sig) {
  EndSimulation();
}

int main(int argc, char *argv[]) {
  struct sigaction sa = {.sa_handler = handle_sigint};
  sigaction(SIGINT, &sa, NULL);

  ParseArguments(argc, argv);

  fprintf(stderr, "Starting simulation...\n");
  App_Init();
  vTaskStartScheduler();
  fprintf(stderr, "Simulation finished.\n");

  fprintf(stderr, "Starting simulation...\n");
  App_Init();
  vTaskStartScheduler();
  fprintf(stderr, "Simulation finished.\n");

  return 0;
}
