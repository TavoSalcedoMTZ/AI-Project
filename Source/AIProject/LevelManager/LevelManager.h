#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelManager.generated.h"

class UUpgradeItem; 

USTRUCT(BlueprintType)
struct FRoundData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round Settings")
	int32 TotalEnemies = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round Settings")
	float SpawnRate = 2.0f;
};

UCLASS()
class AIPROJECT_API ULevelManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level Manager|Economy")
	int32 SharedMoney = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Manager|Shop")
	TArray<TSubclassOf<UUpgradeItem>> ShopUpgradesClasses;


	UPROPERTY(BlueprintReadOnly, Category = "Level Manager|Shop")
	TArray<UUpgradeItem*> InstancedShopUpgrades;

	UFUNCTION(BlueprintCallable, Category = "Level Manager|Shop")
	void InitializeShop();

	UFUNCTION(BlueprintCallable, Category = "Level Manager|Shop")
	bool TryPurchaseUpgrade(UUpgradeItem* UpgradeToBuy);

	UFUNCTION(BlueprintCallable, Category = "Level Manager")
	void StartRound(int32 RoundNumber);

	UFUNCTION(BlueprintCallable, Category = "Level Manager")
	void OnEnemyKilled(int32 PlayerID, int32 MoneyReward);

	UFUNCTION()
	void SpawnEnemyRoutine();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Manager|Data")
	TArray<FRoundData> RoundsConfig;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	int32 CurrentRoundIndex = 0;
	int32 EnemiesSpawnedThisRound = 0;
	int32 EnemiesAlive = 0;
	FTimerHandle SpawnTimerHandle;
};