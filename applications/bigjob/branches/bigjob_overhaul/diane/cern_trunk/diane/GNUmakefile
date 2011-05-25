.PHONY: all clean cleantmp doc test

all:
	$(MAKE) -C idl all

clean: cleantmp
	$(MAKE) -C idl clean
	$(MAKE) -C doc clean
	$(MAKE) -C python/diane/test clean

cleantmp:
	rm -f `find . -name '*.pyc' -or -name '*.pyo' -or -name '*.py~' -or -name 'MasterOID' -or -name 'WorkerOID' -or -name 'ServletOID*' -or -name 'tasks.txt' -or -name 'crash.pp' -or -name 'ServletWrapper-*'`
	$(MAKE) -C python/diane/test cleantmp
	$(MAKE) -C doc cleantmp

doc: all
	$(MAKE) -C doc

test: all
	$(MAKE) -C python/diane/test

compile:
	python -c "import compileall; compileall.compile_dir('.',1000,force=1)"

