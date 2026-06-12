#include "LevelManager/LevelManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "UpgradeItem.h" 
#include "Blueprint/UserWidget.h" 
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
	EnemiesAlive = 0; // Reset por seguridad

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

void ULevelManager::OnEnemyKilled(int32 PlayerID, int32 MoneyReward)
{
	EnemiesAlive--;
	SharedMoney += MoneyReward;

	UE_LOG(LogTemp, Warning, TEXT("El Jugador %d eliminó a un enemigo. Dinero global: %d"), PlayerID, SharedMoney);

	// Condición: Si ya spawneamos todos los de la ronda Y ya no queda ninguno vivo
	if (EnemiesSpawnedThisRound >= RoundsConfig[CurrentRoundIndex].TotalEnemies && EnemiesAlive <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("¡Ronda Completada!"));

		// LLAMAMOS A NUESTRA NUEVA FUNCIÓN
		OnRoundFinalized();
	}
}

// LÓGICA DE TIENDA Y ESPERA AUTOMÁTICA
void ULevelManager::OnRoundFinalized()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 1. Crear el Widget de la Tienda si asignamos la clase
	if (ShopWidgetClass)
	{
		// Usamos el World para instanciar el Widget
		CurrentShopWidget = CreateWidget<UUserWidget>(World, ShopWidgetClass);
		if (CurrentShopWidget)
		{
			CurrentShopWidget->AddToViewport();

			// Opcional: Podrías activar el mouse aquí si tu tienda requiere clics directos
			APlayerController* PC = World->GetFirstPlayerController();
			if(PC) { PC->SetShowMouseCursor(true); PC->SetInputMode(FInputModeGameAndUI()); }
		}
	}

	// 2. Iniciar el contador de 30 segundos para quitar la tienda y arrancar la siguiente ronda
	// El parámetro 'false' al final indica que NO es un timer en bucle (se ejecuta una sola vez)
	World->GetTimerManager().SetTimer(ShopPhaseTimerHandle, this, &ULevelManager::AdvanceToNextRound, 10.0f, false);
}

void ULevelManager::AdvanceToNextRound()
{
	// 1. Si el widget existe, lo removemos de la pantalla
	if (CurrentShopWidget)
	{
		CurrentShopWidget->RemoveFromParent();
		CurrentShopWidget = nullptr; // Limpiamos la referencia

		// Opcional: Si activaste el mouse, aquí lo desactivas
		if (UWorld* World = GetWorld()) {
		     APlayerController* PC = World->GetFirstPlayerController();
		     if(PC) { PC->SetShowMouseCursor(false); PC->SetInputMode(FInputModeGameOnly()); }
		}
	}

	// 2. Calculamos el siguiente número de ronda (CurrentRoundIndex es index 0, así que sumamos 2 para la siguiente ronda)
	int32 NextRoundNumber = CurrentRoundIndex + 2;

	UE_LOG(LogTemp, Warning, TEXT("Iniciando la Ronda %d..."), NextRoundNumber);

	// 3. Comenzamos de nuevo el proceso
	StartRound(NextRoundNumber);
}

void ULevelManager::SetSpawnArea(FVector NewCenter, float NewRadius)
{
	SpawnCenter = NewCenter;
	SpawnRadius = NewRadius;
}