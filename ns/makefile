%.cpp.out: %.cpp
	g++ -g -o $@ $<
%.c.out: %.c
	gcc -lreadline -g -o $@ $<
clean:
	rm -rf *.out
%.py.out: %.py
	cp $< $@
	chmod +x $@
%.tcl.out: %.tcl
	cp $< $@
	chmod +x $@
%.pl.out: %.pl
	cp $< $@
	chmod +x $@
