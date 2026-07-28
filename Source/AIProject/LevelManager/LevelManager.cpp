#include "LevelManager.h" // Asegúrate de que coincida con el nombre de tu archivo
#include "Engine/World.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "UpgradeItem.h" 
#include "NavigationSystem.h" 

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
		UpgradeToBuy->PurchaseCount++;
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
	if (RoundNumber > TotalRounds)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelManager: ¡Se completaron todas las rondas configuradas!"));
		return;
	}

	CurrentRoundIndex = RoundNumber - 1; // Ronda 1 = Index 0
	EnemiesSpawnedThisRound = 0;
	EnemiesAlive = 0; // Reset por seguridad

	// --- CÁLCULO DINÁMICO ---
	// Enemigos = Base (10) + (Ronda * 5)
	CurrentRoundTotalEnemies = BaseEnemies + (CurrentRoundIndex * 5);

	// SpawnRate = Base (2.0) - (Ronda * 0.25), limitado a un mínimo de 0.5f usando FMath::Max
	CurrentRoundSpawnRate = FMath::Max(0.5f, BaseSpawnRate - (CurrentRoundIndex * 0.25f));

	UE_LOG(LogTemp, Warning, TEXT("LevelManager: Iniciando Ronda %d | Enemigos: %d | Frecuencia: %f"), RoundNumber, CurrentRoundTotalEnemies, CurrentRoundSpawnRate);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ULevelManager::SpawnEnemyRoutine, CurrentRoundSpawnRate, true);
	}
}

void ULevelManager::SpawnEnemyRoutine()
{
	// Cambiamos la validación del Array por la nueva variable calculada
	if (EnemiesSpawnedThisRound >= CurrentRoundTotalEnemies)
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

	FVector SpawnLocation = SpawnCenter;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if (NavSystem)
	{
		FNavLocation RandomNavLocation;
		if (NavSystem->GetRandomPointInNavigableRadius(SpawnCenter, SpawnRadius, RandomNavLocation))
		{
			SpawnLocation = RandomNavLocation.Location;
			SpawnLocation.Z += 50.0f;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LevelManager: No se encontró un punto en el NavMesh. Generando en el centro."));
		}
	}

	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (bSpawnSpecial)
	{
		if (SpecialEnemyClass)
		{
			GetWorld()->SpawnActor<AActor>(SpecialEnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
		}
	}
	else
	{
		if (NormalEnemyClass)
		{
			GetWorld()->SpawnActor<AActor>(NormalEnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
		}
	}

	EnemiesSpawnedThisRound++;
	EnemiesAlive++;
}

void ULevelManager::OnEnemyKilled(int32 PlayerID, int32 MoneyReward, FVector DeathLocation)
{
	EnemiesAlive--;

	UE_LOG(LogTemp, Warning, TEXT("El Jugador %d eliminó a un enemigo. Quedan vivos: %d"), PlayerID, EnemiesAlive);

	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// --- LÓGICA DE FUENTE DE MONEDAS ---
		if (CoinClass)
		{
			int32 CoinsToSpawn = FMath::RandRange(3, 10);

			for (int32 i = 0; i < CoinsToSpawn; i++)
			{
				AActor* SpawnedCoin = World->SpawnActor<AActor>(CoinClass, DeathLocation, FRotator::ZeroRotator, SpawnParams);

				if (SpawnedCoin)
				{
					UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(SpawnedCoin->GetRootComponent());
					if (RootPrim && RootPrim->IsSimulatingPhysics())
					{
						FVector Impulse = FVector(FMath::RandRange(-300.f, 300.f), FMath::RandRange(-300.f, 300.f), FMath::RandRange(120.f, 150.f));
						RootPrim->AddImpulse(Impulse, NAME_None, true);
					}
				}
			}
		}

		// --- LÓGICA DE DROPEO DE POCIÓN (33% DE PROBABILIDAD) ---
		if (PotionClass)
		{
			int32 DropChance = FMath::RandRange(1, 100);
			if (DropChance <= 44) // 44% de probabilidad
			{
				AActor* SpawnedPotion = World->SpawnActor<AActor>(PotionClass, DeathLocation, FRotator::ZeroRotator, SpawnParams);
				if (SpawnedPotion)
				{
					UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(SpawnedPotion->GetRootComponent());
					if (RootPrim && RootPrim->IsSimulatingPhysics())
					{
						FVector Impulse = FVector(FMath::RandRange(-200.f, 200.f), FMath::RandRange(-200.f, 200.f), FMath::RandRange(100.f, 150.f));
						RootPrim->AddImpulse(Impulse, NAME_None, true);
					}
				}
			}
		}
	}

	// Condición actualizada para usar CurrentRoundTotalEnemies
	if (EnemiesSpawnedThisRound >= CurrentRoundTotalEnemies && EnemiesAlive <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("¡Ronda Completada!"));
		OnRoundFinalized();
	}
}

void ULevelManager::OnRoundFinalized()
{
	UWorld* World = GetWorld();
	if (!World) return;

	OnShopPhaseStartedEvent.Broadcast();

	World->GetTimerManager().SetTimer(ShopPhaseTimerHandle, this, &ULevelManager::AdvanceToNextRound, 10.0f, false);
}

void ULevelManager::AdvanceToNextRound()
{
	OnShopPhaseEndedEvent.Broadcast();

	int32 NextRoundNumber = CurrentRoundIndex + 2;

	UE_LOG(LogTemp, Warning, TEXT("Iniciando la Ronda %d..."), NextRoundNumber);

	StartRound(NextRoundNumber);
}

void ULevelManager::SetSpawnArea(FVector NewCenter, float NewRadius)
{
	SpawnCenter = NewCenter;
	SpawnRadius = NewRadius;
}