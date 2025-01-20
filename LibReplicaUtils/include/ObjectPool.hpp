#pragma once
#include <ReplicaUtils.hpp>
#include <stack>

namespace Replica
{
	/// <summary>
	/// Move-only object pool template
	/// </summary>
	template<typename T>
	class ObjectPool
	{
	public:
		MAKE_MOVE_ONLY(ObjectPool)

		ObjectPool() : objectsOutstanding(0) { }

		/// <summary>
		/// Retrieves and releases ownership of an object from the
		/// pool
		/// </summary>
		virtual T Get()
		{
			objectsOutstanding++;

			if (!objectPool.empty())
			{
				T object = std::move(objectPool.top());
				objectPool.pop();
				return object;
			}
			else
			{
				return T();
			}
		}

		/// <summary>
		/// Returns an object to the pool and takes ownership of it
		/// </summary>
		virtual void Return(T&& object)
		{
			objectPool.push(std::move(object));
			objectsOutstanding--;

			REP_ASSERT_MSG(objectsOutstanding >= 0, "More objects returned to the pool than issued.")
		}

		/// <summary>
		/// Returns the number of objects currently in use
		/// </summary>
		int GetObjectsOutstanding() const { return objectsOutstanding; }

		/// <summary>
		/// Returns the number of objects available in the pool
		/// </summary>
		int GetObjectsAvailable() const { return (int)objectPool.size(); }

		/// <summary>
		/// Returns the total number of objects tied to the pool, including objects
		/// stored for reuse and objects currently in-use.
		/// </summary>
		int GetTotalObjects() const { return GetObjectsOutstanding() + GetObjectsAvailable(); }

		/// <summary>
		/// Trims the given number of objects from the pool
		/// </summary>
		virtual void TrimPool(size_t count)
		{
			count = std::min(count, objectPool.size());

			while (count > 0)
			{
				T object = std::move(objectPool.top());
				objectPool.pop();
				count--;
			}
		}

	protected:
		std::stack<T> objectPool;
		int objectsOutstanding;
	};
}