// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "AI/SAICharacter.h"
#include "Attributes/SAttributeComponent.h"
#include "AI/SAIController.h"
#include "BrainComponent.h"
#include "Widgets/SWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/SPlayerState.h"
#include <Gamemodes/SGameModeBase.h>
#include <Actions/SActionComponent.h>
#include <ActionRougeLike/ActionRougeLike.h>
#include "Gamemodes/SMainGameMode.h"
#include "AIController.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
    AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
    ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
    InventoryComponent = CreateDefaultSubobject<USBaseInventoryComponent>("PlayerInventoryComp");
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    //fix an issue with collision on our capsule colliders
    //GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
    //added our own projectile channel

    GetMesh()->SetGenerateOverlapEvents(true);

    CreditsOnKill = 20;
    TimeToHitParameterName = "TimeToHit";
    DeathReward = 10;
}

/// <summary>
/// Could have the ID be set, what if 2 AIs hate each other?
/// </summary>
/// <returns></returns>
FGenericTeamId ASAICharacter::GetGenericTeamId() const
{
    return FGenericTeamId(AffiliationGroup);
}

void ASAICharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    //Add dynamic bind is known for not being found through unreal magic (AKA INTELLISENSE), its ok that it doesnt see it here
    AttributeComponent->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
    //AttributeComponent->OnRageChanged.AddDynamic(this, &ASAICharacter::OnRageChanged);
}

/// <summary>
/// TODO: gotta seperate this
/// </summary>
/// <param name="NewTarget"></param>
void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC) {
        //don't even need to null check this since we know for a fact this is valid. Cannot be null
        AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
    }
}

/// <summary>
/// Should it live here or in AIController
/// </summary>
/// <param name="InstigatorActor"></param>
/// <param name="OwningComp"></param>
/// <param name="NewHealth"></param>
/// <param name="Delta"></param>
void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    if (Delta < 0.0f)
    {
        //if we shoot at the AI the AI needs to know that someone shot at them and whoever shot our AI needs to become the designated target
        //theres a chance that an AI might shoot a friendly AI and they might agro on each other, will keep for now but might remove later
        if (InstigatorActor)
        {
            SetTargetActor(InstigatorActor);
        }

        if (ActiveHealthBar == nullptr) {
            //spawn ui, only spawn when there isnt already a widget
            ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
            //add it to viewport
            if (ActiveHealthBar)
            {
                //set attached actor (run event constructor)
                ActiveHealthBar->AttachedActor = this;
                ActiveHealthBar->AddToViewport();
            }
        }

        //hit flash
        GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParameterName, GetWorld()->TimeSeconds);

        if (NewHealth <= 0.0f)
        {
            //stop BT 
            AAIController* AIC = Cast<AAIController>(GetController());
            if (AIC)
            {
                AIC->GetBrainComponent()->StopLogic("Killed");
            }

            //rag doll
            GetMesh()->SetAllBodiesSimulatePhysics(true);
            //need to set physics and collision to prevent unwanted issues with things falling through floor, etc
            GetMesh()->SetCollisionProfileName("Ragdoll");


            //remove the stuff our ai leaves behind (like capsules, etc)
            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            //disable this char movement completely (gravity)
            GetCharacterMovement()->DisableMovement();

            //set lifespan
            SetLifeSpan(10.0f);

            //drop currency on death (spawn coin pickup)
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            SpawnParams.Instigator = this;

            const FTransform SpawnTM = FTransform(this->GetActorRotation(), this->GetActorLocation());
            
            //old prototype
            //on minion death event via SGameModeBase
            ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
            if (GM)
            {
                //GM->KillMinionEvent(InstigatorActor, CreditsOnKill);
            }

            //tell instigator if player that they got the kill and update a stat somewhere
            

            //first reduce count for enemy alive


            ASMainGameMode* LevelGameMode = Cast<ASMainGameMode>(GetWorld()->GetAuthGameMode());
            if (LevelGameMode)
            {
                //check enemy type, if normal enemy and part of objective. Update killcount
                //run objective enemy killed event
                //(we run a check objective function in this call btw)
                LevelGameMode->KillNormalEnemyEvent(InstigatorActor, CreditsOnKill, this);

            }
        }
    }
}



