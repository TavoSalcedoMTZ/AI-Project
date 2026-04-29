#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSpawner.generated.h"

UCLASS()
class AIPROJECT_API APlayerSpawner : public AActor
{
    GENERATED_BODY()

public:
    APlayerSpawner();
    virtual void BeginPlay() override;

    void SpawnPlayer(int32 numPlayers);

    // ✅ Clase del personaje a spawnear (asignable desde Blueprint/Editor)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<ACharacter> PlayerCharacterClass;

    TMap<int32, APlayerController*> PlayerControllers;
};