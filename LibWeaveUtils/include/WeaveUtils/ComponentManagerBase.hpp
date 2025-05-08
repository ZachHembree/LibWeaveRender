#pragma once
#include "GlobalUtils.hpp"
#include "DynamicCollections.hpp"
#include "Span.hpp"
#include <algorithm>

namespace Weave
{
	/// <summary>
	/// Base class template for managing components associated with a parent object. Not thread safe.
	/// </summary>
	/// <typeparam name="ParentT">The type of the parent object that owns the components.</typeparam>
	/// <typeparam name="ComponentT">The base type of the components being managed.</typeparam>
	template<typename ParentT, typename ComponentT>
	class ComponentManagerBase
	{
	public:
		/// <summary>
		/// Base class for all components managed by ComponentManagerBase.
		/// </summary>
		class ComponentBase
		{
		public:
			MAKE_IMMOVABLE(ComponentBase);
			friend ComponentManagerBase;

			/// <summary>
			/// Alias for manager base type managing this component
			/// </summary>
			using ManagerT = ComponentManagerBase<ParentT, ComponentT>;

			/// <summary>
			/// Constructs a component base, associating it with a parent.
			/// </summary>
			ComponentBase(ParentT& parent, uint priority = 10) :
				pParent(&parent),
				id(-1),
				priority(priority)
			{ }

			virtual ~ComponentBase() {}

			/// <summary>
			/// Checks if the component is registered with a manager.
			/// </summary>
			bool GetIsRegistered(const ComponentManagerBase* pParent = nullptr)
			{
				if (pParent == nullptr)
					return id != uint(-1) && this->pParent != nullptr;
				else
					return id != uint(-1) && this->pParent == pParent;
			}

			/// <summary>
			/// Gets the update priority of the component. Lower values update sooner.
			/// </summary>
			uint GetPriority() const { return priority; }

		protected:
			ParentT* pParent;
		private:
			uint id;
			uint priority;
		};

		/// <summary>
		/// Unique pointer managing the lifetime of a component.
		/// </summary>
		using CompHandle = std::unique_ptr<ComponentT>;

		/// <summary>
		/// Alias for the type of this manager's components
		/// </summary>
		using CompBaseT = ComponentBase;

		/// <summary>
		/// Constructs and registers a new parent object component in place. Not thread safe.
		/// </summary>
		template <typename T, typename... ArgTs>
		T& CreateComponent(ArgTs&&... args)
		{
			ParentT* pParent = static_cast<ParentT*>(this);
			ComponentT* pComp = RegisterComponent(CompHandle(new T(*pParent, std::forward<ArgTs>(args)...)));
			WV_ASSERT(pComp != nullptr);
			return static_cast<T&>(*pComp);
		}

		/// <summary>
		/// Constructs and registers a new parent object component in place. Not thread safe.
		/// </summary>
		template <typename T, typename... ArgTs>
		void CreateComponent(T*& pDerived, ArgTs&&... args)
		{
			ParentT* pParent = static_cast<ParentT*>(this);
			ComponentT* pBase = RegisterComponent(CompHandle(new T(*pParent, std::forward<ArgTs>(args)...)));
			WV_ASSERT(pBase != nullptr);
			pDerived = static_cast<T*>(pBase);
		}

		/// <summary>
		/// Transfers ownership of the component to the parent object and registers it. Not thread safe.
		/// </summary>
		ComponentT* RegisterComponent(CompHandle&& pComp)
		{
			WV_CHECK_MSG(!pComp->GetIsRegistered(this),
				"Cannot register a component that is already registered to another parent.");

			pComp->id = (uint)components.GetLength();
			pComp->pParent = static_cast<ParentT*>(this);
			CompHandle& newHandle = components.EmplaceBack(std::move(pComp));

			isCompSortingStale = true;
			return newHandle.get();
		}

		/// <summary>
		/// Unregisters the component from the parent object and destroys it. Not thread safe.
		/// </summary>
		void UnregisterComponent(ComponentT& component)
		{
			WV_CHECK_MSG(component.GetIsRegistered(this),
				"Attempted to remove a component that did not belong to the parent object");
			WV_ASSERT_MSG(component.id != uint(-1) && component.id < components.GetLength(), "Component ID invalid.");

			if (components[component.id].get() == &component)
			{
				const uint lastID = component.id;
				component.id = uint(-1);
				component.pParent = nullptr;
				components[lastID].reset();
				areCompIDsStale = true;
			}
		}
	protected:
		using CompSpan = Span<ComponentT*>;

		ComponentManagerBase() :
			isCompSortingStale(false),
			areCompIDsStale(false)
		{ }

		virtual ~ComponentManagerBase() = default;

		/// <summary>
		/// Returns a span of components sorted by priority
		/// </summary>
		CompSpan GetComponents()
		{
			UpdateComponentIDs();
			return CompSpan(sortedComps.GetData(), sortedComps.GetLength());
		}

	private:
		UniqueVector<CompHandle> components;
		UniqueVector<ComponentT*> sortedComps;
		bool isCompSortingStale;
		bool areCompIDsStale;

		/// <summary>
		/// Updates internal component lists: removes nulls, resorts by priority, and updates component IDs.
		/// Call this before iterating through components if additions/removals have occurred.
		/// </summary>
		void UpdateComponentIDs()
		{
			// Remove nulls and compact
			if (areCompIDsStale)
			{
				for (sint i = (sint)components.GetLength() - 1; i >= 0; i--)
				{
					if (components[i].get() == nullptr)
					{
						components.RemoveAt(i);
					}
				}

				isCompSortingStale = true;
			}

			// Sort components
			if (isCompSortingStale)
			{
				sortedComps.Resize(components.GetLength());

				for (uint i = 0; i < (uint)components.GetLength(); i++)
					sortedComps[i] = components[i].get();

				std::sort(sortedComps.begin(), sortedComps.end(), [](const ComponentT* pLeft, const ComponentT* pRight)
				{
					return pLeft->priority < pRight->priority;
				});

				isCompSortingStale = false;
				areCompIDsStale = true;
			}

			// Update IDs
			if (areCompIDsStale)
			{
				for (ulong i = 0; i < components.GetLength(); i++)
					components[i]->id = (uint)i;

				areCompIDsStale = false;
			}
		}
	};
}