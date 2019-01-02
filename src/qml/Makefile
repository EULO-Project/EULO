all:eulo-qml
eulo-qml:
	@echo "Building eulo-qml ...depends_prefix: " ${depends_prefix} && \
	mkdir -p build && \
	${depends_prefix}/qml/bin/lrelease qml.pro && \
	cd build  && \
	${depends_prefix}/qml/bin/qmake ../qml.pro  "depends_prefix=$${depends_prefix}" && \
	make
	cd ../
