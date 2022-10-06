#pragma once

/// <summary>
/// Base class for objects that cannot be moved or copied, by default.
/// </summary>
class MoveOnlyObjBase
{
protected:
	MoveOnlyObjBase() { };

	MoveOnlyObjBase(const MoveOnlyObjBase&) = delete;
	MoveOnlyObjBase& operator=(const MoveOnlyObjBase&) = delete;

	MoveOnlyObjBase(MoveOnlyObjBase&&) = default;
	MoveOnlyObjBase& operator=(MoveOnlyObjBase&&) = default;
};