#include "bones/skin.hpp"
#include "bones/bone.hpp"
#include "bones/skinnable.hpp"
#include "bones/tendon.hpp"
#include "shapes/path_vertex.hpp"
#include "constraints/constraint.hpp"

using namespace rive;

Skin::~Skin() { delete[] m_BoneTransforms; }

StatusCode Skin::onAddedClean(CoreContext* context)
{
	m_WorldTransform[0] = xx();
	m_WorldTransform[1] = xy();
	m_WorldTransform[2] = yx();
	m_WorldTransform[3] = yy();
	m_WorldTransform[4] = tx();
	m_WorldTransform[5] = ty();

	m_Skinnable = Skinnable::from(parent());
	if (m_Skinnable == nullptr)
	{
		return StatusCode::MissingObject;
	}

	m_Skinnable->skin(this);

	return StatusCode::Ok;
}

void Skin::update(ComponentDirt value)
{
	int bidx = 6;
	Mat2D world;
	for (auto tendon : m_Tendons)
	{
		Mat2D::multiply(
		    world, tendon->bone()->worldTransform(), tendon->inverseBind());
		m_BoneTransforms[bidx++] = world[0];
		m_BoneTransforms[bidx++] = world[1];
		m_BoneTransforms[bidx++] = world[2];
		m_BoneTransforms[bidx++] = world[3];
		m_BoneTransforms[bidx++] = world[4];
		m_BoneTransforms[bidx++] = world[5];
	}
}

void Skin::buildDependencies()
{
	// depend on bones from tendons
	for (auto tendon : m_Tendons)
	{
		auto bone = tendon->bone();
		bone->addDependent(this);
		for (auto constraint : bone->peerConstraints())
		{
			constraint->parent()->addDependent(this);
		}
	}

	// Make sure no-one is calling this twice.
	assert(m_BoneTransforms == nullptr);
	// We can now init the bone buffer.
	auto size = (m_Tendons.size() + 1) * 6;
	m_BoneTransforms = new float[size];
	m_BoneTransforms[0] = 1;
	m_BoneTransforms[1] = 0;
	m_BoneTransforms[2] = 0;
	m_BoneTransforms[3] = 1;
	m_BoneTransforms[4] = 0;
	m_BoneTransforms[5] = 0;
}

void Skin::deform(std::vector<PathVertex*>& vertices)
{
	for (auto vertex : vertices)
	{
		vertex->deform(m_WorldTransform, m_BoneTransforms);
	}
}
void Skin::addTendon(Tendon* tendon) { m_Tendons.push_back(tendon); }

void Skin::onDirty(ComponentDirt dirt) { m_Skinnable->markSkinDirty(); }