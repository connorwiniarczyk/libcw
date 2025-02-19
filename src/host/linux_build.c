#include <cwcore.h>
#include <cwhost.h>
#include <stdlib.h>

char*  cwbuild_prefix;
char*  cwbuild_dest;
char** cwbuild_flags;
char** cwbuild_include_dirs;
char** cwbuild_lib_dirs;
char** cwbuild_libs;

static char* default_flags[] = { "-g", "-Wall", "-Wextra", "-Werror", NULL };
static char* default_libs[]  = { "-lcw", "-lc", "-lm", NULL };

void cwbuild_init(CwArena* a, const char* prefix) {
    cwbuild_prefix = (char*)prefix;

    cwbuild_flags = default_flags;
    cwbuild_libs  = default_libs;
    cwbuild_dest  = "build";

	char* local_headers = cwfmt_cstr(a, "%s/include", prefix);
	char* local_libs = cwfmt_cstr(a, "-L%s/lib", prefix);

	cwbuild_include_dirs = cwarena_align_to(a, alignof(char**));
	cwarena_push_ptr(a, "-I");
	cwarena_push_ptr(a, "src");
	cwarena_push_ptr(a, "-I");
	cwarena_push_ptr(a, "include");
	cwarena_push_ptr(a, "-I");
	cwarena_push_ptr(a, local_headers);
	cwarena_push_ptr(a, NULL);

	cwbuild_lib_dirs = cwarena_align_to(a, alignof(char**));
	cwarena_push_ptr(a, local_libs);
	cwarena_push_ptr(a, NULL);

	cwbuild_lib_dirs = cwarena_align_to(a, alignof(char**));
	cwarena_push_ptr(a, local_libs);
	cwarena_push_ptr(a, NULL);
}

CwCmd cwbuild_compile_cmd(CwArena* a, const char* src) {
    CwStr base_name = cwpath_get_base(cwpath_get_file(cwstr(src)));
    char* dest = cwfmt_cstr(a, "%s/%w.o", cwbuild_dest, base_name);

    CwCmd output = cwcmd_create("gcc", a, 0);
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

    CwCmd output = cwcmd_create("gcc", a, 0);

	cwcmd_push_arglist(&output, objects);
    cwcmd_push_arglist(&output, (const char**)cwbuild_flags);
    cwcmd_push_arglist(&output, (const char**)cwbuild_lib_dirs);
    cwcmd_push_arglist(&output, (const char**)cwbuild_libs);
    cwcmd_push_args(&output, "-o", dest_path);

    *a = output.mem;
    output.mem = cwarena_reserve(a, sizeof(void*));

    return output;
}
