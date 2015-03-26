#include "Entity.hh"
#include <Core/AScene.hh>

namespace AGE
{
	EntityData::EntityData(AScene *_scene)
		: link(this, _scene)
		, scene(_scene)
		, outOfContext(false)
	{
	}

	const Entity &EntityData::getEntity() const { return entity; }
	Entity &EntityData::getEntity() { return entity; }
	const AGE::Link &EntityData::getLink() const { return link; }
	AGE::Link &EntityData::getLink() { return link; }
	AScene *EntityData::getScene() { return scene; }

	ComponentBase *EntityData::getComponent(ComponentType id)
	{
		if (!haveComponent(id))
			return nullptr;
		return components[id];
	}

	void EntityData::addComponentPtr(ComponentBase *cpt)
	{
		auto id = cpt->getType();
		if (haveComponent(id))
		{
			// if already have component of this type, we remove the old one
			removeComponent(id);
		}
		if (components.size() <= id)
		{
			components.resize(id + 1, nullptr);
		}
		components[id] = cpt;
		scene->informFiltersComponentAddition(id, *this);
	}

	void EntityData::removeComponent(ComponentType id)
	{
		if (!haveComponent(id))
			return;
		components[id]->reset();
		scene->deleteComponent(components[id]);
		components[id] = nullptr;
		scene->informFiltersComponentDeletion(id, *this);
	}

	bool EntityData::haveComponent(ComponentType id) const
	{
		return !(components.size() <= id || components[id] == nullptr);
	}

	/////////////////////////////////////

	Entity::Entity()
		: id(0)
		, version(0)
		, flags(0)
	{}

	Entity::~Entity()
	{}

	Entity::Entity(const Entity &o)
	{
		id = o.id;
		version = o.version;
		flags = o.flags;
		ptr = o.ptr;
	}

	Entity::Entity(Entity &&o)
	{
		id = std::move(o.id);
		version = std::move(o.version);
		flags = std::move(o.flags);
		ptr = o.ptr;
	}

	Entity &Entity::operator=(const Entity &o)
	{
		id = o.id;
		version = o.version;
		flags = o.flags;
		ptr = o.ptr;
		return *this;
	}

	bool Entity::operator==(const Entity &o) const
	{
		return version == o.version && id == o.id;
	}

	bool Entity::operator!=(const Entity &o) const
	{
		return !(version == o.version && id == o.id);
	}

	bool Entity::operator<(const Entity &o) const
	{
		return id < o.id;
	}

	bool Entity::operator<=(const Entity &o) const
	{
		return id <= o.id;
	}

	bool Entity::operator>(const Entity &o) const
	{
		return id > o.id;
	}

	bool Entity::operator>=(const Entity &o) const
	{
		return id >= o.id;
	}

	Entity *Entity::getPtr()
	{
		return &ptr->getEntity();
	}

	const Entity *Entity::getPtr() const
	{
		return &ptr->getEntity();
	}

	AScene *Entity::getScene()
	{
		return ptr->getScene();
	}

	AGE::Link &Entity::getLink()
	{
		return ptr->getLink();
	}

	const AGE::Link &Entity::getLink() const
	{
		return ptr->getLink();
	}

	AGE::Link *Entity::getLinkPtr()
	{
		return &ptr->getLink();
	}

	const AGE::Link *Entity::getLinkPtr() const
	{
		return &ptr->getLink();
	}

	ComponentBase *Entity::getComponent(ComponentType id)
	{
		return ptr->getComponent(id);
	}

	void Entity::removeComponent(ComponentType id)
	{
		ptr->removeComponent(id);
	}

	bool Entity::haveComponent(ComponentType id) const
	{
		return ptr->haveComponent(id);
	}

	void Entity::addComponentPtr(ComponentBase *cpt)
	{
		ptr->addComponentPtr(cpt);
	}

	const std::vector<ComponentBase*> &Entity::getComponentList() const
	{
		return ptr->getComponentList();
	}
}