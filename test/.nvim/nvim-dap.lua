local dap = require("dap")

dap.adapters.lldb = {
  type = "executable",
  command = "/usr/bin/lldb-dap",
  name = "lldb",
}

dap.configurations.cpp = {
  {
    name = "Run",
    type = "lldb",
    request = "launch",
    program = "${workspaceFolder}/build/foo",
    cwd = "${workspaceFolder}/build",
    stopOnEntry = false,
  },
}

dap.configurations.c = dap.configurations.cpp
