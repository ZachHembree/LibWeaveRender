#pragma once
#include <array>
#include "WinUtils.hpp"

namespace Weave
{
	/// <summary>
	/// An SPSC Win32 event buffer (single producer/single consumer). One thread writes messages from the Window thread 
	/// using AddMessage(). The other thread consumes GetMessages().
	/// </summary>
	class AsyncWin32Buffer
	{
	public:
		struct Message
		{
			HWND hWnd;
			uint msg;
			ulong wParam;
			slong lParam;
		};

		AsyncWin32Buffer() :
			readIndex(0),
			writeIndex(1)
		{ }
		
		/// <summary>
		/// Records a message in the buffer synchronously. To be called from the thread recording 
		/// messages only.
		/// </summary>
		template <typename... MsgArgTs>
		void AddMessage(MsgArgTs&&... args)
		{
			msgBuffers[writeIndex].EmplaceBack(args...);
		}

		/// <summary>
		/// Records a message in the buffer synchronously. To be called from the thread recording 
		/// messages only.
		/// </summary>
		void AddMessage(Message&& msg)
		{
			msgBuffers[writeIndex].Add(std::move(msg));
		}

		/// <summary>
		/// Returns an array of all messages buffered since this function was last called.
		/// Subsequent calls to this function will invalidate the previously returned array.
		/// To be called by the consuming thread only.
		/// </summary>
		const IDynamicArray<Message>& GetMessages()
		{
			msgBuffers[readIndex].Clear();
			const uint lastWrite = writeIndex;
			writeIndex = readIndex.load();
			readIndex = lastWrite;

			return msgBuffers[readIndex];
		}

	private:
		std::array<UniqueVector<Message>, 2> msgBuffers;
		std::atomic<uint> readIndex;
		std::atomic<uint> writeIndex;
	};
}