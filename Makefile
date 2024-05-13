

test: build/out
	build/out

build/out: clean
	@mkdir -p build
	cc main.c -o build/out


clean:
	@rm -rf build

