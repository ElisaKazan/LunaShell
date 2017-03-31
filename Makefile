unsh: bin/repl.o bin/prompt.o bin/unicorn.o
	cc -o unsh $^

bin/%.o: src/%.c include/unicorn.h
	cc -c -Iinclude $(CFLAGS) $< -o $@

clean:
	rm bin/*.o unsh
