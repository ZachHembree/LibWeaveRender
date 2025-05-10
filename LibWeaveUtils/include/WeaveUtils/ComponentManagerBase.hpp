#pragma once
#include <atomic>
#include <mutex>
#include "GlobalUtils.hpp"
#include "DynamicCollections.hpp"
#include "MutexSpan.hpp"

namespace Weave
{
	/// <summary>
	/// Base class template for managing components associated with a parent object. Thread safe.
	/// </summary>
	/// <typeparam name="ParentT">The type of the parent object that owns the components.</typeparam>
	/// <typeparam name="ComponentT">The base type of the components being managed.</typeparam>
	template<typename ParentT, typename ComponentT>
	class ComponentManagerBase
	{
	public:
		static constexpr uint InvalidID = -1;
		static constexpr uint PendingID = -2;

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
					return id != InvalidID && this->pParent != nullptr;
				else
					return id != InvalidID && this->pParent == pParent;
			}

			/// <summary>
			/// Gets the update priority of the component. Lower values update sooner.
			/// </summary>
			uint GetPriority() const { return priority; }

		protected:
			std::atomic<ParentT*> pParent;
		private:
			std::atomic<uint> id;
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
		/// Constructs and registers a new parent object component in place. Thread safe.
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
		/// Constructs and registers a new parent object component in place. Thread safe.
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
		/// Transfers ownership of the component to the parent object and registers it. Thread safe.
		/// </summary>
		ComponentT* RegisterComponent(CompHandle&& pComp)
		{
			WV_CHECK_MSG(!pComp->GetIsRegistered(this),
				"Cannot register a component that is already registered to another parent.");

			std::unique_lock<std::mutex>(queueMutex);
			pComp->pParent = static_cast<ParentT*>(this);
			pComp->id = PendingID;
			CompHandle& newHandle = regQueues[activeQueue].EmplaceBack(std::move(pComp));

			return newHandle.get();
		}

		/// <summary>
		/// Unregisters the component from the parent object and destroys it. Thread safe.
		/// Contends lock with GetComponents().
		/// </summary>
		void UnregisterComponent(ComponentT& component)
		{
			WV_CHECK_MSG(component.GetIsRegistered(this),
				"Attempted to remove a component that did not belong to the parent object");

			std::unique_lock<std::mutex>(compMutex);
			WV_ASSERT_MSG(component.id != InvalidID && component.id < components.GetLength(), "Component ID invalid.");

			if (components[component.id].get() == &component)
			{
				const uint lastID = component.id;
				component.id = InvalidID;
				component.pParent = nullptr;
				components[lastID].reset();
				areCompIDsStale = true;
			}
		}
	protected:
		using CompSpan = MutexSpan<ComponentT*>;

		ComponentManagerBase() :
			isCompSortingStale(false),
			areCompIDsStale(false),
			activeQueue(0),
			inactiveQueue(1)
		{ }

		virtual ~ComponentManagerBase() = default;

		/// <summary>
		/// Returns a temporary span of components, sorted by priority. Protected by a mutex 
		/// for the lifetime of the span. Contends lock with UnregisterComponent().
		/// </summary>
		CompSpan GetComponents()
		{
			UpdateComponentIDs();
			return CompSpan(sortedComps, compMutex);
		}

	private:
		std::array<UniqueVector<CompHandle>, 2> regQueues;
		std::atomic<uint> activeQueue, inactiveQueue;
		std::mutex queueMutex;

		UniqueVector<CompHandle> components;
		UniqueVector<ComponentT*> sortedComps;
		std::mutex compMutex;
		bool isCompSortingStale;
		bool areCompIDsStale;

		void SwapActiveQueues()
		{
			std::unique_lock<std::mutex>(queueMutex);
			const uint lastQueue = activeQueue;
			activeQueue = inactiveQueue.load();
			inactiveQueue = lastQueue;
		}

		/// <summary>
		/// Updates internal component lists: removes nulls, resorts by priority, and updates component IDs.
		/// Call this before iterating through components if additions/removals have occurred.
		/// </summary>
		void UpdateComponentIDs()
		{
			SwapActiveQueues();
			UniqueVector<CompHandle>& newComps = regQueues[inactiveQueue];
			std::unique_lock<std::mutex>(compMutex);

			// Remove nulls and compact
			if (areCompIDsStale)
			{
				for (sint i = (sint)components.GetLength() - 1; i >= 0; i--)
				{
					if (components[i].get() == nullptr)
						components.RemoveAt(i);
				}

				isCompSortingStale = true;
			}

			// Add pending components
			if (!newComps.IsEmpty())
			{
				for (CompHandle& comp : newComps)
					components.Add(std::move(comp));

				newComps.Clear();
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