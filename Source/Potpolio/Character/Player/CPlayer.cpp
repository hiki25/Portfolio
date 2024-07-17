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




ACPlayer::ACPlayer()
{
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
	}

	//Katana
	CHelpers::GetAsset(&Katana,"/Game/Player/Katana/SK_Katana");
	CHelpers::CreateSceneComponent(this, &KatanaComponet, TEXT("Katana"), GetMesh());

	//CameraComponent, SpringArmComponent Upload in memory 
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	//SpringArmComp Setting
	SpringArmComp->SetupAttachment(GetMesh());
	SpringArmComp->SetRelativeRotation(FRotator(0, +90, 0));
	SpringArmComp->SetRelativeLocation(FVector(0, 0, 140));
	SpringArmComp->TargetArmLength = 200.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bEnableCameraLag = true;

	//CameraComp Setting
	CameraComp->SetupAttachment(SpringArmComp);

	//Player Setting
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);

	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
}

void ACPlayer::BeginPlay()
{
	Super::BeginPlay();

	//Attach Katana
	KatanaComponet->SetSkeletalMesh(Katana);
	KatanaComponet->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("KatanaSoket"));

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
	PlayerInputComponent->BindAction("AR", EInputEvent::IE_Pressed,this, &ACPlayer::OnAR);
	PlayerInputComponent->BindAction("Pistol", EInputEvent::IE_Pressed,this, &ACPlayer::OnPistol);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed,this, &ACPlayer::OnJump);

}




void ACPlayer::OnMoveForward(float Axis)
{
	FRotator ControlRotation = FRotator(0,GetControlRotation().Yaw,0);
	FVector Direction = FQuat(ControlRotation).GetForwardVector();
	AddMovementInput(Direction,Axis);
}

void ACPlayer::OnMoveRight(float Axis)
{
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
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void ACPlayer::OnWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
}

void ACPlayer::OnSword()
{
	CheckFalse(ActionComp->IsUnarmedMode());
}

void ACPlayer::OnAR()
{
}

void ACPlayer::OnPistol()
{
}

void ACPlayer::OnJump()
{
	StateComp->SetJumpMode();
}


void ACPlayer::Begin_Jump()
{
	MontageComp->PlayJump();
}

void ACPlayer::Begin_Parkour()
{
}

void ACPlayer::End_Jump()
{
	StateComp->SetIdleMode();
}

void ACPlayer::End_Parkour()
{
}


//Change State
void ACPlayer::OnStateTypeChanged(EStateType PreType, EStateType NewType)
{
	switch (NewType)
	{
	case EStateType::Jump:
	{
		Begin_Jump();
		break;
	}
	case EStateType::Parkour:
	{
		Begin_Parkour();
		break;
	}
	case EStateType::Hitted:
	{
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
