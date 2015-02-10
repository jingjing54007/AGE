#pragma once

#include <cstddef>
#include <utility>
#include <cstdint>
#include <bitset>

#include "EntityTypedef.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/vector.hpp>

#include <Core/Link.hpp>

namespace AGE
{
	class EntityData;
	class EntityFilter;
	struct ComponentBase;
	class ComponentManager;

	class Entity
	{
	public:
		Entity()
			: id(0)
			, version(0)
			, flags(0)
		{}

		~Entity()
		{}

		Entity(const Entity &o)
		{
			id = o.id;
			version = o.version;
			flags = o.flags;
			ptr = o.ptr;
		}

		Entity(Entity &&o)
		{
			id = std::move(o.id);
			version = std::move(o.version);
			flags = std::move(o.flags);
			ptr = o.ptr;
		}

		Entity &operator=(const Entity &o)
		{
			id = o.id;
			version = o.version;
			flags = o.flags;
			ptr = o.ptr;
			return *this;
		}

		bool operator==(const Entity &o) const
		{
			return version == o.version && id == o.id;
		}

		bool operator!=(const Entity &o) const
		{
			return !(version == o.version && id == o.id);
		}

		bool operator<(const Entity &o) const
		{
			return id < o.id;
		}

		bool operator<=(const Entity &o) const
		{
			return id <= o.id;
		}

		bool operator>(const Entity &o) const
		{
			return id > o.id;
		}

		bool operator>=(const Entity &o) const
		{
			return id >= o.id;
		}

		inline ENTITY_ID getId() const
		{
			return id;
		}

		inline ENTITY_VERSION getVersion() const
		{
			return version;
		}

		inline const ENTITY_FLAGS &getFlags() const
		{
			return flags;
		}

		inline ENTITY_FLAGS &setFlags()
		{
			return flags;
		}

		const AGE::Link &getLink() const;
		AGE::Link &getLink();
		const AGE::Entity *getPtr() const;
		AGE::Entity *getPtr();

		ComponentBase *getComponent(ComponentType id);

		template <typename T>
		T *getComponent()
		{
			return static_cast<T*>(ptr->getComponent(Component<T>::getTypeId()));
		}

		template <typename T, typename... Args>
		T *addComponent(Args &&...args)
		{
			return ptr->addComponent<T>(args...);
		}

		void removeComponent(ComponentType id);

		template <typename T>
		void removeComponent()
		{
			ptr->removeComponent(Component<T>::getTypeId());
		}

		bool haveComponent(ComponentType id) const;

		template <typename T>
		bool haveComponent() const
		{
			return ptr->haveComponent(Component<T>::getTypeId());
		}
	private:
		ENTITY_ID id;
		ENTITY_VERSION version;
		ENTITY_FLAGS flags;
		EntityData *ptr;

		friend AScene;
		friend EntityFilter;
		friend ComponentManager;
	};
}

namespace std
{
	template<>
	struct hash < AGE::Entity >
	{
		typedef AGE::Entity argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& s) const
		{
			result_type res = 0;
			res = s.getId();
			return res;
		}
	};
}