#include "LevelManager/LevelManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"

void ULevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Inicializaciones iniciales si son necesarias
}

void ULevelManager::StartRound(int32 RoundNumber)
{
	// Ajustamos el índice (Ronda 1 es el índice 0 del Array)
	CurrentRoundIndex = RoundNumber - 1;
	EnemiesSpawnedThisRound = 0;

	// Verificamos que tengamos configuración para esta ronda
	if (RoundsConfig.IsValidIndex(CurrentRoundIndex))
	{
		float Rate = RoundsConfig[CurrentRoundIndex].SpawnRate;

		// Iniciamos el Timer que llama a SpawnEnemyRoutine cada 'Rate' segundos
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

	// Si ya spawneamos todos los enemigos de esta ronda, detenemos el timer
	if (EnemiesSpawnedThisRound >= MaxEnemies)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(SpawnTimerHandle);
		}
		return;
	}

	// === LÓGICA DE PROBABILIDAD ===
	// Ronda real (CurrentRoundIndex + 1)
	bool bCanSpawnSpecialEnemy = (CurrentRoundIndex + 1) >= 5;

	bool bSpawnSpecial = false;

	if (bCanSpawnSpecialEnemy)
	{
		// Ratio 1 a 8 -> Generamos un número entre 1 y 9
		int32 RandomRoll = FMath::RandRange(1, 9);

		// Si cae 9 (o cualquier otro número específico), spawneamos el especial
		if (RandomRoll == 9)
		{
			bSpawnSpecial = true;
		}
	}

	if (bSpawnSpecial)
	{
		// TODO: Lógica para spawnear Enemigo Especial
		UE_LOG(LogTemp, Warning, TEXT("Spawneando Enemigo ESPECIAL"));
	}
	else
	{
		// TODO: Lógica para spawnear Enemigo Normal
		UE_LOG(LogTemp, Warning, TEXT("Spawneando Enemigo NORMAL"));
	}

	EnemiesSpawnedThisRound++;
	EnemiesAlive++;
}

void ULevelManager::OnEnemyKilled(int32 PlayerID, int32 MoneyReward)
{
	EnemiesAlive--;

	// Asignamos el dinero al jugador correspondiente
	if (PlayerID == 0) // Jugador 1
	{
		Player1Money += MoneyReward;
		UE_LOG(LogTemp, Warning, TEXT("Jugador 1 ganó %d de dinero. Total: %d"), MoneyReward, Player1Money);
	}
	else if (PlayerID == 1) // Jugador 2
	{
		Player2Money += MoneyReward;
		UE_LOG(LogTemp, Warning, TEXT("Jugador 2 ganó %d de dinero. Total: %d"), MoneyReward, Player2Money);
	}

	// Comprobar si la ronda terminó
	if (EnemiesSpawnedThisRound >= RoundsConfig[CurrentRoundIndex].TotalEnemies && EnemiesAlive <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("¡Ronda Completada!"));
		// Aquí podrías llamar a StartRound(CurrentRoundIndex + 2) tras un breve delay
	}
}