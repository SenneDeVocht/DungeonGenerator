#include "pch.h"
#include "CameraMovement.h"

#include <Mage/Components/CameraComponent.h>
#include <Mage/Components/Transform.h>
#include <Mage/Scenegraph/GameObject.h>

CameraMovement::CameraMovement(Mage::Transform* target)
	: m_pTarget(target)
{}

void CameraMovement::Update()
{
	// lerp from own pos to target
	const glm::vec2 targetPos = m_pTarget->GetWorldPosition();
	const glm::vec2 ownPos = GetGameObject()->GetTransform()->GetWorldPosition();

	const glm::vec2 newPos = ownPos + (targetPos - ownPos) * m_snappyness;

	GetGameObject()->GetTransform()->SetWorldPosition(newPos);
}
