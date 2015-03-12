#pragma once

#include <Utils/DependenciesInjector.hpp>
#include <memory>
#include <Components/ComponentRegistrationManager.hpp>
#include <Core/EntityIdRegistrar.hh>
#include <list>
#include <array>
#include <glm/fwd.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/map.hpp>
#include <Utils/Containers/Queue.hpp>
#include <Entities/EntitySerializationInfos.hpp>
#include <Utils/ObjectPool.hpp>
#include <unordered_set>
#include <Components/ComponentManager.hpp>

namespace AGE
{
	class Engine;
	class RenderScene;
	class EntityFilter;
	class System;
	class SceneManager;

	class AScene : public DependenciesInjector, public EntityIdRegistrar, public ComponentManager
	{
	private:
		std::multimap<std::size_t, std::shared_ptr<System> >                    _systems;
		std::array<std::list<EntityFilter*>, MAX_CPT_NUMBER + MAX_TAG_NUMBER>   _filters;
		std::list<EntityFilter*>                                                 _allFilters;
		AGE::ObjectPool<EntityData>                                             _entityPool;
		AGE::Queue<std::uint16_t>                                               _freeEntityId;
		std::unordered_set<Entity>                                              _entities;
		ENTITY_ID                                                               _entityNumber;
		AGE::RenderScene                                                        *_renderScene;
		bool                                                                    _active;
		friend EntityFilter;
		friend class AGE::RenderScene;
		friend class AGE::SceneManager;
	protected:
		AGE::Engine *                                              _engine;
		inline void setActive(bool tof) { _active = tof; }
	public:
		AScene(AGE::Engine *engine);
		virtual ~AScene();
		inline std::size_t      getNumberOfEntities() const { return _entities.size(); }
		virtual bool 			userStart() = 0;
		virtual bool 			userUpdateBegin(double time) = 0;
		virtual bool            userUpdateEnd(double time) = 0;
		void 					update(double time);
		bool                    start();
		inline AGE::Engine *getEngine() { return _engine; }
		inline void setRenderScene(AGE::RenderScene *renderScene) { _renderScene = renderScene; }
		inline bool isActive() const { return _active; }

		void                    registerFilter(EntityFilter *filter);
		void                    filterSubscribe(ComponentType id, EntityFilter* filter);
		void                    filterUnsubscribe(ComponentType id, EntityFilter* filter);

		void                    informFiltersTagAddition(TAG_ID id, const EntityData &entity);
		void                    informFiltersTagDeletion(TAG_ID id, const EntityData &entity);
		void                    informFiltersComponentAddition(ComponentType id, const EntityData &entity);
		void                    informFiltersComponentDeletion(ComponentType id, const EntityData &entity);
		void                    informFiltersEntityCreation(const EntityData &entity);
		void                    informFiltersEntityDeletion(const EntityData &entity);

		Entity &createEntity();
		void destroy(const Entity &e);
		void clearAllEntities();

		template <typename T>
		std::shared_ptr<T> addSystem(std::size_t priority)
		{
			auto tmp = std::make_shared<T>((AScene*)(this));
			if (!tmp->init())
				return nullptr;
			_systems.insert(std::make_pair(priority, tmp));
			return tmp;
		}

		template <typename T>
		std::shared_ptr<T> getSystem()
		{
			for (auto &e : _systems)
			{
				if (typeid(*e.second.get()).name() == typeid(T).name())
					return std::static_pointer_cast<T>(e.second);
			}
			return nullptr;
		}

		template <typename T>
		void deleteSystem()
		{
			for (auto &e : _systems)
			{
				if (typeid(*e.second.get()).name() == typeid(T).name())
				{
					delete e.second;
					_systems.erase(e);
					return;
				}
			}
		}

		template <typename T>
		bool activateSystem()
		{
			for (auto &e : _systems)
			{
				if (typeid(*e.second.get()).name() == typeid(T).name())
					return e.second->setActivation(true);
			}
			return false;
		}

		template <typename T>
		bool deactivateSystem()
		{
			for (auto &e : _systems)
			{
				if (typeid(*e.second.get()).name() == typeid(T).name())
					return e.second->setActivation(false);
			}
			return false;
		}

		void saveToJson(const std::string &fileName);
		void loadFromJson(const std::string &fileName);
		void saveToBinary(const std::string &fileName);
		void loadFromBinary(const std::string &fileName);

		template <typename Archive>
		void load(std::ifstream &s)
		{
			// @ECS TODO
			
			//Archive ar(s);

			//std::uint16_t size = 0;
			//ar(size);
			//for (unsigned int i = 0; i < size; ++i)
			//{
			//	auto &e = createEntity();
			//	auto &ed = *e.ptr;

			//	EntitySerializationInfos infos(ed);
			//	ar(infos);
			//	e.flags = infos.flags;
			//	ed.link.setPosition(infos.link.getPosition());
			//	ed.link.setOrientation(infos.link.getOrientation());
			//	ed.link.setScale(infos.link.getScale());

			//	for (auto &hash : infos.componentsHash)
			//	{
			//		std::size_t componentTypeId;
			//		auto ptr = ComponentRegistrar::getInstance().createComponentFromType(hash, ar, componentTypeId, this);
			//		ed.barcode.setComponent(componentTypeId);
			//		assert(_componentsManagers[componentTypeId] != nullptr);
			//		_componentsManagers[componentTypeId]->addComponentPtr(e, ptr);
			//		informFiltersComponentAddition(componentTypeId, ed);
			//	}
			//	//	ar(*e.get());
			//}
			////updateEntityHandles();
		}


		////////////////////////
		///////
		// Component Manager Get / Set

		template <typename T>
		void clearComponentsType()
		{
			//TODO
			auto id = Component<T>::getTypeId();
			if (_componentsManagers[id] == nullptr)
				return;
			auto &manager = *static_cast<ComponentManager<T>*>(_componentsManagers[id]);
			auto &col = manager.getComponents();
			for (std::size_t i = 0; i < manager.getSize(); ++i)
			{
				_entityPool[col[i].entityId].barcode.unsetComponent(ComponentType(id));
			}
			manager.clearComponents();
			for (auto filter : _filters[id])
			{
				filter->clearCollection();
			}
		}

		void addTag(Entity &e, TAG_ID tag);
		void removeTag(Entity &e, TAG_ID tag);
		bool isTagged(Entity &e, TAG_ID tag);
	};
}
