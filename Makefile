all:
	g++ -o js -I/opt/local/include -L/opt/local/lib -lonig *.cpp libv8.a