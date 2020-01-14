#pragma once

#include <memory>

// adapted from code posted by Pete

template <typename Base>
std::shared_ptr<Base> shared_from_base(std::enable_shared_from_this<Base> * base)
{
	return base->shared_from_this();
}

template <typename Base>
std::shared_ptr<const Base> shared_from_base(std::enable_shared_from_this<Base> const * base)
{
	return base->shared_from_this();
}

template <typename Derived>
std::shared_ptr<Derived> shared_from(Derived * ptr)
{
	return std::static_pointer_cast<Derived>(shared_from_base(ptr));
}