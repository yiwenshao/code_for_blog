.PHONY:all

all:Embed full

full:full.cc
	g++ -o full full.cc `mysql_config --include --libmysqld-libs` -std=c++11

Embed:Embed.cc
	g++ -o Embed Embed.cc `mysql_config --include --libmysqld-libs` -std=c++11
.PHONY:clean
clean:
	rm -rf Embed shadow full
