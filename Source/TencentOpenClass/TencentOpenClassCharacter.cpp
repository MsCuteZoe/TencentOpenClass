// Copyright Epic Games, Inc. All Rights Reserved.

#include "TencentOpenClassCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "LOLAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ATencentOpenClassCharacter

ATencentOpenClassCharacter::ATencentOpenClassCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AttributeSet = CreateDefaultSubobject<ULOLAttributeSet>(TEXT("AttributeSet"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATencentOpenClassCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATencentOpenClassCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATencentOpenClassCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATencentOpenClassCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATencentOpenClassCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATencentOpenClassCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATencentOpenClassCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATencentOpenClassCharacter::OnResetVR);
}

void ATencentOpenClassCharacter::BeginPlay() {
	Super::BeginPlay();
	if (AbilitySystemComp != nullptr)
	{
		//初始化技能
		if (PreloadedAbilities.Num() > 0)
		{
			for (auto i = 0; i < PreloadedAbilities.Num(); i++)
			{
				if (PreloadedAbilities[i] != nullptr)
				{
					// 为什么PreloadedAbilities[i]后面要用->运算符？
					// 因为
					if (PreloadedAbilities[i]->IsChildOf(UGameplayAbility::StaticClass())) {
						
					}
					// FGameplayAbilitySpec是GA的实例，其构造函数的第二个参数代表GA的等级，这里暂令其全部为1
					AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(PreloadedAbilities[i].GetDefaultObject(), 1));
				}
			}
		}

		//初始化ASC
		AbilitySystemComp->InitAbilityActorInfo(this, this);
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetHealthAttribute())
			.AddUObject(this, &ATencentOpenClassCharacter::HandleHealthChanged);

		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetManaAttribute())
			.AddUObject(this, &ATencentOpenClassCharacter::HandleManaChanged);

		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetMaxHealthAttribute())
			.AddUObject(this, &ATencentOpenClassCharacter::HandleHealthChanged);

		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetMaxManaAttribute())
			.AddUObject(this, &ATencentOpenClassCharacter::HandleManaChanged);

		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetSpeedAttribute())
			.AddUObject(this, &ATencentOpenClassCharacter::HandleSpeedChanged);
	}
	
}

void ATencentOpenClassCharacter::PreInitializeComponents() {
	Super::PreInitializeComponents();
	
}


UAbilitySystemComponent* ATencentOpenClassCharacter::GetAbilitySystemComponent() const {
	return AbilitySystemComp;
}
 
void ATencentOpenClassCharacter::GiveAbility(TSubclassOf<UGameplayAbility> Ability) {
	if (AbilitySystemComp) {
		if (HasAuthority() && Ability) {
			AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(Ability.GetDefaultObject(), 1));
		}
	}
}


void ATencentOpenClassCharacter::HandleHealthChanged(const FOnAttributeChangeData& Data) {
	OnHealthChanged.Broadcast(Data.NewValue);
}

void ATencentOpenClassCharacter::HandleManaChanged(const FOnAttributeChangeData& Data) {
	OnManaChanged.Broadcast(Data.NewValue);
}

void ATencentOpenClassCharacter::HandleSpeedChanged(const FOnAttributeChangeData& Data) {
	OnSpeedChanged.Broadcast(Data.NewValue);
}

void ATencentOpenClassCharacter::HandleInputMovement() {
	OnInputMovement.Broadcast();
}

void ATencentOpenClassCharacter::OnResetVR()
{
	// If TencentOpenClass is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in TencentOpenClass.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATencentOpenClassCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ATencentOpenClassCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ATencentOpenClassCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATencentOpenClassCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATencentOpenClassCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		HandleInputMovement();
	}
}

void ATencentOpenClassCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
		
		HandleInputMovement();
	}
}
