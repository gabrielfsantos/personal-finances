# "Define linter aspects"

load("@aspect_rules_lint//lint:clang_tidy.bzl", "lint_clang_tidy_aspect")
load("@aspect_rules_lint//lint:lint_test.bzl", "lint_test")
# load("@aspect_rules_lint//lint:yamllint.bzl", "lint_yamllint_aspect")

# yamllint = lint_yamllint_aspect(
#     binary = Label("//tools/lint:yamllint"),
#     config = Label("//:.yamllint"),
# )

# yamllint_test = lint_test(aspect = yamllint)

# clang_tidy = lint_clang_tidy_aspect(
#     binary = Label("//tools/lint:clang_tidy"),
#     configs = [
#         Label("//:.clang-tidy"),
#         Label("//src/cpp/lib:get/.clang-tidy"),
#     ],
#     lint_target_headers = True,
#     angle_includes_are_system = False,
#     verbose = False,
# )

# clang_tidy_test = lint_test(aspect = clang_tidy)

# an example of setting up a different clang-tidy aspect with different
# options. This one uses a single global clang-tidy file
clang_tidy = lint_clang_tidy_aspect(
    binary = "@@//tools/lint:clang_tidy",
    global_config = "@@//:.clang-tidy",
    lint_target_headers = True,
    angle_includes_are_system = False,
    # verbose = False,
    verbose = True,
)

clang_tidy_test = lint_test(aspect = clang_tidy)
