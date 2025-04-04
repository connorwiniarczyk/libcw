#include <cwcore.h>
#include <cwhost.h>
#include <stdlib.h>

CwCompile cwcompile_create(CwArena* a, const char* src, const char* dest) {
    CwCompile output = {0};
    output.src = src;
    output.dest = dest;
    output.target = CWTARGET_AMD64_POSIX;

    output.include_paths.ptr = cwarena_align_to(a, alignof(char**));
    output.include_paths.a = cwarena_reserve(a, sizeof(char**) * 16); 

    output.flags.ptr = cwarena_align_to(a, alignof(char**));
    output.flags.a = cwarena_reserve(a, sizeof(char**) * 16); 
    cwarray_push(&output.include_paths.a, char*, "-O2", "-Wall", "-Werror", "-Wextra", "-g");

	return output;
}


CwLink cwlink_create(CwArena* a, const char* dest) {
    CwLink output = {0};
    output.dest = dest;
    output.target = CWTARGET_AMD64_POSIX;

    output.flags.ptr = cwarena_align_to(a, alignof(char**));
    output.flags.a = cwarena_reserve(a, sizeof(char**) * 16); 

    output.lib_paths.ptr = cwarena_align_to(a, alignof(char**));
    output.lib_paths.a = cwarena_reserve(a, sizeof(char**) * 16); 

    output.libs.ptr = cwarena_align_to(a, alignof(char**));
    output.libs.a = cwarena_reserve(a, sizeof(char**) * 16); 

    output.objects.ptr = cwarena_align_to(a, alignof(char**));
    output.objects.a = cwarena_reserve(a, sizeof(char**) * 32); 

	return output;
}

CwCmd cwcompile_cmd(CwArena* a, CwCompile* params) {
    CwCmd output = {0};
    output.io.in  = 0;
    output.io.out = 1;
    output.io.err = 2;
    output.ptr = cwarena_align_to(a, alignof(char*));

    switch (params -> target) {
        case CWTARGET_AMD64_POSIX:   cwarray_push(a, char*, "gcc"); break;
        case CWTARGET_AMD64_MINGW32: cwarray_push(a, char*, "x86_64-w64-mingw32-gcc"); break;
        case CWTARGET_WASM32:        cwarray_push(a, char*, "clang", "--target=wasm32"); break;
        default: cwpanic("INVALID TARGET");
    }

    int array_size;
    array_size = (cwarena_allocated(params -> flags.a, params -> flags.ptr) / sizeof(char*));
    for (int i=0; i<array_size; i++) {
        cwarena_push(a, char*, params -> flags.ptr[i]);
    }

    array_size = (cwarena_allocated(params -> include_paths.a, params -> include_paths.ptr) / sizeof(char*));
    for (int i=0; i<array_size; i++) {
        cwarena_push(a, char*, "-I");
        cwarena_push(a, char*, params -> include_paths.ptr[i]);
    }

    cwarena_push(a, const char*, "-c");
    cwarena_push(a, const char*, params -> src);
    cwarena_push(a, const char*, "-o");
    cwarena_push(a, const char*, params -> dest);

    // cwarray_push(a, const char*, "-c", params -> dest, "-o", (char*)params -> src);

    output.size = cwarena_allocated(*a, output.ptr) / sizeof(char*);
    output.a = cwarena_reserve(a, sizeof(char*) * 1);

    return output;
}

CwCmd cwlink_cmd(CwArena* a, CwLink* params) {
    CwCmd output = {0};
    output.io.in  = 0;
    output.io.out = 1;
    output.io.err = 2;
    output.ptr = cwarena_align_to(a, alignof(char*));

    switch (params -> target) {
        case CWTARGET_AMD64_POSIX:   cwarray_push(a, char*, "gcc"); break;
        case CWTARGET_AMD64_MINGW32: cwarray_push(a, char*, "x86_64-w64-mingw32-gcc"); break;
        case CWTARGET_WASM32:        cwarray_push(a, char*, "clang", "--target=wasm32"); break;
        default: cwpanic("INVALID TARGET");
    }

    int array_size;
    array_size = (cwarena_allocated(params -> objects.a, params -> objects.ptr) / sizeof(char*));
    for (int i=0; i<array_size; i++) {
        cwarena_push(a, char*, params -> objects.ptr[i]);
    }

    array_size = (cwarena_allocated(params -> flags.a, params -> flags.ptr) / sizeof(char*));
    for (int i=0; i<array_size; i++) {
        cwarena_push(a, char*, params -> flags.ptr[i]);
    }

    array_size = (cwarena_allocated(params -> lib_paths.a, params -> lib_paths.ptr) / sizeof(char*));
    for (int i=0; i<array_size; i++) {
        cwarena_push(a, char*, params -> lib_paths.ptr[i]);
    }

    array_size = (cwarena_allocated(params -> libs.a, params -> libs.ptr) / sizeof(char*));
    for (int i=0; i<array_size; i++) {
        cwarena_push(a, char*, params -> libs.ptr[i]);
    }


    cwarena_push(a, const char*, "-o");
    cwarena_push(a, const char*, params -> dest);

    output.size = cwarena_allocated(*a, output.ptr) / sizeof(char*);
    output.a = cwarena_reserve(a, sizeof(char*) * 1);

    return output;
}

// char*  cwbuild_compiler;
// char*  cwbuild_prefix;
// char*  cwbuild_dest;
// char** cwbuild_flags;
// char** cwbuild_include_dirs;
// char** cwbuild_lib_dirs;
// char** cwbuild_libs;
// bool cwbuild_debug_symbols = true;
// bool cwbuild_pedantic = true;
// bool cwbuild_strict = true;
// static char* default_flags[] = { NULL };
// static char* default_libs[]  = { "-lcw", "-lc", "-lm", NULL };
// static void cwbuild_push_flags(CwCmd* cmd) {
//     if (cwbuild_debug_symbols) cwcmd_push_args(cmd, "-g");
//     if (cwbuild_pedantic) cwcmd_push_args(cmd, "-Wextra", "-Wall");
//     if (cwbuild_strict) cwcmd_push_args(cmd, "-Werror");
// }

// void cwbuild_init(CwArena* a, const char* prefix) {
//     cwbuild_compiler = "gcc";
//     cwbuild_prefix = (char*)prefix;

//     cwbuild_flags = default_flags;
//     cwbuild_libs  = default_libs;

// 	char* local_headers = cwfmt_cstr(a, "%s/include", prefix);
// 	char* local_libs = cwfmt_cstr(a, "-L%s/lib", prefix);

// 	cwbuild_include_dirs = cwarena_align_to(a, alignof(char**));
// 	cwarena_push_ptrs(a, "-I", "src", "-I", "build", "-I", "include");
// 	cwarena_push_ptrs(a, "-I", local_headers);
// 	cwarena_push_ptr(a, NULL);

// 	cwbuild_lib_dirs = cwarena_align_to(a, alignof(char**));
// 	cwarena_push_ptr(a, local_libs);
// 	cwarena_push_ptr(a, NULL);
// }

// void cwbuild_init_mingw(CwArena* a, const char* prefix) {
//     cwbuild_compiler = "x86_64-w64-mingw32-gcc";
//     cwbuild_prefix = (char*)prefix;

//     // static char* flags[] = { "-g", "-Wall", "-Wextra", "-Werror", "-mwindows", "-static-libgcc", NULL };
//     static char* flags[] = { "-mwindows", "-static-libgcc", NULL };
//     cwbuild_flags = flags;

//     static char* libs[] = { "-lcw", "-lmingw32", "-lwinmm", "-lgdi32", NULL };
//     cwbuild_libs = libs;
//     cwbuild_dest = "build.mingw";

// 	char* local_headers = cwfmt_cstr(a, "%s/include", prefix);
// 	char* local_libs = cwfmt_cstr(a, "-L%s/lib.mingw", prefix);

// 	cwbuild_include_dirs = cwarena_align_to(a, alignof(char**));
// 	cwarena_push_ptrs(a, "-I", "src", "-I", "build.mingw", "-I", "include");
// 	cwarena_push_ptrs(a, "-I", local_headers);
// 	cwarena_push_ptr(a, NULL);

// 	cwbuild_lib_dirs = cwarena_align_to(a, alignof(char**));
// 	cwarena_push_ptr(a, local_libs);
// 	cwarena_push_ptr(a, NULL);
// }

// CwCmd cwbuild_compile_cmd(CwArena* a, const char* src, const char* dest) {
//     CwCmd output = cwcmd_create(cwbuild_compiler, a, 0);
//     cwbuild_push_flags(&output);

//     cwcmd_push_arglist(&output, (const char**)cwbuild_flags);
//     cwcmd_push_arglist(&output, (const char**)cwbuild_include_dirs);
//     cwcmd_push_args(&output, "-o", dest);
//     cwcmd_push_args(&output, "-c", src);

//     *a = output.mem;
//     output.mem = cwarena_reserve(a, sizeof(void*));

//     return output;
// }

// CwCmd cwbuild_link_cmd(CwArena* a, const char* dest, const char** objects) {
//     CwCmd output = cwcmd_create(cwbuild_compiler, a, 0);

// 	cwcmd_push_arglist(&output, objects);
//     cwcmd_push_arglist(&output, (const char**)cwbuild_flags);
//     cwcmd_push_arglist(&output, (const char**)cwbuild_lib_dirs);
//     cwcmd_push_arglist(&output, (const char**)cwbuild_libs);
//     cwcmd_push_args(&output, "-o", dest);

//     *a = output.mem;
//     output.mem = cwarena_reserve(a, sizeof(void*));

//     return output;
// }

// CwCmd cwbuild_archive_cmd(CwArena* a, const char* dest, const char** objects) {
//     // char* dest_path = cwfmt_cstr(a, "%s/%s", cwbuild_dest, dest);

//     CwCmd output = cwcmd_create("ar", a, 0);
//     cwcmd_push_arg(&output, "-rc");
//     cwcmd_push_arg(&output, dest);

// 	cwcmd_push_arglist(&output, objects);

//     *a = output.mem;
//     output.mem = cwarena_reserve(a, sizeof(void*));

//     return output;
// }
