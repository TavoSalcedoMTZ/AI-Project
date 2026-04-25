#include "MultiplayerGameMode.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

AMultiplayerGameMode::AMultiplayerGameMode()
{
   
  //  DefaultPawnClass = ACharacter::StaticClass();
}

void AMultiplayerGameMode::BeginPlay()
{
    Super::BeginPlay();
    SpawnPlayers(NumberOfPlayers);
}

void AMultiplayerGameMode::SpawnPlayers(int32 NumPlayers)
{
    UWorld* World = GetWorld();
    if (!World) return;

    TSubclassOf<ACharacter> ClassToSpawn = PlayerCharacterClass
        ? PlayerCharacterClass
        : TSubclassOf<ACharacter>(ACharacter::StaticClass());

    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerStarts);

    for (int32 i = 0; i < NumPlayers; i++)
    {
        APlayerController* PC = nullptr;

        if (i == 0)
        {
            PC = UGameplayStatics::GetPlayerController(World, 0);
        }
        else
        {
            PC = UGameplayStatics::CreatePlayer(World, i, true);
        }

        if (!PC) continue;

        
     
        FRotator SpawnRotation = FRotator::ZeroRotator;

        if (PlayerStarts.IsValidIndex(i))
        {
            SpawnLocation = PlayerStarts[i]->GetActorLocation();
            SpawnRotation = PlayerStarts[i]->GetActorRotation();
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ACharacter* NewCharacter = World->SpawnActor<ACharacter>(
            ClassToSpawn,
            SpawnLocation,
            SpawnRotation,
            SpawnParams
        );

        if (NewCharacter)
        {
            PC->Possess(NewCharacter);
            PlayerControllers.Add(i, PC);
        }
    }
}