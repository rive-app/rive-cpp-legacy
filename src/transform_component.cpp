#include "rive/transform_component.hpp"
#include "rive/world_transform_component.hpp"
#include "rive/shapes/clipping_shape.hpp"
#include "rive/math/vec2d.hpp"
#include "rive/constraints/constraint.hpp"

using namespace rive;

StatusCode TransformComponent::onAddedClean(CoreContext* context)
{
	m_ParentTransformComponent =
	    parent() != nullptr && parent()->is<WorldTransformComponent>()
	        ? parent()->as<WorldTransformComponent>()
	        : nullptr;
	return StatusCode::Ok;
}

void TransformComponent::buildDependencies()
{
	if (parent() != nullptr)
	{
		parent()->addDependent(this);
	}
}

void TransformComponent::markTransformDirty()
{
	if (!addDirt(ComponentDirt::Transform))
	{
		return;
	}
	markWorldTransformDirty();
}

void TransformComponent::updateTransform()
{
	if (rotation() != 0)
	{
		Mat2D::fromRotation(m_Transform, rotation());
	}
	else
	{
		Mat2D::identity(m_Transform);
	}
	m_Transform[4] = x();
	m_Transform[5] = y();
	Mat2D::scaleByValues(m_Transform, scaleX(), scaleY());
}

void TransformComponent::updateWorldTransform()
{
	if (m_ParentTransformComponent != nullptr)
	{
		Mat2D::multiply(m_WorldTransform,
		                m_ParentTransformComponent->m_WorldTransform,
		                m_Transform);
	}
	else
	{
		Mat2D::copy(m_WorldTransform, m_Transform);
	}

	for (auto constraint : m_Constraints)
	{
		constraint->constrain(this);
	}
}

void TransformComponent::update(ComponentDirt value)
{
	if (hasDirt(value, ComponentDirt::Transform))
	{
		updateTransform();
	}
	if (hasDirt(value, ComponentDirt::WorldTransform))
	{
		updateWorldTransform();
	}
	if (hasDirt(value, ComponentDirt::RenderOpacity))
	{
		m_RenderOpacity = opacity();
		if (m_ParentTransformComponent != nullptr)
		{
			m_RenderOpacity *= m_ParentTransformComponent->childOpacity();
		}
	}
}

const Mat2D& TransformComponent::transform() const { return m_Transform; }

Mat2D& TransformComponent::mutableTransform() { return m_Transform; }

void TransformComponent::rotationChanged() { markTransformDirty(); }
void TransformComponent::scaleXChanged() { markTransformDirty(); }
void TransformComponent::scaleYChanged() { markTransformDirty(); }

void TransformComponent::addConstraint(Constraint* constraint)
{
	m_Constraints.push_back(constraint);
}