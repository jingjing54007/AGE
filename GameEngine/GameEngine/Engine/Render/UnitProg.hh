#pragma once

# include <Utils/OpenGL.hh>
# include <iostream>

class UnitProg
{
public:
	UnitProg(std::string const &filename, GLenum type);
	UnitProg(UnitProg const &copy) = delete;
	UnitProg(UnitProg &&move);
	UnitProg &operator=(UnitProg const &u) = delete;
	UnitProg &operator=(UnitProg &&u) = delete;
	~UnitProg();

public:
	GLenum getType() const;
	GLuint getId() const;
	bool compile();
	void destroy();
private:
	bool compileUnitProg(char const *fileName);

private:
	std::string _filename;
	GLenum _type;
	GLuint _id;
};