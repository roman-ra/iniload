all: png svg

png:
	dot -Tpng parsergraph.gv > parsergraph.png

svg:
	dot -Tsvg parsergraph.gv > parsergraph.svg

.PHONY: clean
clean:
	rm -f parsergraph.png parsergraph.svg
