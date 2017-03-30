unsh: bin/repl.o bin/prompt.o bin/unicorn.o
	cc -o unsh $^

bin/%.o: src/%.c 
	cc -c -Iinclude $(CFLAGS) $< -o $@

clean:
	rm bin/*.o unsh
