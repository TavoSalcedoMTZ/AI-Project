#include "MultiplayerGameMode.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

AMultiplayerGameMode::AMultiplayerGameMode()
{
    // Evita que Unreal spawne un pawn automatico para el Player 0
    DefaultPawnClass = nullptr;
}

void AMultiplayerGameMode::BeginPlay()
{
    Super::BeginPlay();
    // No llamar SpawnPlayers aqui, llamarlo desde Blueprint
}

void AMultiplayerGameMode::SpawnPlayers(int32 NumPlayers)
{
    if (bPlayersSpawned) return;
    bPlayersSpawned = true;

    UWorld* World = GetWorld();
    if (!World) return;

    UE_LOG(LogTemp, Warning, TEXT("SpawnPlayers llamado con %d jugadores"), NumPlayers);

    TSubclassOf<ACharacter> ClassToSpawn = PlayerCharacterClass
        ? PlayerCharacterClass
        : TSubclassOf<ACharacter>(ACharacter::StaticClass());

    UE_LOG(LogTemp, Warning, TEXT("PlayerCharacterClass valido: %s"),
        PlayerCharacterClass ? TEXT("SI") : TEXT("NO"));

    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerStarts);

    UE_LOG(LogTemp, Warning, TEXT("PlayerStarts encontrados: %d"), PlayerStarts.Num());

    PlayerControllers.Empty();

    for (int32 i = 0; i < NumPlayers; i++)
    {
        APlayerController* PC = nullptr;

        if (i == 0)
        {
            PC = UGameplayStatics::GetPlayerController(World, 0);
            if (PC && PC->GetPawn())
            {
                PC->GetPawn()->Destroy();
            }
        }
        else
        {

            PC = UGameplayStatics::CreatePlayer(World, -1, false);

            if (PC && PC->GetPawn())
            {
                PC->GetPawn()->Destroy();
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Player %d — PC valido: %s"),
            i, PC ? TEXT("SI") : TEXT("NO"));

        if (!PC) continue;

        FVector SpawnPos = SpawnLocation;
        FRotator SpawnRotation = FRotator::ZeroRotator;

        if (PlayerStarts.IsValidIndex(i))
        {
            SpawnPos = PlayerStarts[i]->GetActorLocation();
            SpawnRotation = PlayerStarts[i]->GetActorRotation();
        }
        else
        {
            SpawnPos = SpawnLocation + FVector(i * 200.f, 0.f, 0.f);
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ACharacter* NewCharacter = World->SpawnActor<ACharacter>(
            ClassToSpawn,
            SpawnPos,
            SpawnRotation,
            SpawnParams
        );

        UE_LOG(LogTemp, Warning, TEXT("Player %d — Character spawneado: %s"),
            i, NewCharacter ? TEXT("SI") : TEXT("NO"));

        if (NewCharacter)
        {
            PC->Possess(NewCharacter);
            PlayerControllers.Add(i, PC);
            UE_LOG(LogTemp, Warning, TEXT("Player %d — Possess exitoso"), i);
        }
    }
}
