

namespace gl
{

	template <typename TYPE>
	AGE::Vector<InternalData> Key<TYPE>::_data;

	template <typename TYPE>
	Key<TYPE>::Key()
	{
		_id = -1;
	}

	template <typename TYPE>
	Key<TYPE>::~Key()
	{
	}

	template <typename TYPE>
	Key<TYPE>::Key(Key<TYPE> const &copy) 
		: _id(copy._id)
	{
	}

	template <typename TYPE>
	Key<TYPE> &Key<TYPE>::operator=(Key<TYPE> const &k)
	{
		_id = k._id;
		return (*this);
	}

	template <typename TYPE>
	std::size_t Key<TYPE>::getId() const
	{
		return (_id);
	}

	// Test the validity of the key
	template <typename TYPE>
	bool Key<TYPE>::empty() const
	{
		return (_id == -1);
	}

	template <typename TYPE>
	bool Key<TYPE>::operator!() const
	{
		return (empty());
	}

	template <typename TYPE>
	bool Key<TYPE>::operator==(Key<TYPE> const &compare) const
	{
		return (_id == compare._id);
	}

	template <typename TYPE>
	bool Key<TYPE>::operator!=(Key<TYPE> const &compare) const
	{
		return (!(*this == compare));
	}

	// render the key invalid to the use
	template <typename TYPE>
	void Key<TYPE>::destroy()
	{
		_id = -1;
	}

	template <typename TYPE>
	bool Key<TYPE>::operator<(Key<TYPE> const &compare) const
	{
		return (_id < compare._id);
	}

	template <typename TYPE>
	bool Key<TYPE>::operator>(Key<TYPE> const &compare) const
	{
		return (_id > compare._id);
	}

	template <typename TYPE>
	Key<TYPE>::Key(size_t id)
	{
		_id = id;
	}

}