DEPS := libreadline6 libreadline6-dev

unsh: bin/repl.o bin/prompt.o bin/unicorn.o
	cc -o unsh $^ -lreadline

bin/%.o: src/%.c include/unicorn.h
	cc -c -Iinclude $(CFLAGS) $< -o $@

deps:
	sudo apt-get install ${DEPS}

clean:
	rm bin/*.o unsh
