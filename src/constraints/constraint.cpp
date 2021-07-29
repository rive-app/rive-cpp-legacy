#include "constraints/constraint.hpp"
#include "container_component.hpp"
#include "transform_component.hpp"
#include "core_context.hpp"
#include "artboard.hpp"
#include "math/mat2d.hpp"

using namespace rive;

StatusCode Constraint::onAddedClean(CoreContext* context)
{
	if (!parent()->is<TransformComponent>())
	{
		return StatusCode::InvalidObject;
	}

	parent()->as<TransformComponent>()->addConstraint(this);

	return StatusCode::Ok;
}

void Constraint::markConstraintDirty()
{
	parent()->as<TransformComponent>()->markTransformDirty();
}

void Constraint::strengthChanged() { markConstraintDirty(); }

void Constraint::buildDependencies()
{
	Super::buildDependencies();
	parent()->addDependent(this);
}

void Constraint::onDirty(ComponentDirt dirt)
{
	// Whenever the constraint gets any dirt, make sure to mark the constrained
	// component dirty.
	markConstraintDirty();
}

static Mat2D identity;
const Mat2D& rive::getParentWorld(const TransformComponent& component)
{
	auto parent = component.parent();
	if (parent->is<Artboard>())
	{
		// TODO: when we have symbols working artboards will need to store their
		// world transform (probably should just become TransformComponent).
		return identity;
	}
	else
	{
		return parent->as<TransformComponent>()->worldTransform();
	}
}