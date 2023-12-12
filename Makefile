all:
	g++ user.cpp -o user

debug:
	g++ -g user.cpp -o user

clean:
	rm user