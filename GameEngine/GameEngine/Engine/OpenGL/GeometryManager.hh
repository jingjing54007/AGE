#pragma once

#include <Utils/OpenGL.hh>
#include <OpenGL/Key.hh>
#include <OpenGL/Buffer.hh>
#include <OpenGL/VertexArray.hh>
#include <OpenGL/Pool.hh>
#include <Utils/Dependency.hpp>
#include <map>
#include <vector>

namespace gl
{
	class Vertices;

	//!\file GeometryManager.hh
	//!\author Dorian Pinaud
	//!\version v1.0
	//!\class GeometryManager
	//!\brief Handle the geometry of the render
	class GeometryManager : public Dependency
	{
	public:
		template <typename TYPE, typename POOL>
		struct Attach
		{
			TYPE const *data;
			POOL *pool;
			Key<Pool::Element<TYPE>> element;
		};
	public:
		// constructor
		GeometryManager();
		~GeometryManager();

		// handle pools
		Key<VertexPool> addVertexPool();
		Key<VertexPool> addVertexPool(uint8_t nbrAttributes, GLenum *typeComponent, uint8_t *sizeTypeComponent, uint8_t *nbrComponent);
		Key<VertexPool> getVertexPool(size_t index) const;
		size_t nbrVertexPool() const;
		GeometryManager &rmVertexPool(Key<VertexPool> &key);
		Key<IndexPool> addIndexPool();
		Key<IndexPool> getIndexPool(size_t index) const;
		size_t nbrIndexPool() const;
		GeometryManager &rmIndexPool(Key<IndexPool> &key);

		//handle Vertices and Indices
		Key<Vertices> addVertices(size_t nbrVertices, uint8_t nbrBuffers, size_t *sizeBuffers, void **buffers);
		Key<Indices> addIndices(size_t nbrIndices, uint32_t *buffers);
		GeometryManager &rmVertices(Key<Vertices> &key);
		GeometryManager &rmIndices(Key<Indices> &key);
		Key<Vertices> getVertices(size_t index) const;
		Key<Indices> getIndices(size_t index) const;
		size_t getNbrVertices() const;
		size_t getNbrIndices() const;

		// attach vertices to pools
		GeometryManager &attachVerticesToVertexPool(Key<Vertices> const &vertices, Key<VertexPool> const &pool);
		GeometryManager &dettachVerticesToVertexPool(Key<Vertices> const &vertices);
		GeometryManager &attachIndicesToIndexPool(Key<Indices> const &vertices, Key<IndexPool> const &pool);
		GeometryManager &dettachIndicesToIndexPool(Key<Indices> const &vertices);
		GeometryManager &attachIndexPoolToVertexPool(Key<VertexPool> const &vertexpool, Key<IndexPool> const &indicespool);
		GeometryManager &dettachIndexPoolToVertexPool(Key<VertexPool> const &vertexpool, Key<IndexPool> const &indexpool);

		// draw
		GeometryManager &draw(GLenum mode, Key<Indices> const &keyindices, Key<Vertices> const &keyVertice);
		GeometryManager &draw(GLenum mode, Key<Vertices> const &keyvertices);
	private:
		// data represent pools
		std::map<Key<IndexPool>, IndexPool> _indexPool;
		std::map<Key<VertexPool>, VertexPool> _vertexPool;
		std::map<Key<Indices>, Indices>	_indices;
		std::map<Key<Vertices>, Vertices> _vertices;
		std::map<Key<Vertices>, Attach<Vertices, VertexPool>> _vertexAttach;
		std::map<Key<Indices>, Attach<Indices, IndexPool>> _indexAttach;
	};
}