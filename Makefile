# Makefile for Recursive Ray Tracer

build:
	clang++ -O3 `fltk-config --use-gl --ldflags` -fsanitize=address MyGLCanvas.cpp ./scene/SceneParser.cpp ./scene/tinyxmlparser.cpp ./scene/tinyxmlerror.cpp ./scene/tinyxml.cpp ./scene/tinystr.cpp Camera.cpp main.cpp FlatSceneTree.cpp RayTracer.cpp -Wno-deprecated-declarations -Wno-nonportable-include-path -o rrt

clean:
	rm rrt
