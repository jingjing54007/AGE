#include <Render/GeometryManagement/Vertices.hh>
#include <Render/ProgramResources/Types/ProgramResourcesType.hh>
#include <Render/GeometryManagement/BlockMemory.hh>

Vertices::Vertices(std::vector<GLenum> &&types, size_t nbrVertex, size_t offset, size_t index) :
_index(index),
_offset(offset),
_nbr_vertex(nbrVertex),
_types(std::move(types)),
_data(_types.size()),
_indices_data(nbrVertex, 0),
_block_memories(_types.size())
{
	for (auto index = 0ull; index < _types.size(); ++index) {
		auto &iterator = available_types.find(_types[index]);
		if (iterator != available_types.end()) {
			auto &t = iterator->second;
			_data[index].resize(t.size * nbrVertex, 0);
		}
	}
}

Vertices::Vertices(Vertices const &copy) :
_index(copy._index),
_offset(copy._offset),
_nbr_vertex(copy._nbr_vertex),
_types(copy._types),
_data(copy._data),
_indices_data(copy._indices_data),
_block_memories(copy._block_memories),
_indices_block_memory(copy._indices_block_memory)
{
}

Vertices::Vertices(Vertices &&move) :
_index(move._index),
_offset(move._offset),
_nbr_vertex(move._nbr_vertex),
_types(std::move(move._types)),
_data(std::move(move._data)),
_indices_data(std::move(move._indices_data)),
_block_memories(std::move(move._block_memories)),
_indices_block_memory(std::move(move._indices_block_memory))
{

}

size_t Vertices::nbr_vertex() const
{
	return (_nbr_vertex);
}

size_t Vertices::nbr_buffer() const
{
	return (_data.size());
}


std::vector<uint8_t> &&Vertices::transfer_data(size_t index)
{
	return (std::move(_data[index]));
}

std::vector<uint8_t> &&Vertices::transfer_indices_data()
{
	return (std::move(_indices_data));
}

GLenum Vertices::get_type(size_t index) const
{
	return (_types[index]);
}

Vertices & Vertices::set_block_memory(std::shared_ptr<BlockMemory> const &blockMemory, size_t index)
{
	_block_memories[index] = blockMemory;
	return (*this);
}

Vertices &Vertices::set_indices_block_memory(std::shared_ptr<BlockMemory> const &blockMemory)
{
	_indices_block_memory = blockMemory;
	return (*this);
}

Vertices & Vertices::remove()
{
	auto index = 0ull;
	for (auto &block : _block_memories) {
		if (block.lock()) {
			block.lock()->remove();
		}
		else {
			_data[index].clear();
		}
		++index;
	}
	return (*this);
}

Vertices & Vertices::set_indices(std::vector<unsigned int> const &data)
{
	std::vector<uint8_t> tmp(data.size() * sizeof(unsigned int));
	std::memcpy(tmp.data(), data.data(), tmp.size());
	if (_indices_block_memory.lock()) {
		*_indices_block_memory.lock() = tmp;
		return (*this);
	}
	if (tmp.size() == _indices_data.size()) {
		return (*this);
	}
	_indices_data = tmp;
	return (*this);
}

Vertices & Vertices::draw(GLenum mode)
{
	if (_indices_block_memory.lock()) {
		auto offset = _indices_block_memory.lock()->offset();
		glDrawElements(mode, _nbr_vertex, GL_UNSIGNED_INT, (GLvoid *)offset);
	}
	else {
		glDrawArrays(mode, _offset, _nbr_vertex);
	}
	return (*this);
}

unsigned int const * Vertices::get_indices(size_t &size) const
{
	if (_indices_block_memory.lock()) {
		auto &to_show = _indices_block_memory.lock()->get();
		size = to_show.size() / sizeof(unsigned int);
		return ((unsigned int const *)to_show.data());
	}
	else {
		size = _indices_data.size() / sizeof(unsigned int);
		return ((unsigned int const *)_indices_data.data());
	}
}