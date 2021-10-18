all: 
	make html
	make pdf

clean:
	rm -rf latex
	rm -rf html
html:
	mkdir -p latex
	cp templates/*.tex latex/.
	doxygen

pdf:
	cd latex; make
	mv latex/refman.pdf "../PSP_SP0-VXWORKS6.9_sdd_s5.pdf"
