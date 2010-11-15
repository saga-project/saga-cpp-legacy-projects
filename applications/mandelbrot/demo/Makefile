
all:

clean:
	-ps -ef | grep run_demo.sh | grep -v grep | cut -c 6-12 | xargs kill
	rm -f  demo.lock
	rm -f  demo.log
	rm -f  demo.stop

distclean: clean
	rm -rf demo-*/

restart: clean
	./run_demo.sh

