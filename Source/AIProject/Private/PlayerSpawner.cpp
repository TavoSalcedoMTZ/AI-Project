#include "PlayerSpawner.h"
#include "GameFramework/Character.h"

APlayerSpawner::APlayerSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APlayerSpawner::BeginPlay()
{
    Super::BeginPlay();


    SpawnPlayer(2);
}

void APlayerSpawner::SpawnPlayer(int32 numPlayers)
{
  
    TSubclassOf<ACharacter> ClassToSpawn = PlayerCharacterClass
        ? PlayerCharacterClass
        : TSubclassOf<ACharacter>(ACharacter::StaticClass());

    for (int32 i = 0; i < numPlayers; i++)
    {
    
        APlayerController* NewPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i);

        if (i > 0)
        {
           
            NewPlayerController = UGameplayStatics::CreatePlayer(GetWorld(), i, true);
        }

        if (NewPlayerController)
        {
            FVector SpawnLocation = GetActorLocation() + FVector(i * 200.f, 0.f, 0.f);
            FRotator SpawnRotation = GetActorRotation();

      
            ACharacter* NewCharacter = GetWorld()->SpawnActor<ACharacter>(
                ClassToSpawn,
                SpawnLocation,
                SpawnRotation
            );

            if (NewCharacter)
            {
                NewPlayerController->Possess(NewCharacter);
                PlayerControllers.Add(i, NewPlayerController);
            }
        }
    }
}