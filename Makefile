DEPS := libreadline6 libreadline6-dev

unsh: bin/repl.o bin/prompt.o bin/unicorn.o bin/signals.o
	${CC} -o unsh $^ -lreadline

bin/%.o: src/%.c include/unicorn.h
	${CC} -Wall -std=c99 -c -Iinclude $(CFLAGS) $< -o $@

deps:
	sudo apt-get install ${DEPS}

clean:
	rm -rf bin/*.o unsh
