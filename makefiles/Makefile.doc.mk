# Generate documentation

cpp-doc:
	doxygen tools/cpp.doxy

java-doc:
	doxygen tools/java.doxy

python-doc:
	pdoc3 --html -o docs/python/ortools/sat/python/ ortools/sat/python/cp_model.py
	pdoc3 --html -o docs/python/ortools/util/ ortools/gen/ortools/util/sorted_interval_list.py
