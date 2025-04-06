#include <cwcore.h>
#include <cwhost.h>
#include <stdlib.h>

CwCompileConfig cwcompile_config(CwArena* a, const char* src, const char* dest) {
    CwCompileConfig output = {0};
    output.src = src;
    output.dest = dest;
    output.target = CWTARGET_AMD64_POSIX;

    output.include_paths.ptr = cwarena_align_to(a, alignof(char**));
    output.include_paths.a = cwarena_reserve(a, sizeof(char**) * 16); 

    output.flags.ptr = cwarena_align_to(a, alignof(char**));
    output.flags.a = cwarena_reserve(a, sizeof(char**) * 16); 
    // cwarray_push(&output.include_paths.a, char*, "-O2", "-Wall", "-Werror", "-Wextra", "-g");

	return output;
}


CwLinkConfig cwlink_config(CwArena* a, const char* dest) {
    CwLinkConfig output = {0};
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

CwCmd cwcompile_cmd(CwArena* a, CwCompileConfig params) {
    CwCmd output = {0};
    output.io.in  = 0;
    output.io.out = 1;
    output.io.err = 2;
    output.ptr = cwarena_align_to(a, alignof(char*));

    switch (params.target) {
        case CWTARGET_AMD64_POSIX:   cwarray_push(a, char*, "gcc"); break;
        case CWTARGET_AMD64_MINGW32: cwarray_push(a, char*, "x86_64-w64-mingw32-gcc"); break;
        case CWTARGET_WASM32:        cwarray_push(a, char*, "clang", "--target=wasm32"); break;
        default: cwpanic("INVALID TARGET");
    }

    cwarray_clone(a, cwarray_cast(params.flags));
    // cwarray_clone(a, cwarray_cast(params.include_paths));

    int len = cwarray_len(cwarray_cast(params.include_paths));
    for (int i=0; i<len; i++) {
        cwarena_push_ptr(a, "-I");
        cwarena_push_ptr(a, params.include_paths.ptr[i]);
    }

    cwarena_push(a, const char*, "-c");
    cwarena_push(a, const char*, params.src);
    cwarena_push(a, const char*, "-o");
    cwarena_push(a, const char*, params.dest);

    output.size = cwarena_allocated(*a, output.ptr) / sizeof(char*);
    output.a = cwarena_reserve(a, sizeof(char*) * 1);

    return output;
}


CwCmd cwlink_cmd(CwArena* a, CwLinkConfig params) {
    CwCmd output = {0};
    output.io.in  = 0;
    output.io.out = 1;
    output.io.err = 2;
    output.ptr = cwarena_align_to(a, alignof(char*));

    switch (params.target) {
        case CWTARGET_AMD64_POSIX:   cwarray_push(a, char*, "gcc"); break;
        case CWTARGET_AMD64_MINGW32: cwarray_push(a, char*, "x86_64-w64-mingw32-gcc"); break;
        case CWTARGET_WASM32:        cwarray_push(a, char*, "clang", "--target=wasm32"); break;
        default: cwpanic("INVALID TARGET");
    }


    cwarray_clone(a, cwarray_cast(params.objects));

    // if (params.target == CWTARGET_WASM32) {
    // }

    cwarray_clone(a, cwarray_cast(params.flags));
    cwarray_clone(a, cwarray_cast(params.lib_paths));
    cwarray_clone(a, cwarray_cast(params.libs));

    // string_array_clone((StringArray*)(&params.objects), a);
    // string_array_clone((StringArray*)(&params.flags), a);
    // string_array_clone((StringArray*)(&params.lib_paths), a);
    // string_array_clone((StringArray*)(&params.libs), a);

    cwarena_push(a, const char*, "-o");
    cwarena_push(a, const char*, params.dest);

    output.size = cwarena_allocated(*a, output.ptr) / sizeof(char*);
    output.a = cwarena_reserve(a, sizeof(char*) * 1);

    return output;
}
