#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "MultiplayerGameMode.generated.h"

UCLASS()
class AIPROJECT_API AMultiplayerGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMultiplayerGameMode();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SpawnPlayers(int32 NumPlayers);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<ACharacter> PlayerCharacterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    int32 NumberOfPlayers = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    FVector SpawnLocation;

    UPROPERTY(BlueprintReadOnly, Category = "Spawning")
    TMap<int32, APlayerController*> PlayerControllers;

private:
    bool bPlayersSpawned = false;
};