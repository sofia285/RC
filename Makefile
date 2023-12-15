all:
	g++ AS.cpp -o AS
	g++ user.cpp -o user

clean:
	rm server user