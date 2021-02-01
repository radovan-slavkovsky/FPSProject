
#include "FPSCharacter.h"


AFPSCharacter::AFPSCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    //create 1st person camera
    FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("1st Person camera"));
    check(FPSCameraComponent != nullptr);

    // attach camera
    FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent>(GetCapsuleComponent()));
    FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
    FPSCameraComponent->bUsePawnControlRotation = true;

    //1st person setup 
    FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
    check(FPSMesh != nullptr);
    FPSMesh->SetOnlyOwnerSee(true);
    FPSMesh->SetupAttachment(FPSCameraComponent);
    // some shadow off
    FPSMesh->bCastDynamicShadow = false;
    FPSMesh->CastShadow = false;
    // stop seing normal figure 
    GetMesh()->SetOwnerNoSee(true);

}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    check(GEngine != nullptr);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("!We are using FPSCharacter."));

}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
}

void AFPSCharacter::MoveForward(float Value)
{
    FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
    AddMovementInput(Direction, Value);
    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT(Direction));

}

void AFPSCharacter::MoveRight(float Value)
{
    FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
    AddMovementInput(Direction, Value);
   //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT(Direction"));
}

void AFPSCharacter::Fire() {
    if (ProjectileClass) {

        //get camera transform
        FVector CameraLocation;
        FRotator CameraRotation;
        GetActorEyesViewPoint(CameraLocation, CameraRotation);
        MuzzleOffset.Set(100.0f, 0.0f, 0.0f);
        // Transform MuzzleOffset from camera space to world space.
        FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
        FRotator MuzzleRotation = CameraRotation;
        MuzzleRotation.Pitch += 10.0f;

        UWorld* World = GetWorld();
        if (World) {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();

            AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
        }
    }
}

void AFPSCharacter::StartJump() {
    bPressedJump = true;
}

void AFPSCharacter::StopJump() {
    bPressedJump = false;
}