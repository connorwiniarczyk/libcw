#include <cwcore.h>
#include <cwhost.h>

CwStr cwstr(const char* cstr) {
    const char* ptr;
    for (ptr = cstr; *ptr != '\0'; ptr += 1);

    int size = ptr - cstr;

    return (CwStr){ .ptr = (char*)(cstr), .size = size };
}

CwStr cwstr_empty() {
    return (CwStr){ .ptr = NULL, .size = 0 };
}

CwStr cwstr_builder(CwArena* a) {
    CwStr output;
    output.ptr = (char*)(a -> start);
    output.size = 0;

    return output;
}

bool cwstr_equals(CwStr a, CwStr b) {
    if (a.size != b.size) return false;

    for (int i=0; i<a.size; i++) {
		if (a.ptr[i] != b.ptr[i]) return false;
    }

    return true;
}

bool cwstr_contains(CwStr self, char c) {
    for (int i=0; i<self.size; i++) {
		if (self.ptr[i] == c) return true;
    }

    return false;
}

CwStr cwstr_substr(CwStr self, int start, int end) {
    if (start >= 0) {} else {cwpanic("failed assertion");};
    if (end >= start) {} else {cwpanic("failed assertion");};
    if (end <= self.size) {} else {cwpanic("failed assertion");};

    return (CwStr) { self.ptr + start, end - start };

}


int cwstr_find(CwStr input, char c) {
	for (int i = 0; i<input.size; i++) {
    	if (input.ptr[i] == c) return i;
	}

	return -1;
}

int cwstr_find_last(CwStr input, char c) {
    int output = -1;
	for (int i = 0; i<input.size; i++) {
    	if (input.ptr[i] == c) output = i;
	}

	return output;
}

int cwstr_parse_int(CwStr input) {
    int output = 0;

    bool negative = false;
    if (input.size > 0 && input.ptr[0] == '-') {
        negative = true;
        input = cwstr_substr(input, 1, input.size);
    }

    while (input.size > 0) {
        char next = input.ptr[input.size - 1];
        if (next >= '0' && next <= '9') {} else {cwpanic("failed assertion");};

        output += (next - '0');
        input = cwstr_substr(input, 0, input.size - 1);
    }

    if (negative) output *= -1;

    return output;
}

double cwparse_whole_part(CwStr input) {
    double output = 0.0;
    double power = 1.0;
    while (input.size) {
		char next = input.ptr[input.size - 1];
		if (next < '0' || next > '9') {
    		cwpanic("tried to parse invalid character"); 
		}

		output += ((double)(next - '0') * power);
		power *= 10;
		input.size -= 1;
    }

    return output;
}

double cwparse_fraction_part(CwStr input) {
    double output = 0.0;
    double power = 1.0;
    while (input.size) {
		char next = input.ptr[0];
		if (next < '0' || next > '9') {
    		cwpanic("tried to parse invalid character"); 
		}

		output += ((double)(next - '0') * power);
		power *= 0.1;
		input = cwstr_substr(input, 1, input.size);
    }

    return output;
}

double cwparse_double(CwStr input) {
    int sign = 1;
    if (input.size >= 1 && input.ptr[0] == '-') {
        sign = -1;
        input = cwstr_substr(input, 1, input.size);
    }

    CwStr whole_part = cwstr_split(&input, '.');
    CwStr fraction_part = input;

    cwlog("%w", whole_part);
    cwlog("%w", fraction_part);

	double output = 0.0;
	output += cwparse_whole_part(whole_part);
	output += cwparse_fraction_part(fraction_part);

	return output * sign;
}

CwStr cwstr_split(CwStr* self, char c) {
	int index = cwstr_find(*self, c);
	if (index < 0) {
    	CwStr output = *self;
    	*self = cwstr_empty();
    	return output;
	}

	CwStr output = cwstr_substr(*self, 0, index);
	*self = cwstr_substr(*self, index + 1, self -> size);

	return output;
}

CwStr cwpath_get_file(CwStr input) {
	int separator = cwstr_find_last(input, '/');
	if (separator <= input.size) {} else {cwpanic("failed assertion");};

	if (separator < 0) return input;
	if (separator == input.size) return cwstr_empty();

	return cwstr_substr(input, separator + 1, input.size);
}


CwStr cwpath_get_dir(CwStr input) {
	int separator = cwstr_find_last(input, '/');
	if (separator <= input.size) {} else {cwpanic("failed assertion");};

	if (separator < 1) return cwstr_empty();
	if (separator == input.size) return input;

	return cwstr_substr(input, 0, separator - 1);
}

CwStr cwpath_get_base(CwStr input) {
	CwStr filename = cwpath_get_file(input);

	int separator = cwstr_find(filename, '.');
	if (separator < 1) return cwstr_empty();

	return cwstr_substr(filename, 0, separator);

}

CwStr cwpath_get_ext(CwStr input) {
	CwStr filename = cwpath_get_file(input);

	int separator = cwstr_find(filename, '.');
	if (separator >= filename.size) return cwstr_empty();

	return cwstr_substr(filename, separator + 1, filename.size);
}
