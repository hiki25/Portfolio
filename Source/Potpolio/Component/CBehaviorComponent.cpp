#include "Component/CBehaviorComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Player/CPlayer.h"

UCBehaviorComponent::UCBehaviorComponent()
{
	BehaviorKey = "BehaviorKey";
	PlayerKey = "PlayerKey";
	LocationKey = "LocationKey";
}


void UCBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCBehaviorComponent::SetBlackBoard(UBlackboardComponent* InBlackBoardComp)
{
	BlackboardComp = InBlackBoardComp;
}

bool UCBehaviorComponent::IsWaitMode()
{
	return GetType() == EBehaviorType ::Wait;
}

bool UCBehaviorComponent::IsApprochMode()
{
	return GetType() == EBehaviorType::Approch;
}

bool UCBehaviorComponent::IsActionMode()
{
	return GetType() == EBehaviorType::Action;
}

bool UCBehaviorComponent::IsPatrolMode()
{
	return GetType() == EBehaviorType::Patrol;
}

bool UCBehaviorComponent::IsHittedMode()
{
	return GetType() == EBehaviorType::Hitted;
}

bool UCBehaviorComponent::IsEscapeMode()
{
	return GetType() == EBehaviorType::Escape;
}

bool UCBehaviorComponent::ISReloadMode()
{
	return GetType() == EBehaviorType::Reload;;
}

void UCBehaviorComponent::SetWaitMode()
{
	ChangeType(EBehaviorType::Wait);
}

void UCBehaviorComponent::SetApprochMode()
{
	ChangeType(EBehaviorType::Approch);
}

void UCBehaviorComponent::SetActionMode()
{
	ChangeType(EBehaviorType::Action);
}

void UCBehaviorComponent::SetPatrolMode()
{
	ChangeType(EBehaviorType::Patrol);
}

void UCBehaviorComponent::SetHittedMode()
{
	ChangeType(EBehaviorType::Hitted);
}

void UCBehaviorComponent::SetEscapeMode()
{
	ChangeType(EBehaviorType::Escape);
}

void UCBehaviorComponent::SetReloadMode()
{
	ChangeType(EBehaviorType::Reload);
}

ACPlayer* UCBehaviorComponent::GetPlayerKey()
{
	return Cast<ACPlayer>(BlackboardComp->GetValueAsObject(PlayerKey));
}

FVector UCBehaviorComponent::GetLocationKey()
{
	return BlackboardComp->GetValueAsVector(LocationKey);
}

void UCBehaviorComponent::ChangeType(EBehaviorType NewType)
{
	BlackboardComp->SetValueAsEnum(BehaviorKey, (uint8)NewType);
}

EBehaviorType UCBehaviorComponent::GetType()
{
	return (EBehaviorType)BlackboardComp->GetValueAsEnum(BehaviorKey);
}


