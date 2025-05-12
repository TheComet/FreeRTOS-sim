/* clang-format off */
#include "gcc-plugin.h"
#include "plugin-version.h"
#include "tree.h"
#include "tree-pass.h"
#include "context.h"
#include "function.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "basic-block.h"
/* clang-format on */

int plugin_is_GPL_compatible;

namespace {

const pass_data tick_pass_data = {
    GIMPLE_PASS,               // Type
    "instructioncallbackpass", // Name
    OPTGROUP_NONE,             // Optinfo
    TV_NONE,                   // TV
    PROP_gimple_any,           // Properties required
    0,                         // Properties provided
    0,                         // Properties destroyed
    0,                         // Flags
};

struct tick_pass : gimple_opt_pass {
  tick_pass(gcc::context *ctx, const char *callback_name,
            int callback_frequency)
      : gimple_opt_pass(tick_pass_data, ctx), callback_name(callback_name),
        callback_frequency(callback_frequency), tick_callback_decl(nullptr) {}

  bool gate(function *) override {
    return true; // run on all functions
  }

  unsigned int execute(function *fun) override {
    // Can't create the function signature in the constructor (probably because
    // of global state?) so have to do it here.
    if (!tick_callback_decl) {
      tree fn_type = build_function_type_list(void_type_node, NULL_TREE);
      tick_callback_decl = build_fn_decl(callback_name, fn_type);
    }

    // Go through each basic block and insert callbacks at regular intervals.
    // Important: Every block must have at minimum one callback. We don't want
    // e.g. a small loop to exist that could spin forever. Basic blocks have
    // one entry and one exit, by definition.
    basic_block bb;
    int freq_counter = 0;
    FOR_EACH_BB_FN(bb, fun) {
      bool call_was_inserted = false;
      for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi);
           gsi_next(&gsi)) {

        if (++freq_counter < callback_frequency)
          continue;

        gcall *call = gimple_build_call(tick_callback_decl, 0);
        gsi_insert_before(&gsi, call, GSI_SAME_STMT);
        freq_counter = 0;
      }

      if (!call_was_inserted) {
        gimple_stmt_iterator gsi = gsi_start_bb(bb);
        gcall *call = gimple_build_call(tick_callback_decl, 0);
        gsi_insert_before(&gsi, call, GSI_SAME_STMT);
      }
    }

    return 0;
  }

  tree tick_callback_decl;
  const char *callback_name;
  const int callback_frequency;
};

} // namespace

int plugin_init(struct plugin_name_args *plugin_info,
                struct plugin_gcc_version *version) {
  if (!plugin_default_version_check(version, &gcc_version)) {
    fprintf(stderr, "This GCC plugin is incompatible with your GCC version\n");
    return 1;
  }

  const struct plugin_argument *args = plugin_info->argv;
  int argc = plugin_info->argc;

  const char *callback_name = nullptr;
  int callback_frequency = 5;
  for (int i = 0; i < argc; ++i) {
    if (strcmp(args[i].key, "callback") == 0) {
      callback_name = args[i].value;
    } else if (strcmp(args[i].key, "frequency") == 0) {
      callback_frequency = std::atoi(args[i].value);
    }
  }

  if (!callback_name) {
    fprintf(stderr, "No callback name provided. Use "
                    "-fplugin-arg-instructioncallbackpass-callback=<name>\n");
    return 1;
  }

  if (callback_frequency < 1) {
    fprintf(stderr,
            "Frequency must be greater than 0. Use "
            "-fplugin-arg-instructioncallbackpass-frequency=<number>\n");
  }

  struct register_pass_info pass_info;
  pass_info.pass = new tick_pass(g, callback_name, callback_frequency);
  pass_info.reference_pass_name =
      "cfg"; // Run after Control Flow Graph creation
  pass_info.ref_pass_instance_number = 1;
  pass_info.pos_op = PASS_POS_INSERT_AFTER;

  register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, nullptr,
                    &pass_info);

  return 0;
}
