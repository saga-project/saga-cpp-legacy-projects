all:
	@echo DIANE_DOXYGEN_PROJECT_NUMBER=\"${DIANE_DOXYGEN_PROJECT_NUMBER}\"
	mkdir -p generated_src
	@echo making related pages
	@python make_related_pages.py > generated_src/related_pages.py
	(cat Doxyfile; echo "PROJECT_NUMBER=${DIANE_DOXYGEN_PROJECT_NUMBER}") | doxygen -
	@cat doxygen.log
	cp src/DIANE-logo.png reference/html
	@echo '<html><body> <p><b><a href="html/index.html">API reference</a></b></p><p><b><a href="../../python/diane/test/output/stdout.txt">test summary</a></b>,<b><a href="../../python/diane/test/output">output</a></b></p></body></html>' > reference/index.html

clean: cleantmp
	rm -rf reference

cleantmp:
	rm -rf generated_src doxygen.log

