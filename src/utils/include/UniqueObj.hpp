#pragma once

/// <summary>
/// Base class for objects that cannot be moved or copied, by default.
/// </summary>
class UniqueObjBase
{
protected:
	UniqueObjBase() { };

	UniqueObjBase(const UniqueObjBase&) = delete;
	UniqueObjBase(UniqueObjBase&&) = delete;
	UniqueObjBase& operator=(const UniqueObjBase&) = delete;
	UniqueObjBase& operator=(UniqueObjBase&&) = delete;
};