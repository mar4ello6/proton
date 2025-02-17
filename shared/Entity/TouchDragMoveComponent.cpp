#include "PlatformPrecomp.h"
#include "TouchDragMoveComponent.h"
#include "EntityUtils.h"

TouchDragMoveComponent::TouchDragMoveComponent()
{
	SetName("TouchDragMove");
}

TouchDragMoveComponent::~TouchDragMoveComponent()
{
}

void TouchDragMoveComponent::OnAdd(Entity* pEnt)
{
	EntityComponent::OnAdd(pEnt);
	m_pPos2d = &GetParent()->GetVar("pos2d")->GetVector2();
	m_pSize2d = &GetParent()->GetVar("size2d")->GetVector2();
	EntityComponent *pDragComponent = GetParent()->GetComponentByName("TouchDrag");
	
	if (!pDragComponent)
	{
		assert(0 && "TouchDragMoveComponent requires a TouchDragComponent to be added first");
		return;
	}
	pDragComponent->GetFunction("OnTouchDragUpdate")->sig_function.connect(1, boost::bind(&TouchDragMoveComponent::OnTouchDragUpdate, this, _1));
	GetParent()->GetFunction("OnOverStart")->sig_function.connect(1, boost::bind(&TouchDragMoveComponent::OnOverStart, this, _1));
	GetParent()->GetFunction("OnOverEnd")->sig_function.connect(1, boost::bind(&TouchDragMoveComponent::OnOverEnd, this, _1));

	//register to get updated every frame
	//GetParent()->GetFunction("OnInput")->sig_function.connect(1, boost::bind(&TouchDragComponent::OnInput, this, _1));

}


void TouchDragMoveComponent::OnRemove()
{
	EntityComponent::OnRemove();
}


void TouchDragMoveComponent::UpdateStatusMessage(string msg)
{
	
	int timeMS = 1000;

	Entity *pOldEnt = GetParent()->GetEntityByName("DebugText");
	if (pOldEnt)
	{
		pOldEnt->SetName("");
		pOldEnt->SetTaggedForDeletion();
	}

	Entity* pEnt = CreateTextLabelEntity(GetParent(), "DebugText", 0, 0, msg);
	SetupTextEntity(pEnt, FONT_LARGE, 0.66f);
	FadeOutAndKillEntity(pEnt, true, 100, timeMS);
}

void TouchDragMoveComponent::OnTouchDragUpdate(VariantList* pVList)
{
	CL_Vec2f vMovement = pVList->Get(1).GetVector2();

#ifdef _DEBUG
	//LogMsg("offset %s", PrintVector2(vMovement).c_str());
#endif
 	if (m_bIsDraggingLook)
	{
		*m_pPos2d += vMovement;
		UpdateStatusMessage("Pos: X: " + toString(m_pPos2d->x)+" Y: "+ toString(m_pPos2d->y));
	}
}

void TouchDragMoveComponent::OnOverStart(VariantList* pVList)
{
	m_bIsDraggingLook = true;

	//move it to the top layer
	GetParent()->GetParent()->MoveEntityToTopByAddress(GetParent());
	
}

void TouchDragMoveComponent::OnOverEnd(VariantList* pVList)
{
	m_bIsDraggingLook = false;
}
