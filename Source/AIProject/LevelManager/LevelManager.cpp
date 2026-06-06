#include "LevelManager/LevelManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "UpgradeItem.h" 

void ULevelManager::InitializeShop()
{
	InstancedShopUpgrades.Empty();

	for (TSubclassOf<UUpgradeItem> UpgradeClass : ShopUpgradesClasses)
	{
		if (UpgradeClass)
		{
			UUpgradeItem* NewUpgradeInstance = NewObject<UUpgradeItem>(this, UpgradeClass);
			if (NewUpgradeInstance)
			{
				InstancedShopUpgrades.Add(NewUpgradeInstance);
			}
		}
	}
}

bool ULevelManager::TryPurchaseUpgrade(UUpgradeItem* UpgradeToBuy)
{
	if (!UpgradeToBuy) return false;

	if (UpgradeToBuy->CanPurchase(this, InstancedShopUpgrades))
	{
		SharedMoney -= UpgradeToBuy->Cost;

		UpgradeToBuy->bIsPurchased = true;

		UpgradeToBuy->OnUpgradePurchased();

		return true; 
	}

	return false;
}

void ULevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void ULevelManager::StartRound(int32 RoundNumber)
{
	CurrentRoundIndex = RoundNumber - 1;
	EnemiesSpawnedThisRound = 0;

	if (RoundsConfig.IsValidIndex(CurrentRoundIndex))
	{
		float Rate = RoundsConfig[CurrentRoundIndex].SpawnRate;

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ULevelManager::SpawnEnemyRoutine, Rate, true);
		}
	}
}

void ULevelManager::SpawnEnemyRoutine()
{
	if (!RoundsConfig.IsValidIndex(CurrentRoundIndex)) return;

	int32 MaxEnemies = RoundsConfig[CurrentRoundIndex].TotalEnemies;

	if (EnemiesSpawnedThisRound >= MaxEnemies)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(SpawnTimerHandle);
		}
		return;
	}

	bool bCanSpawnSpecialEnemy = (CurrentRoundIndex + 1) >= 5;

	bool bSpawnSpecial = false;

	if (bCanSpawnSpecialEnemy)
	{
		int32 RandomRoll = FMath::RandRange(1, 9);

		if (RandomRoll == 9)
		{
			bSpawnSpecial = true;
		}
	}

	if (bSpawnSpecial)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawneando Enemigo ESPECIAL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawneando Enemigo NORMAL"));
	}

	EnemiesSpawnedThisRound++;
	EnemiesAlive++;
}

void ULevelManager::OnEnemyKilled(int32 PlayerID, int32 MoneyReward)
{
	EnemiesAlive--;

	SharedMoney += MoneyReward;

	UE_LOG(LogTemp, Warning, TEXT("El Jugador %d eliminó a un enemigo. Dinero global: %d"), PlayerID, SharedMoney);

	if (EnemiesSpawnedThisRound >= RoundsConfig[CurrentRoundIndex].TotalEnemies && EnemiesAlive <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("¡Ronda Completada!"));
	}
}