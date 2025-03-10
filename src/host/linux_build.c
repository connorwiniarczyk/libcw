#include <cwcore.h>
#include <cwhost.h>
#include <stdlib.h>

char*  cwbuild_compiler;
char*  cwbuild_prefix;
char*  cwbuild_dest;
char** cwbuild_flags;
char** cwbuild_include_dirs;
char** cwbuild_lib_dirs;
char** cwbuild_libs;

bool cwbuild_debug_symbols = true;
bool cwbuild_pedantic = true;
bool cwbuild_strict = true;

static char* default_flags[] = { NULL };
static char* default_libs[]  = { "-lcw", "-lc", "-lm", NULL };

static void cwbuild_push_flags(CwCmd* cmd) {
    if (cwbuild_debug_symbols) cwcmd_push_args(cmd, "-g");
    if (cwbuild_pedantic) cwcmd_push_args(cmd, "-Wextra", "-Wall");
    if (cwbuild_strict) cwcmd_push_args(cmd, "-Werror");
}

void cwbuild_init(CwArena* a, const char* prefix) {
    cwbuild_compiler = "gcc";
    cwbuild_prefix = (char*)prefix;

    cwbuild_flags = default_flags;
    cwbuild_libs  = default_libs;
    cwbuild_dest  = "build";

	char* local_headers = cwfmt_cstr(a, "%s/include", prefix);
	char* local_libs = cwfmt_cstr(a, "-L%s/lib", prefix);

	cwbuild_include_dirs = cwarena_align_to(a, alignof(char**));
	cwarena_push_ptrs(a, "-I", "src", "-I", "build", "-I", "include");
	cwarena_push_ptrs(a, "-I", local_headers);
	cwarena_push_ptr(a, NULL);

	cwbuild_lib_dirs = cwarena_align_to(a, alignof(char**));
	cwarena_push_ptr(a, local_libs);
	cwarena_push_ptr(a, NULL);
}

void cwbuild_init_mingw(CwArena* a, const char* prefix) {
    cwbuild_compiler = "x86_64-w64-mingw32-gcc";
    cwbuild_prefix = (char*)prefix;

    // static char* flags[] = { "-g", "-Wall", "-Wextra", "-Werror", "-mwindows", "-static-libgcc", NULL };
    static char* flags[] = { "-static-libgcc", NULL };
    cwbuild_flags = flags;

    static char* libs[] = { "-lcw", "-lmingw32", "-lwinmm", "-lgdi32", NULL };
    cwbuild_libs = libs;
    cwbuild_dest = "build.mingw";

	char* local_headers = cwfmt_cstr(a, "%s/include", prefix);
	char* local_libs = cwfmt_cstr(a, "-L%s/lib.mingw", prefix);

	cwbuild_include_dirs = cwarena_align_to(a, alignof(char**));
	cwarena_push_ptrs(a, "-I", "src", "-I", "build.mingw", "-I", "include");
	cwarena_push_ptrs(a, "-I", local_headers);
	cwarena_push_ptr(a, NULL);

	cwbuild_lib_dirs = cwarena_align_to(a, alignof(char**));
	cwarena_push_ptr(a, local_libs);
	cwarena_push_ptr(a, NULL);
}

CwCmd cwbuild_compile_cmd(CwArena* a, const char* src) {
    CwStr base_name = cwpath_get_base(cwpath_get_file(cwstr(src)));
    char* dest = cwfmt_cstr(a, "%s/%w.o", cwbuild_dest, base_name);

    CwCmd output = cwcmd_create(cwbuild_compiler, a, 0);
    cwbuild_push_flags(&output);

    cwcmd_push_arglist(&output, (const char**)cwbuild_flags);
    cwcmd_push_arglist(&output, (const char**)cwbuild_include_dirs);
    cwcmd_push_args(&output, "-o", dest);
    cwcmd_push_args(&output, "-c", src);

    *a = output.mem;
    output.mem = cwarena_reserve(a, sizeof(void*));

    return output;
}

CwCmd cwbuild_link_cmd(CwArena* a, const char* dest, const char** objects) {
    char* dest_path = cwfmt_cstr(a, "%s/%s", cwbuild_dest, dest);

    CwCmd output = cwcmd_create(cwbuild_compiler, a, 0);

	cwcmd_push_arglist(&output, objects);
    cwcmd_push_arglist(&output, (const char**)cwbuild_flags);
    cwcmd_push_arglist(&output, (const char**)cwbuild_lib_dirs);
    cwcmd_push_arglist(&output, (const char**)cwbuild_libs);
    cwcmd_push_args(&output, "-o", dest_path);

    *a = output.mem;
    output.mem = cwarena_reserve(a, sizeof(void*));

    return output;
}

CwCmd cwbuild_archive_cmd(CwArena* a, const char* dest, const char** objects) {
    char* dest_path = cwfmt_cstr(a, "%s/%s", cwbuild_dest, dest);

    CwCmd output = cwcmd_create("ar", a, 0);
    cwcmd_push_arg(&output, "-rc");
    cwcmd_push_arg(&output, dest_path);

	cwcmd_push_arglist(&output, objects);

    *a = output.mem;
    output.mem = cwarena_reserve(a, sizeof(void*));

    return output;
}
