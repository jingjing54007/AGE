#include "SharedMesh.hh"

#include "Parsers/Parsers.hh"

#include <assert.h>

namespace	Resources
{

SharedMesh::SharedMesh(void)
{
}

SharedMesh::~SharedMesh(void)
{
}

bool	SharedMesh::load(std::string const &path)
{
	if (loadObj(path, _geometry) == false)
		return (false);
	//_buffer.init();
	std::cout << "size : " << _geometry.vertices.size() << std::endl;
	std::cout << "size : " << _geometry.colors.size() << std::endl;
	std::cout << "size : " << _geometry.normals.size() << std::endl;
	std::cout << "size : " << _geometry.uvs.size() << std::endl;
	std::array<Data, 4> data = {Data(_geometry.vertices.size() * 3 * sizeof(float), &_geometry.vertices[0].x),
		Data(_geometry.colors.size() * 3 * sizeof(float), &_geometry.colors[0].x),
		Data(_geometry.normals.size() * 3 * sizeof(float), &_geometry.normals[0].x),
		Data(_geometry.uvs.size() * 2 * sizeof(float), &_geometry.uvs[0].x)};
	Data indicesData(_geometry.indices.size() * sizeof(unsigned int), &_geometry.indices[0]);
	Vertice<4> obj(_geometry.vertices.size(), data, &indicesData);
	std::cout << "nombre de octet dans le buffer de geometry : " << obj.getSizeVertexBuffer() << std::endl;
	std::cout << "nombre de octet dans le buffer de indices : " << obj.getSizeIndicesBuffer() << std::endl;
	/*_buffer.setIndices(_geometry.indices.size(), &_geometry.indices[0]);
	_buffer.addAttribute(_geometry.vertices.size(), 4, sizeof(float), reinterpret_cast<OpenGLTools::Byte *>(&_geometry.vertices[0].x));
	_buffer.addAttribute(_geometry.colors.size(), 4, sizeof(float), reinterpret_cast<OpenGLTools::Byte *>(&_geometry.colors[0].x));
	_buffer.addAttribute(_geometry.normals.size(), 4, sizeof(float), reinterpret_cast<OpenGLTools::Byte *>(&_geometry.normals[0].x));
	_buffer.addAttribute(_geometry.uvs.size(), 2, sizeof(float), reinterpret_cast<OpenGLTools::Byte *>(&_geometry.uvs[0].x));
	_buffer.transferGPU(GL_STREAM_DRAW);*/
	assert(_geometry.vertices.size() > 0 && "Cannot create mesh without vertices.");
	return (true);
}

void		SharedMesh::draw() const
{
	//_buffer.draw(GL_TRIANGLES);
}

OpenGLTools::VertexArray	&SharedMesh::getBuffer()
{
	return (_buffer);
}

const Geometry      &SharedMesh::getGeometry() const
{
	return _geometry;
}

}