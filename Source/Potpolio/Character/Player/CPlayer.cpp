#include "CPlayer.h"
#include "Global.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/CActionComponent.h"
#include "Component/CMontageComponent.h"
#include "Component/CAttributeComponent.h"
#include "Action/CActionData.h"
#include "Action/CDoAction.h"
#include "../Enemy/CEnemy.h"





ACPlayer::ACPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	TargetMax = -1.f;

	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	USkeletalMesh* MeshComp;
	ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT("/Game/Player/Samurai/SK_Samurai"));
	if (MeshAsset.Succeeded())
	{
		MeshComp = MeshAsset.Object;
		GetMesh()->SetSkeletalMesh(MeshComp);
	}

	//Component
	{
		//ActionComponet
		CHelpers::CreateActorComponent(this, &ActionComp, TEXT("ActionComp"));
		//MontageComponet
		CHelpers::CreateActorComponent(this, &MontageComp, TEXT("MontageComp"));
		//StateComponent
		CHelpers::CreateActorComponent(this, &StateComp, TEXT("StateComp"));
		//AttributeComponet
		CHelpers::CreateActorComponent(this, &AttributeComp, TEXT("Attribute"));
	}

	//CameraComponent, SpringArmComponent Upload in memory 
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	//SpringArmComp Setting
	SpringArmComp->SetupAttachment(GetMesh());
	SpringArmComp->SetRelativeRotation(FRotator(0, 90, 0));
	SpringArmComp->SetRelativeLocation(FVector(0, 0, 140));
	SpringArmComp->TargetArmLength = 200.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->ProbeChannel= ECollisionChannel::ECC_Camera;


	//CameraComp Setting
	CameraComp->SetupAttachment(SpringArmComp);

	//Player Setting
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;;
	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);

	GetCharacterMovement()->MaxWalkSpeed = AttributeComp->GetWalkpeed();


}

void ACPlayer::BeginPlay()
{
	Super::BeginPlay();

	//Bind Func
	StateComp->OnStateTypeChanged.AddDynamic(this,&ACPlayer::OnStateTypeChanged);
	
}

//Construction
void ACPlayer::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//Dynamic Material
	{
		//Hand Material
		UObject* HandsKatanaMatAsset = StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, TEXT("/Game/Player/Materials/Instance/MI_handskatana"));
		CheckNull(HandsKatanaMatAsset);
		UMaterialInstanceConstant* HandsKatanaMatConstant = Cast<UMaterialInstanceConstant>(HandsKatanaMatAsset);
		HandsKatanaMat = UMaterialInstanceDynamic::Create(HandsKatanaMatConstant, nullptr);
		Materials.Add(HandsKatanaMat);

		//CostumeMat Material
		UObject* CostumeMatAsset = StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, TEXT("/Game/Player/Materials/Instance/MI_costume"));
		CheckNull(CostumeMatAsset);
		UMaterialInstanceConstant* CostumeMatConstant = Cast<UMaterialInstanceConstant>(CostumeMatAsset);
		CostumeMat = UMaterialInstanceDynamic::Create(CostumeMatConstant, nullptr);
		Materials.Add(CostumeMat);

		//HeadMat Material
		UObject* HeadMatAsset = StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, TEXT("/Game/Player/Materials/Instance/MI_head"));
		CheckNull(HeadMatAsset);
		UMaterialInstanceConstant* HeadMatConstant = Cast<UMaterialInstanceConstant>(HeadMatAsset);
		HeadMat = UMaterialInstanceDynamic::Create(HeadMatConstant, nullptr);
		Materials.Add(HeadMat);

		//EyeMat Material
		UObject* EyeMatAsset = StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, TEXT("/Game/Player/Materials/Instance/MI_eye"));
		CheckNull(EyeMatAsset);
		UMaterialInstanceConstant* EyeMatConstant = Cast<UMaterialInstanceConstant>(EyeMatAsset);
		EyeMat = UMaterialInstanceDynamic::Create(EyeMatConstant, nullptr);
		Materials.Add(EyeMat);

		//HairMat Material
		UObject* HairMatAsset = StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, TEXT("/Game/Player/Materials/Instance/MI_hair"));
		CheckNull(HairMatAsset);
		UMaterialInstanceConstant* HairMatConstant = Cast<UMaterialInstanceConstant>(HairMatAsset);
		HairMat = UMaterialInstanceDynamic::Create(HairMatConstant, nullptr);
		Materials.Add(HairMat);

		//PantsMat Material
		UObject* PantsMatAsset = StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, TEXT("/Game/Player/Materials/Instance/MI_pants"));
		CheckNull(PantsMatAsset);
		UMaterialInstanceConstant* PantsMatConstant = Cast<UMaterialInstanceConstant>(PantsMatAsset);
		PantsMat = UMaterialInstanceDynamic::Create(PantsMatConstant, nullptr);
		Materials.Add(PantsMat);

		//JacketMat Material
		UObject* JacketMatAsset = StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, TEXT("/Game/Player/Materials/Instance/MI_jacket"));
		CheckNull(JacketMatAsset);
		UMaterialInstanceConstant* JacketMatConstant = Cast<UMaterialInstanceConstant>(JacketMatAsset);
		JacketMat = UMaterialInstanceDynamic::Create(JacketMatConstant, nullptr);
		Materials.Add(JacketMat);
	}

	for (int32 i = 0; i < Materials.Num(); i++)
	{
		GetMesh()->SetMaterial(i, Materials[i]);
	}

	
	
}

void ACPlayer::Tick(float DeltaSeconds)
{
	if (LockOnTarget)
	{

	if (1000.f < UKismetMathLibrary::Vector_Distance(GetActorLocation(), LockOnTarget->GetActorLocation()))
	{
		LockOnTarget->TagetWidgetOff();
		MostLearestActor = nullptr;
	}
	}

	CheckTrue(ActionComp->IsUnarmedMode());
	if (MostLearestActor)
	{
		LockOnTarget = MostLearestActor;
		FRotator ContorlRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockOnTarget->GetActorLocation());
		GetController()->SetControlRotation(ContorlRotator);
	}
	
}


void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//BindAxis
	PlayerInputComponent->BindAxis("MoveForward", this, &ACPlayer::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPlayer::OnMoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ACPlayer::OnLockUp);
	PlayerInputComponent->BindAxis("LookHorizone", this, &ACPlayer::OnLockRight);
	

	PlayerInputComponent->BindAction("Run",EInputEvent::IE_Pressed,this,&ACPlayer::OnRun);
	PlayerInputComponent->BindAction("Run",EInputEvent::IE_Released,this,&ACPlayer::OnWalk);
	PlayerInputComponent->BindAction("Sword", EInputEvent::IE_Pressed,this, &ACPlayer::OnSword);
	PlayerInputComponent->BindAction("Pistol", EInputEvent::IE_Pressed,this, &ACPlayer::OnPistol);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed,this, &ACPlayer::OnReload);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed,this, &ACPlayer::OnJump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released,this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("PrimaryAct", EInputEvent::IE_Pressed,this, &ACPlayer::OnPrimaryAct);
	PlayerInputComponent->BindAction("SecondaryAct", EInputEvent::IE_Pressed,this, &ACPlayer::OnSecondaryAct);
	PlayerInputComponent->BindAction("SecondaryAct", EInputEvent::IE_Released,this, &ACPlayer::OffSecondaryAct);
	PlayerInputComponent->BindAction("Target_On", EInputEvent::IE_Pressed,this, &ACPlayer::Target_On);

}

FGenericTeamId ACPlayer::GetGenericTeamId() const
{
	return FGenericTeamId();
}




void ACPlayer::OnMoveForward(float Axis)
{
	CheckFalse(AttributeComp->IsMove());

	FRotator ControlRotation = FRotator(0,GetControlRotation().Yaw,0);
	FVector Direction = FQuat(ControlRotation).GetForwardVector();
	AddMovementInput(Direction,Axis);
}

void ACPlayer::OnMoveRight(float Axis)
{
	CheckFalse(AttributeComp->IsMove());

	FRotator ControlRotation = FRotator(0, GetControlRotation().Yaw, 0);
	FVector Direction = FQuat(ControlRotation).GetRightVector();
	AddMovementInput(Direction, Axis);
}

void ACPlayer::OnLockUp(float Axix)
{
	AddControllerPitchInput(Axix);
}

void ACPlayer::OnLockRight(float Axix)
{
	AddControllerYawInput(Axix);

	
}


//Player Speed Setting
void ACPlayer::OnRun()
{
	CheckFalse(StateComp->IsIdleMode());
	if (ActionComp->IsSwordMode())
	{
	MontageComp->PlayEvade();
	StateComp->SetEvadeMode();
	FTimerDynamicDelegate Delegate;
	Delegate.BindUFunction(this,"OnStartRun");
	GetWorldTimerManager().SetTimer(RunTimer, Delegate,1.5f,false);
	return;
	}
	GetCharacterMovement()->MaxWalkSpeed = AttributeComp->GetSprintpeed();
}

void ACPlayer::OnStartRun()
{
	GetWorldTimerManager().ClearTimer(RunTimer);
	StopAnimMontage();
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = AttributeComp->GetSprintpeed();
}

void ACPlayer::OnWalk()
{
	GetWorldTimerManager().ClearTimer(RunTimer);
	if (ActionComp->IsSwordMode())
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
	GetCharacterMovement()->MaxWalkSpeed = AttributeComp->GetWalkpeed();
}

void ACPlayer::OnSword()
{
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->SetSwordMode();
}


void ACPlayer::OnPistol()
{
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->SetPistolMode();
}

void ACPlayer::OnPrimaryAct()
{
	ActionComp->DoAction();
}

void ACPlayer::OnSecondaryAct()
{
	CheckTrue(ActionComp->IsUnarmedMode())
	ActionComp->DoSubAction(true);
}

void ACPlayer::OffSecondaryAct()
{
	CheckTrue(ActionComp->IsUnarmedMode())
	ActionComp->DoSubAction(false);

}

void ACPlayer::OnJump()
{
	CheckFalse(StateComp->IsIdleMode());
	StateComp->SetJumpMode();
	Jump();
}

void ACPlayer::OnReload()
{
	CheckFalse(StateComp->IsIdleMode());
	CheckFalse(ActionComp->IsPistolMode());
	StateComp->SetReloadMode();
}

void ACPlayer::Begin_Reload()
{
	MontageComp->PlayReload();
}

void ACPlayer::Target_On()
{
	CheckTrue(ActionComp->IsUnarmedMode());

	FVector Center = GetActorLocation();
	FVector End = GetActorLocation() + FVector(0,0,10);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType;
	ObjectType.Add(EObjectTypeQuery::ObjectTypeQuery3);

	TArray<AActor*> IgnoreActor;
	IgnoreActor.Add(this);
	TArray<FHitResult> TraceResult;

	if (UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Center, End, 1000.f, ObjectType, false, IgnoreActor, EDrawDebugTrace::ForDuration, TraceResult, true))
	{
		for (const auto& Result : TraceResult)
		{
			TargetActors.AddUnique(Result.Actor.Get());
		}

		for (const auto& Actor : TargetActors)
		{ 
			float Dot = GetActorLocation() | Actor->GetActorLocation();
			if (Dot >= TargetMax)
			{
				TargetMax = Dot;
				MostLearestActor = Cast<ACEnemy>(Actor);
			}
		}
		if (MostLearestActor)
		{
			LockOnTarget = MostLearestActor;
			LockOnTarget->TagetWidgetOn();
		}
	}

	
}



void ACPlayer::End_Jump()
{
	StateComp->SetIdleMode();
}

void ACPlayer::End_Parkour()
{
}

void ACPlayer::End_Reload()
{
	StateComp->SetIdleMode();
}

float ACPlayer::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageValue = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	DamageInstigator = EventInstigator;

	//ActionComp->Abort();
	AttributeComp->OnDecreseHealth(Damage);

	if (AttributeComp->GetCurrentHealth() <= 0)
	{
		StateComp->SetDeadMode();
		return 0.0f;
	}

	StateComp->SetHittedMode();

	return DamageValue;
}



void ACPlayer::Hitted()
{
	MontageComp->PlayHitted();
}

//Change State
void ACPlayer::OnStateTypeChanged(EStateType PreType, EStateType NewType)
{
	switch (NewType)
	{
	case EStateType::Reload:
	{
		Begin_Reload();
		break;
	}
	case EStateType::Hitted:
	{
		Hitted();
		break;
	}
	case EStateType::Dead:
	{
		break;
	}
	default:
		break;
	}
}
