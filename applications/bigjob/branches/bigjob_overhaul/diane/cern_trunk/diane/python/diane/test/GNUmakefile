.PHONY: all clean cleantmp test

all: clean
	mkdir -p output
	python run.py 2>&1 | tee output/stdout.txt

cleantmp:
	echo

clean: cleantmp
	rm -rf output