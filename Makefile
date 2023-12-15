all:
	g++ AS.cpp -o AS
	g++ user.cpp -o user

test: test.cpp
	g++ test.cpp -o test
	./test

clean:
	rm AS user
	rm -r ASDIR/USERS/112233